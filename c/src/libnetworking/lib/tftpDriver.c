/*
 * Trivial File Transfer Protocol (RFC 1350)
 *
 * Transfer file to/from remote host
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 *
 *  $Id$
 */

#include <stdio.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/rtems_bsdnet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*
 * Range of UDP ports to try
 */
#define UDP_PORT_BASE	3180

/*
 * Pathname prefix
 */
#define TFTP_PATHNAME_PREFIX	"/TFTP/"

/*
 * Default limits
 */
#define PACKET_REPLY_MILLISECONDS	6000
#define OPEN_RETRY_LIMIT	10
#define IO_RETRY_LIMIT		10

/*
 * TFTP opcodes
 */
#define TFTP_OPCODE_RRQ		1
#define TFTP_OPCODE_WRQ		2
#define TFTP_OPCODE_DATA	3
#define TFTP_OPCODE_ACK		4
#define TFTP_OPCODE_ERROR	5

/*
 * Largest data transfer
 */
#define TFTP_BUFSIZE	512

/*
 * Packets transferred between machines
 */
union tftpPacket {
	/*
	 * RRQ/WRQ packet
	 */
	struct tftpRWRQ {
		rtems_unsigned16	opcode;
		char			filename_mode[TFTP_BUFSIZE];
	} tftpRWRQ;

	/*
	 * DATA packet
	 */
	struct tftpDATA {
		rtems_unsigned16	opcode;
		rtems_unsigned16	blocknum;
		rtems_unsigned8		data[TFTP_BUFSIZE];
	} tftpDATA;

	/*
	 * ACK packet
	 */
	struct tftpACK {
		rtems_unsigned16	opcode;
		rtems_unsigned16	blocknum;
	} tftpACK;

	/*
	 * ERROR packet
	 */
	struct tftpERROR {
		rtems_unsigned16	opcode;
		rtems_unsigned16	errorCode;
		char			errorMessage[TFTP_BUFSIZE];
	} tftpERROR;
};

/*
 * State of each TFTP stream
 */
struct tftpStream {
	/*
	 * Buffer for storing most recently-received packet
	 */
	union tftpPacket	pkbuf;

	/*
	 * Last block number received
	 */
	rtems_unsigned16	blocknum;

	/*
	 * Data transfer socket
	 */
	int			socket;
	struct sockaddr_in	myAddress;
	struct sockaddr_in	farAddress;

	/*
	 * Indices into buffer
	 */
	int	nleft;
	int	nused;

	/*
	 * Flags
	 */
	int	firstReply;
	int	eof;
};

/*
 * Number of streams open at the same time
 */
static rtems_id tftp_mutex;
static int nStreams;
static struct tftpStream ** volatile tftpStreams;

/*
 * Initialize the TFTP driver
 */
/* XXX change name to rtems_bsdnet_initialize_tftp_filesystem ("mountpt") */
/* XXX this won't be a driver when we are finished */
rtems_device_driver rtems_tftp_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
	rtems_status_code sc;

	sc = rtems_semaphore_create (rtems_build_name('T', 'F', 'T', 'P'),
					1,
					RTEMS_FIFO |
						RTEMS_BINARY_SEMAPHORE |
						RTEMS_NO_INHERIT_PRIORITY |
						RTEMS_NO_PRIORITY_CEILING |
						RTEMS_LOCAL,
					0,
					&tftp_mutex);
	if (sc != RTEMS_SUCCESSFUL)
		return sc;
/* XXX change to a mount */
	rtems_io_register_name (TFTP_PATHNAME_PREFIX, major, minor);
	return RTEMS_SUCCESSFUL;
}

/*
 * Set error message
 * This RTEMS/UNIX error mapping needs to be fixed!
 */
static void
tftpSetErrno (struct tftpStream *tp)
{
	unsigned int tftpError;
	static const int errorMap[] = {
		0,
		ENOENT,
		EPERM,
		ENOSPC,
		EINVAL,
		ENXIO,
		EEXIST,
		ESRCH,
		0,
	};

	tftpError = ntohs (tp->pkbuf.tftpERROR.errorCode);
	if (tftpError < (sizeof errorMap / sizeof errorMap[0]))
		errno = errorMap[tftpError];
	else
		errno = 1000 + tftpError;
}

