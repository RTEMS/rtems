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
#include <stdlib.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/rtems_bsdnet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef set_errno_and_return_minus_one
#define set_errno_and_return_minus_one( _error ) \
  do { errno = (_error); return -1; } while(0)
#endif


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

typedef const char *tftp_node;
extern rtems_filesystem_operations_table  rtems_tftp_ops;
extern rtems_filesystem_file_handlers_r   rtems_tftp_handlers;

/*
 *  Direct copy from the IMFS.  Look at this.
 */

rtems_filesystem_limits_and_options_t rtems_tftp_limits_and_options = {
   5,   /* link_max */
   6,   /* max_canon */
   7,   /* max_input */
   255, /* name_max */
   255, /* path_max */
   2,   /* pipe_buf */
   1,   /* posix_async_io */
   2,   /* posix_chown_restrictions */
   3,   /* posix_no_trunc */
   4,   /* posix_prio_io */
   5,   /* posix_sync_io */
   6    /* posix_vdisable */
};

int rtems_tftp_mount_me(
  rtems_filesystem_mount_table_entry_t *temp_mt_entry
)
{
  rtems_status_code  sc;

  temp_mt_entry->mt_fs_root.handlers = &rtems_tftp_handlers;
  temp_mt_entry->mt_fs_root.ops      = &rtems_tftp_ops;

  /*
   *   We have no tftp filesystem specific data to maintain.  This
   *   filesystem may only be mounted ONCE.
   *
   *   And we maintain no real filesystem nodes, so there is no real root.
   */

  temp_mt_entry->fs_info                = NULL;
  temp_mt_entry->mt_fs_root.node_access = NULL;

  /*
   *  These need to be looked at for full POSIX semantics.
   */

  temp_mt_entry->pathconf_limits_and_options = rtems_tftp_limits_and_options; 


  /*
   *  Now allocate a semaphore for mutual exclusion.
   *
   *  NOTE:  This could be in an fsinfo for this filesystem type.
   */
  
  sc = rtems_semaphore_create (
    rtems_build_name('T', 'F', 'T', 'P'),
    1,
    RTEMS_FIFO |
    RTEMS_BINARY_SEMAPHORE |
    RTEMS_NO_INHERIT_PRIORITY |
    RTEMS_NO_PRIORITY_CEILING |
    RTEMS_LOCAL,
    0,
    &tftp_mutex
  );

  if (sc != RTEMS_SUCCESSFUL)
    set_errno_and_return_minus_one( ENOMEM ); 

  return 0;
}

/*
 * Initialize the TFTP driver
 */