/*
 * Send a message to make the other end shut up
 */
static void
sendStifle (struct tftpStream *tp, struct sockaddr_in *to)
{
	int len;

	/*
	 * Create the error packet (Unknown transfer ID).
	 */
	tp->pkbuf.tftpERROR.opcode = htons (TFTP_OPCODE_ERROR);
	tp->pkbuf.tftpERROR.errorCode = htons (5);
	len = sizeof tp->pkbuf.tftpERROR.opcode +
				sizeof tp->pkbuf.tftpERROR.errorCode + 1;
	len += sprintf (tp->pkbuf.tftpERROR.errorMessage, "GO AWAY"); 

	/*
	 * Send it
	 */
	sendto (tp->socket, (char *)&tp->pkbuf, len, 0,
					(struct sockaddr *)to, sizeof *to);
}

/*
 * Wait for a data packet
 */
static int
getPacket (struct tftpStream *tp)
{
	int len;
	struct timeval tv;

	tv.tv_sec = 6;
	tv.tv_usec = 0;
	setsockopt (tp->socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);
	for (;;) {
		union {
			struct sockaddr s;
			struct sockaddr_in i;
		} from;
		int fromlen = sizeof from;
		len = recvfrom (tp->socket, (char *)&tp->pkbuf,
							sizeof tp->pkbuf, 0,
							&from.s, &fromlen);
		if (len < 0)
			break;
		if (from.i.sin_addr.s_addr == tp->farAddress.sin_addr.s_addr) {
			if (tp->firstReply) {
				tp->firstReply = 0;
				tp->farAddress.sin_port = from.i.sin_port;
			}
			if (tp->farAddress.sin_port == from.i.sin_port) 
				break;
		}

		/*
		 * Packet is from someone with whom we are
		 * not interested.  Tell them to go away.
		 */
		sendStifle (tp, &from.i);
	}
	tv.tv_sec = 0;
	setsockopt (tp->socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);
	return len;
}

/*
 * Send an acknowledgement
 */
static int
sendAck (struct tftpStream *tp)
{
	/*
	 * Create the acknowledgement
	 */
	tp->pkbuf.tftpACK.opcode = htons (TFTP_OPCODE_ACK);
	tp->pkbuf.tftpACK.blocknum = htons (tp->blocknum);

	/*
	 * Send it
	 */
	if (sendto (tp->socket, (char *)&tp->pkbuf, sizeof tp->pkbuf.tftpACK, 0,
					(struct sockaddr *)&tp->farAddress,
					sizeof tp->farAddress) < 0)
		return errno;
	return 0;
}

/*
 * Release a stream and clear the pointer to it
 */
static void
releaseStream (int s)
{
	rtems_semaphore_obtain (tftp_mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	free (tftpStreams[s]);
	tftpStreams[s] = NULL;
	rtems_semaphore_release (tftp_mutex);
}

/*
 * Open a TFTP stream
 */
rtems_device_driver rtems_tftp_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
	rtems_libio_open_close_args_t *ap = pargp;
	struct tftpStream *tp;
	int retryCount;
	rtems_unsigned32 farAddress;
	int s;
	int len;
	char *cp1, *cp2;
	char *remoteFilename;
	rtems_interval now;
	rtems_status_code sc;

/* XXX change to eval_path/open */
	/*
	 * Read-only for now
	 */
	if (ap->flags & LIBIO_FLAGS_WRITE)
		return RTEMS_NOT_IMPLEMENTED;

	/*
	 * Pick apart the name into a host:pathname pair
	 */
	if (strlen (ap->iop->pathname) <= strlen (TFTP_PATHNAME_PREFIX))
		return RTEMS_INVALID_NAME;
	cp2 = ap->iop->pathname + strlen (TFTP_PATHNAME_PREFIX);
	if (*cp2 == '/') {
		farAddress = rtems_bsdnet_bootp_server_address.s_addr;
	}
	else {
		char *hostname;

		cp1 = cp2;
		while (*cp2 != '/') {
			if (*cp2 == '\0')
				return RTEMS_INVALID_NAME;
			cp2++;
		}
		len = cp2 - cp1;
		hostname = malloc (len + 1);
		if (hostname == NULL)
			return RTEMS_NO_MEMORY;
		strncpy (hostname, cp1, len);
		hostname[len] = '\0';
		farAddress = inet_addr (hostname);
		free (hostname);
	}
	if ((farAddress == 0) || (farAddress == ~0))
		return RTEMS_INVALID_NAME;
	if (*++cp2 == '\0')
		return RTEMS_INVALID_NAME;
	remoteFilename = cp2;
	if (strlen (remoteFilename) > (TFTP_BUFSIZE - 10))
		return RTEMS_INVALID_NAME;

	/*
	 * Find a free stream
	 */
	sc = rtems_semaphore_obtain (tftp_mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	if (sc != RTEMS_SUCCESSFUL)
		return sc;
	for (s = 0 ; s < nStreams ; s++) {
		if (tftpStreams[s] == NULL)
			break;
	}
	if (s == nStreams) {
		/*
		 * Reallocate stream pointers
		 * Guard against the case where realloc() returns NULL.
		 */
		struct tftpStream **np;

		np = realloc (tftpStreams, ++nStreams * sizeof *tftpStreams);
		if (np == NULL) {
			rtems_semaphore_release (tftp_mutex);
			return RTEMS_NO_MEMORY;
		}
		tftpStreams = np;
	}
	tp = tftpStreams[s] = malloc (sizeof (struct tftpStream));
	rtems_semaphore_release (tftp_mutex);
	if (tp == NULL)
		return RTEMS_NO_MEMORY;
	ap->iop->data0 = s;
	ap->iop->data1 = tp;

	/*
	 * Create the socket
	 */
	if ((tp->socket = socket (AF_INET, SOCK_DGRAM, 0)) < 0) {
		releaseStream (s);
		return RTEMS_TOO_MANY;
	}

	/*
	 * Bind the socket to a local address
	 */
	retryCount = 0;
	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &now);
	for (;;) {
		int try = (now + retryCount) % 10;

		tp->myAddress.sin_family = AF_INET;
		tp->myAddress.sin_port = htons (UDP_PORT_BASE + nStreams * try + minor);
		tp->myAddress.sin_addr.s_addr = htonl (INADDR_ANY);
		if (bind (tp->socket, (struct sockaddr *)&tp->myAddress, sizeof tp->myAddress) >= 0)
			break;
		if (++retryCount == 10) {
			close (tp->socket);
			releaseStream (minor);
			return RTEMS_RESOURCE_IN_USE;
		}
	}

	/*
	 * Set the UDP destination to the TFTP server
	 * port on the remote machine.
	 */
	tp->farAddress.sin_family = AF_INET;
	tp->farAddress.sin_addr.s_addr = farAddress;
	tp->farAddress.sin_port = htons (69);

	/*
	 * Start the transfer
	 */
	tp->firstReply = 1;
	for (;;) {
		/*
		 * Create the request
		 */
		tp->pkbuf.tftpRWRQ.opcode = htons (TFTP_OPCODE_RRQ);
		cp1 = tp->pkbuf.tftpRWRQ.filename_mode;
		cp2 = remoteFilename;
		while ((*cp1++ = *cp2++) != '\0')
			continue;
		cp2 = "octet";
		while ((*cp1++ = *cp2++) != '\0')
			continue;
		len = cp1 - (char *)&tp->pkbuf.tftpRWRQ;

		/*
		 * Send the request
		 */
		if (sendto (tp->socket, (char *)&tp->pkbuf, len, 0, 
					(struct sockaddr *)&tp->farAddress,
					sizeof tp->farAddress) < 0) {
			close (tp->socket);
			releaseStream (minor);
			return RTEMS_UNSATISFIED;
		}

		/*
		 * Get reply
		 */
		len = getPacket (tp);
		if (len >= (int) sizeof tp->pkbuf.tftpACK) {
			int opcode = ntohs (tp->pkbuf.tftpDATA.opcode);
			if ((opcode == TFTP_OPCODE_DATA)
			 && (ntohs (tp->pkbuf.tftpDATA.blocknum) == 1)) {
				tp->nused = 0;
				tp->blocknum = 1;
				tp->nleft = len - 2 * sizeof (rtems_unsigned16);
				tp->eof = (tp->nleft < TFTP_BUFSIZE);
				if (sendAck (tp) != 0) {
					close (tp->socket);
					releaseStream (minor);
					return RTEMS_UNSATISFIED;
				}
				break;
			}
			if (opcode == TFTP_OPCODE_ERROR) {
				tftpSetErrno (tp);
				close (tp->socket);
				releaseStream (ap->iop->data0);
				return RTEMS_INTERNAL_ERROR;
			}
		}

		/*
		 * Keep trying
		 */
		if (++retryCount >= OPEN_RETRY_LIMIT) {
			close (tp->socket);
			releaseStream (minor);
			return RTEMS_UNSATISFIED;
		}
	}
	return RTEMS_SUCCESSFUL;
}