int rtems_bsdnet_initialize_tftp_filesystem () 
{
 int                                   status;
 rtems_filesystem_mount_table_entry_t *entry;

 status = mkdir( TFTP_PATHNAME_PREFIX, S_IRWXU | S_IRWXG | S_IRWXO );
 if ( status == -1 )
   return status; 

  status = mount( 
     &entry,
     &rtems_tftp_ops,
     RTEMS_FILESYSTEM_READ_ONLY,
     NULL,
     TFTP_PATHNAME_PREFIX
  );

  if ( status )
    perror( "TFTP mount failed" );

  return status;
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

int rtems_tftp_evaluate_for_make(
   const char                         *path,       /* IN     */
   rtems_filesystem_location_info_t   *pathloc,    /* IN/OUT */
   const char                        **name        /* OUT    */
)
{  
  set_errno_and_return_minus_one( EIO );    
}

/*
 * XXX - Fix return values.
 */

int rtems_tftp_eval_path(  
  const char                        *pathname,     /* IN     */
  int                                flags,        /* IN     */
  rtems_filesystem_location_info_t  *pathloc       /* IN/OUT */
)
{

  /*
   * Read-only for now
   */
   
  if ( (flags & O_WRONLY) == O_WRONLY )
    set_errno_and_return_minus_one( ENOENT );

  /*
   * The File system is mounted at TFTP_PATHNAME_PREFIX
   * the caller of this routine has striped off this part of the
   * name. Save the remainder of the name for use by the open routine.
   */

  pathloc->node_access = (void * ) pathname;
  pathloc->handlers    = &rtems_tftp_handlers;

  return 0;
}


int rtems_tftp_open(
  rtems_libio_t *iop,
  const char    *new_name,
  unsigned32     flag,
  unsigned32     mode
)
{
  struct tftpStream  *tp;
  int                 retryCount;
  rtems_unsigned32    farAddress;
  int                 s;
  int                 len;
  char               *cp1;
  char               *cp2;
  char               *remoteFilename;
  rtems_interval      now;
  rtems_status_code   sc;
  char               *hostname;

  /*
   * This came from the evaluate path. 
   */

  cp2 = iop->file_info;  

  cp1 = cp2;
  while (*cp2 != '/') {
    if (*cp2 == '\0')
      return ENOENT;
    cp2++;
  }

  len = cp2 - cp1;
  hostname = malloc (len + 1);
  if (hostname == NULL)
    return ENOMEM; 

  strncpy (hostname, cp1, len);
  hostname[len] = '\0';
  farAddress = inet_addr (hostname);
  free (hostname);

  if ((farAddress == 0) || (farAddress == ~0))
    return ENOENT;

  if (*++cp2 == '\0')
    return ENOENT;

  remoteFilename = cp2;
  if (strlen (remoteFilename) > (TFTP_BUFSIZE - 10))
    return ENOENT;    

  /*
   * Find a free stream
   */

  sc = rtems_semaphore_obtain (tftp_mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL)
    return EBUSY;

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
      return ENOMEM;
    }
    tftpStreams = np;
  }

  tp = tftpStreams[s] = malloc (sizeof (struct tftpStream));
  rtems_semaphore_release (tftp_mutex);
  if (tp == NULL)
    return ENOMEM;
  iop->data0 = s;
  iop->data1 = tp;

  /*
   * Create the socket
   */

  if ((tp->socket = socket (AF_INET, SOCK_DGRAM, 0)) < 0) {
    releaseStream (s);
    return ENOMEM;
  }

  /*
   * Bind the socket to a local address
   */

  retryCount = 0;
  rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &now);
  for (;;) {
    int try = (now + retryCount) % 10;

    tp->myAddress.sin_family = AF_INET;
    tp->myAddress.sin_port = htons (UDP_PORT_BASE + nStreams * try + s);
    tp->myAddress.sin_addr.s_addr = htonl (INADDR_ANY);
    if (bind (tp->socket, (struct sockaddr *)&tp->myAddress, sizeof tp->myAddress) >= 0)
      break;
    if (++retryCount == 10) {
      close (tp->socket);
      releaseStream (s);
      return EBUSY;
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
    cp1 = (char *) tp->pkbuf.tftpRWRQ.filename_mode;
    cp2 = (char *) remoteFilename;
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
      releaseStream (s);
      return EIO;
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
          releaseStream (s);
          return EIO;
        }
        break;
      }
      if (opcode == TFTP_OPCODE_ERROR) {
        tftpSetErrno (tp);
        close (tp->socket);
        releaseStream (s);
        return EIO;
      }
    }

    /*
     * Keep trying
     */
    if (++retryCount >= OPEN_RETRY_LIMIT) {
      close (tp->socket);
      releaseStream (s);
      return EIO;
    }
  }

  return 0;
}

/*
 * Read from a TFTP stream
 */

int rtems_tftp_read(
  rtems_libio_t *iop,
  void          *buffer,
  unsigned32     count
)
{
  char              *bp;
  struct tftpStream *tp;
  int                retryCount;
  int                nwant;

  tp = iop->data1;

  /*
   * Read till user request is satisfied or EOF is reached
   */

  bp = buffer;
  nwant = count;
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
            set_errno_and_return_minus_one( EIO );
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
        set_errno_and_return_minus_one( EIO );
      if (sendAck (tp) != 0)
        set_errno_and_return_minus_one( EIO );
    }
  }

 /*
  * XXX - Eric is this right?
  *
  */
  return count - nwant;
}

/*
 * Close a TFTP stream
 */
int rtems_tftp_close(
  rtems_libio_t *iop
)
{
  struct tftpStream *tp = iop->data1;;

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
  releaseStream (iop->data0);
  return RTEMS_SUCCESSFUL;
}

int rtems_tftp_write(
  rtems_libio_t *iop,
  const void    *buffer,
  unsigned32     count
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

rtems_filesystem_node_types_t rtems_tftp_node_type(
   rtems_filesystem_location_info_t    *pathloc         /* IN */
)
{
  return RTEMS_FILESYSTEM_MEMORY_FILE;
}


rtems_filesystem_operations_table  rtems_tftp_ops = {
  rtems_tftp_eval_path,            /* eval_path */
  rtems_tftp_evaluate_for_make,    /* evaluate_for_make */
  NULL,                            /* link */
  NULL,                            /* unlink */
  rtems_tftp_node_type,            /* node_type */
  NULL,                            /* mknod */
  NULL,                            /* chown */
  NULL,                            /* freenodinfo */
  NULL,                            /* mount */
  rtems_tftp_mount_me,             /* initialize */
  NULL,                            /* unmount */
  NULL,                            /* fsunmount */
  NULL,                            /* utime, */
  NULL,                            /* evaluate_link */
  NULL,                            /* symlink */
  NULL,                            /* readlin */
};

rtems_filesystem_file_handlers_r rtems_tftp_handlers = {
  rtems_tftp_open,
  rtems_tftp_close,
  rtems_tftp_read,
  rtems_tftp_write,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};