/*
 * Read from a TFTP stream
 */
rtems_device_driver rtems_tftp_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
	rtems_libio_rw_args_t *ap = pargp;
	char *bp;
	struct tftpStream *tp;
	int retryCount;
	int nwant;

	tp = ap->iop->data1;

	/*
	 * Read till user request is satisfied or EOF is reached
	 */
	bp = ap->buffer;
	nwant = ap->count;
	while (nwant) {
		if (tp->nleft) {
			int count;
			if (nwant < tp->nleft)
				count = nwant;
			else
				count = tp->nleft;
			memcpy (bp, &tp->pkbuf.tftpDATA.data[tp->nused], count);
			tp->nused += count;
			tp->nleft -= count;
			bp += count;
			nwant -= count;
			if (nwant == 0)
				break;
		}
		if (tp->eof)
			break;

		/*
		 * Wait for the next packet
		 */
		retryCount = 0;
		for (;;) {
			int len = getPacket (tp);
			if (len >= (int)sizeof tp->pkbuf.tftpACK) {
				int opcode = ntohs (tp->pkbuf.tftpDATA.opcode);
				rtems_unsigned16 nextBlock = tp->blocknum + 1;
				if ((opcode == TFTP_OPCODE_DATA)
				 && (ntohs (tp->pkbuf.tftpDATA.blocknum) == nextBlock)) {
					tp->nused = 0;
					tp->nleft = len - 2 * sizeof (rtems_unsigned16);
					tp->eof = (tp->nleft < TFTP_BUFSIZE);
					tp->blocknum++;
					if (sendAck (tp) != 0)
						return RTEMS_IO_ERROR;
					break;
				}
				if (opcode == TFTP_OPCODE_ERROR) {
					tftpSetErrno (tp);
					return RTEMS_INTERNAL_ERROR;
				}
			}

			/*
			 * Keep trying?
			 */
			if (++retryCount == IO_RETRY_LIMIT)
				return RTEMS_IO_ERROR;
			if (sendAck (tp) != 0)
				return RTEMS_IO_ERROR;
		}
	}
	ap->bytes_moved = ap->count - nwant;
	return RTEMS_SUCCESSFUL;
}

/*
 * Close a TFTP stream
 */
rtems_device_driver rtems_tftp_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
	rtems_libio_open_close_args_t *ap = pargp;
	struct tftpStream *tp = ap->iop->data1;;

	if (!tp->eof && !tp->firstReply) {
		/*
		 * Tell the other end to stop
		 */
		rtems_interval ticksPerSecond;
		sendStifle (tp, &tp->farAddress);
		rtems_clock_get (RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticksPerSecond);
		rtems_task_wake_after (1 + ticksPerSecond / 10);
	}
	close (tp->socket);
	releaseStream (ap->iop->data0);
	return RTEMS_SUCCESSFUL;
}

rtems_device_driver rtems_tftp_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
	return RTEMS_NOT_CONFIGURED;
}

rtems_device_driver rtems_tftp_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
	return RTEMS_NOT_CONFIGURED;
}
