/*
 * vim: set expandtab tabstop=4 shiftwidth=4 ai :
 * 
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
 *
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
#include <rtems/libio_.h>
#include <rtems/rtems_bsdnet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef set_errno_and_return_minus_one
#define set_errno_and_return_minus_one( _error ) \
  do { errno = (_error); return -1; } while(0)
#endif

#ifdef RTEMS_TFTP_DRIVER_DEBUG
int rtems_tftp_driver_debug = 1;
#endif

/*
 * Range of UDP ports to try
 */
#define UDP_PORT_BASE        3180

/*
 * Pathname prefix
 */
#define TFTP_PATHNAME_PREFIX "/TFTP/"

/*
 * Root node_access value
 * By using the address of a local static variable
 * we ensure a unique value for this identifier.
 */
#define ROOT_NODE_ACCESS    (&tftp_mutex)

/*
 * Default limits
 */
#define PACKET_FIRST_TIMEOUT_MILLISECONDS  400
#define PACKET_TIMEOUT_MILLISECONDS        6000
#define OPEN_RETRY_LIMIT                   10
#define IO_RETRY_LIMIT                     10

/*
 * TFTP opcodes
 */
#define TFTP_OPCODE_RRQ     1
#define TFTP_OPCODE_WRQ     2
#define TFTP_OPCODE_DATA    3
#define TFTP_OPCODE_ACK     4
#define TFTP_OPCODE_ERROR   5

/*
 * Largest data transfer
 */
#define TFTP_BUFSIZE        512

/*
 * Packets transferred between machines
 */
union tftpPacket {
    /*
     * RRQ/WRQ packet
     */
    struct tftpRWRQ {
        rtems_unsigned16    opcode;
        char                filename_mode[TFTP_BUFSIZE];
    } tftpRWRQ;

    /*
     * DATA packet
     */
    struct tftpDATA {
        rtems_unsigned16    opcode;
        rtems_unsigned16    blocknum;
        rtems_unsigned8     data[TFTP_BUFSIZE];
    } tftpDATA;

    /*
     * ACK packet
     */
    struct tftpACK {
        rtems_unsigned16    opcode;
        rtems_unsigned16    blocknum;
    } tftpACK;

    /*
     * ERROR packet
     */
    struct tftpERROR {
        rtems_unsigned16    opcode;
        rtems_unsigned16    errorCode;
        char                errorMessage[TFTP_BUFSIZE];
    } tftpERROR;
};

/*
 * State of each TFTP stream
 */
struct tftpStream {
    /*
     * Buffer for storing most recently-received packet
     */
    union tftpPacket    pkbuf;

    /*
     * Last block number transferred
     */
    rtems_unsigned16    blocknum;

    /*
     * Data transfer socket
     */
    int                 socket;
    struct sockaddr_in  myAddress;
    struct sockaddr_in  farAddress;

    /*
     * Indices into buffer
     */
    int     nleft;
    int     nused;

    /*
     * Flags
     */
    int     firstReply;
    int     eof;
    int     writing;
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

static int rtems_tftp_mount_me(
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
  temp_mt_entry->mt_fs_root.node_access = ROOT_NODE_ACCESS;

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
            RTEMS_FILESYSTEM_READ_WRITE,
            NULL,
            TFTP_PATHNAME_PREFIX
    );

    if ( status )
        perror( "TFTP mount failed" );

    return status;
}

/*
 * Map error message
 */
static int
tftpErrno (struct tftpStream *tp)
{
    unsigned int tftpError;
    static const int errorMap[] = {
        EINVAL,
        ENOENT,
        EPERM,
        ENOSPC,
        EINVAL,
        ENXIO,
        EEXIST,
        ESRCH,
    };

    tftpError = ntohs (tp->pkbuf.tftpERROR.errorCode);
    if (tftpError < (sizeof errorMap / sizeof errorMap[0]))
        return errorMap[tftpError];
    else
        return 1000 + tftpError;
}

/*
 * Send a message to make the other end shut up
 */
static void
sendStifle (struct tftpStream *tp, struct sockaddr_in *to)
{
    int len;
    struct {
        rtems_unsigned16    opcode;
        rtems_unsigned16    errorCode;
        char                errorMessage[12];
    } msg;

    /*
     * Create the error packet (Unknown transfer ID).
     */
    msg.opcode = htons (TFTP_OPCODE_ERROR);
    msg.errorCode = htons (5);
    len = sizeof msg.opcode + sizeof msg.errorCode + 1;
    len += sprintf (msg.errorMessage, "GO AWAY"); 

    /*
     * Send it
     */
    sendto (tp->socket, (char *)&msg, len, 0, (struct sockaddr *)to, sizeof *to);
}

/*
 * Wait for a data packet
 */
static int
getPacket (struct tftpStream *tp, int retryCount)
{
    int len;
    struct timeval tv;

    if (retryCount == 0) {
        tv.tv_sec = PACKET_FIRST_TIMEOUT_MILLISECONDS / 1000;
        tv.tv_usec = (PACKET_FIRST_TIMEOUT_MILLISECONDS % 1000) * 1000;
    }
    else {
        tv.tv_sec = PACKET_TIMEOUT_MILLISECONDS / 1000;
        tv.tv_usec = (PACKET_TIMEOUT_MILLISECONDS % 1000) * 1000;
    }
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
    tv.tv_usec = 0;
    setsockopt (tp->socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);
#ifdef RTEMS_TFTP_DRIVER_DEBUG
    if (rtems_tftp_driver_debug) {
        if (len >= (int) sizeof tp->pkbuf.tftpACK) {
            int opcode = ntohs (tp->pkbuf.tftpDATA.opcode);
            switch (opcode) {
            default:
                printf ("TFTP: OPCODE %d\n", opcode);
                break;

            case TFTP_OPCODE_DATA:
                printf ("TFTP: RECV %d\n", ntohs (tp->pkbuf.tftpDATA.blocknum));
                break;

            case TFTP_OPCODE_ACK:
                printf ("TFTP: GOT ACK %d\n", ntohs (tp->pkbuf.tftpACK.blocknum));
                break;
            }
        }
        else {
            printf ("TFTP: %d-byte packet\n", len);
        }
    }
#endif
    return len;
}

/*
 * Send an acknowledgement
 */
static int
sendAck (struct tftpStream *tp)
{
#ifdef RTEMS_TFTP_DRIVER_DEBUG
    if (rtems_tftp_driver_debug)
        printf ("TFTP: ACK %d\n", tp->blocknum);
#endif

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

static int rtems_tftp_evaluate_for_make(
   const char                         *path,       /* IN     */
   rtems_filesystem_location_info_t   *pathloc,    /* IN/OUT */
   const char                        **name        /* OUT    */
)
{  
  pathloc->node_access = NULL;
  set_errno_and_return_minus_one( EIO );    
}

/*
 * Convert a path to canonical form
 */
static void
fixPath (char *path)
{
    char *inp, *outp, *base;

    outp = inp = path;
    base = NULL;
    for (;;) {
        if (inp[0] == '.') {
            if (inp[1] == '\0')
                break;
            if (inp[1] == '/') {
                inp += 2;
                continue;
            }
            if (inp[1] == '.') {
                if (inp[2] == '\0') {
                    if ((base != NULL) && (outp > base)) {
                        outp--;
                        while ((outp > base) && (outp[-1] != '/'))
                            outp--;
                    }
                    break;
                }
                if (inp[2] == '/') {
                    inp += 3;
                    if (base == NULL)
                        continue;
                    if (outp > base) {
                        outp--;
                        while ((outp > base) && (outp[-1] != '/'))
                            outp--;
                    }
                    continue;
                }
            }
        }
        if (base == NULL)
            base = inp;
        while (inp[0] != '/') {
            if ((*outp++ = *inp++) == '\0')
                return;
        }
        *outp++ = '/';
        while (inp[0] == '/')
            inp++;
    }
    *outp = '\0';
    return;
}

static int rtems_tftp_eval_path(  
  const char                        *pathname,     /* IN     */
  int                                flags,        /* IN     */
  rtems_filesystem_location_info_t  *pathloc       /* IN/OUT */
)
{
    pathloc->handlers    = &rtems_tftp_handlers;

    /*
     * Hack to provide the illusion of directories inside the TFTP file system.
     * Paths ending in a / are assumed to be directories.
     */
    if (pathname[strlen(pathname)-1] == '/') {
        int isRelative = (pathloc->node_access != ROOT_NODE_ACCESS);
        char *cp;
        
        /*
         * Reject attempts to open() directories
         */
        if (flags & RTEMS_LIBIO_PERMS_RDWR)
            set_errno_and_return_minus_one( EISDIR );
        if (isRelative) {
            cp = malloc (strlen(pathloc->node_access)+strlen(pathname)+1);
            if (cp == NULL)
                set_errno_and_return_minus_one( ENOMEM );
            strcpy (cp, pathloc->node_access);
            strcat (cp, pathname);
        }
        else {
            cp = strdup (pathname);
            if (cp == NULL)
                set_errno_and_return_minus_one( ENOMEM );
        }
        fixPath (cp);
        pathloc->node_access = cp;
        return 0;
    }
    if (pathloc->node_access != ROOT_NODE_ACCESS)
        pathloc->node_access = 0;

    /*
     * Reject it if it's not read-only or write-only.
     */
    flags &= RTEMS_LIBIO_PERMS_READ | RTEMS_LIBIO_PERMS_WRITE;
    if ((flags != RTEMS_LIBIO_PERMS_READ) && (flags != RTEMS_LIBIO_PERMS_WRITE) )
        set_errno_and_return_minus_one( EINVAL );
    return 0;
}

/*
 * The routine which does most of the work for the IMFS open handler
 */
static int rtems_tftp_open_worker(
    rtems_libio_t *iop,
    char          *full_path_name,
    unsigned32     flags,
    unsigned32     mode
)
{
    struct tftpStream    *tp;
    int                  retryCount;
    struct in_addr       farAddress;
    int                  s;
    int                  len;
    char                 *cp1;
    char                 *cp2;
    char                 *remoteFilename;
    rtems_interval       now;
    rtems_status_code    sc;
    char                 *hostname;

    /*
     * Extract the host name component
     */
    cp2 = full_path_name;
    while (*cp2 == '/')
        cp2++;
    hostname = cp2;
    while (*cp2 != '/') {
        if (*cp2 == '\0')
            return ENOENT;
        cp2++;
    }
    *cp2++ = '\0';

    /*
     * Convert hostname to Internet address
     */
    if (strcmp (hostname, "BOOTP_HOST") == 0)
        farAddress = rtems_bsdnet_bootp_server_address;
    else
        farAddress.s_addr = inet_addr (hostname);
    if ((farAddress.s_addr == 0) || (farAddress.s_addr == ~0))
        return ENOENT;

    /*
     * Extract file pathname component
     */
    while (*cp2 == '/') 
        cp2++;
    if (strcmp (cp2, "BOOTP_FILE") == 0) {
        cp2 = rtems_bsdnet_bootp_boot_file_name;
        while (*cp2 == '/') 
            cp2++;
    }
    if (*cp2 == '\0')
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
    tp->farAddress.sin_addr = farAddress;
    tp->farAddress.sin_port = htons (69);

    /*
     * Start the transfer
     */
    tp->firstReply = 1;
    retryCount = 0;
    for (;;) {
        /*
         * Create the request
         */
        if ((flags & O_ACCMODE) == O_RDONLY) {
            tp->writing = 0;
            tp->pkbuf.tftpRWRQ.opcode = htons (TFTP_OPCODE_RRQ);
        }
        else {
            tp->writing = 1;
            tp->pkbuf.tftpRWRQ.opcode = htons (TFTP_OPCODE_WRQ);
        }
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
        len = getPacket (tp, retryCount);
        if (len >= (int) sizeof tp->pkbuf.tftpACK) {
            int opcode = ntohs (tp->pkbuf.tftpDATA.opcode);
            if (!tp->writing
             && (opcode == TFTP_OPCODE_DATA)
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
            if (tp->writing
             && (opcode == TFTP_OPCODE_ACK)
             && ((ntohs (tp->pkbuf.tftpACK.blocknum) == 0)
              || (ntohs (tp->pkbuf.tftpACK.blocknum) == 1))) {
                tp->nused = 0;
                tp->blocknum = ntohs (tp->pkbuf.tftpACK.blocknum);
                break;
            }
            if (opcode == TFTP_OPCODE_ERROR) {
                int e = tftpErrno (tp);
                close (tp->socket);
                releaseStream (s);
                return e;
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
 * The IMFS open handler
 */
static int rtems_tftp_open(
    rtems_libio_t *iop,
    const char    *new_name,
    unsigned32     flags,
    unsigned32     mode
)
{
    char *full_path_name;
    char *s1;
    int err;

    /*
     * Tack the `current directory' on to relative paths.
     * We know that the current directory ends in a / character.
     */
    if (*new_name == '/') {
        /*
         * Skip the TFTP filesystem prefix.
         */
        int len = strlen (TFTP_PATHNAME_PREFIX);
        if (strncmp (new_name, TFTP_PATHNAME_PREFIX, len))
            return ENOENT;
        new_name += len;
        s1 = "";
    }
    else {
        s1 = rtems_filesystem_current.node_access;
    }
    full_path_name = malloc (strlen (s1) + strlen (new_name) + 1);
    if (full_path_name == NULL)
        return ENOMEM;
    strcpy (full_path_name, s1);
    strcat (full_path_name, new_name);
    fixPath (full_path_name);
    err = rtems_tftp_open_worker (iop, full_path_name, flags, mode);
    free (full_path_name);
    return err;
}

/*
 * Read from a TFTP stream
 */
static int rtems_tftp_read(
    rtems_libio_t *iop,
    void          *buffer,
    unsigned32    count
)
{
    char              *bp;
    struct tftpStream *tp = iop->data1;
    int               retryCount;
    int               nwant;


    /*
     * Read till user request is satisfied or EOF is reached
     */
    bp = buffer;
    nwant = count;
    while (nwant) {
        if (tp->nleft) {
            int ncopy;
            if (nwant < tp->nleft)
                ncopy = nwant;
            else
                ncopy = tp->nleft;
            memcpy (bp, &tp->pkbuf.tftpDATA.data[tp->nused], ncopy);
            tp->nused += ncopy;
            tp->nleft -= ncopy;
            bp += ncopy;
            nwant -= ncopy;
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
            int len = getPacket (tp, retryCount);
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
                if (opcode == TFTP_OPCODE_ERROR)
                        set_errno_and_return_minus_one( tftpErrno (tp) );
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
    return count - nwant;
}

/*
 * Flush a write buffer and wait for acknowledgement
 */
static int rtems_tftp_flush ( struct tftpStream *tp )
{
    int wlen, rlen;
    int retryCount = 0;

    wlen = tp->nused + 2 * sizeof (rtems_unsigned16);
    for (;;) {
        tp->pkbuf.tftpDATA.opcode = htons (TFTP_OPCODE_DATA);
        tp->pkbuf.tftpDATA.blocknum = htons (tp->blocknum);
#ifdef RTEMS_TFTP_DRIVER_DEBUG
        if (rtems_tftp_driver_debug)
            printf ("TFTP: SEND %d (%d)\n", tp->blocknum, tp->nused);
#endif
        if (sendto (tp->socket, (char *)&tp->pkbuf, wlen, 0, 
                                        (struct sockaddr *)&tp->farAddress,
                                        sizeof tp->farAddress) < 0)
            return EIO;
        rlen = getPacket (tp, retryCount);
        /*
         * Our last packet won't necessarily be acknowledged!
         */
        if ((rlen < 0) && (tp->nused < sizeof tp->pkbuf.tftpDATA.data))
                return 0;
        if (rlen >= (int)sizeof tp->pkbuf.tftpACK) {
            int opcode = ntohs (tp->pkbuf.tftpACK.opcode);
            if ((opcode == TFTP_OPCODE_ACK)
             && (ntohs (tp->pkbuf.tftpACK.blocknum) == (tp->blocknum + 1))) {
                tp->nused = 0;
                tp->blocknum++;
                return 0;
            }
            if (opcode == TFTP_OPCODE_ERROR)
                return tftpErrno (tp);
        }

        /*
         * Keep trying?
         */
        if (++retryCount == IO_RETRY_LIMIT)
            return EIO;
    }
}

/*
 * Close a TFTP stream
 */
static int rtems_tftp_close(
    rtems_libio_t *iop
)
{
    struct tftpStream *tp = iop->data1;;

    if (tp->writing)
        rtems_tftp_flush (tp);
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

static int rtems_tftp_write(
    rtems_libio_t   *iop,
    const void      *buffer,
    unsigned32      count
)
{
    const char        *bp;
    struct tftpStream *tp = iop->data1;
    int               nleft, nfree, ncopy;

    /*
     * Bail out if an error has occurred
     */
    if (!tp->writing)
        return EIO;


    /*
     * Write till user request is satisfied
     * Notice that the buffer is flushed as soon as it is filled rather
     * than waiting for the next write or a close.  This ensures that
     * the flush in close writes a less than full buffer so the far
     * end can detect the end-of-file condition.
     */
    bp = buffer;
    nleft = count;
    while (nleft) {
        nfree = sizeof tp->pkbuf.tftpDATA.data - tp->nused;
        if (nleft < nfree)
            ncopy = nleft;
        else
            ncopy = nfree;
        memcpy (&tp->pkbuf.tftpDATA.data[tp->nused], bp, ncopy);
        tp->nused += ncopy;
        nleft -= ncopy;
        bp += ncopy;
        if (tp->nused == sizeof tp->pkbuf.tftpDATA.data) {
            int e = rtems_tftp_flush (tp);
            if (e) {
                tp->writing = 0;
                set_errno_and_return_minus_one (e);
            }
        }
    }
    return count;
}

/*
 * Dummy version to let fopen(xxxx,"w") work properly.
 */
static int rtems_tftp_ftruncate(
    rtems_libio_t   *iop,
    off_t           count
)
{
    return 0;
}

static rtems_filesystem_node_types_t rtems_tftp_node_type(
     rtems_filesystem_location_info_t        *pathloc                 /* IN */
)
{
    if ((pathloc->node_access == NULL)
     || (pathloc->node_access == ROOT_NODE_ACCESS))
        return RTEMS_FILESYSTEM_MEMORY_FILE;
    return RTEMS_FILESYSTEM_DIRECTORY;
}

static int rtems_tftp_free_node_info(
     rtems_filesystem_location_info_t        *pathloc                 /* IN */
)
{
    if (pathloc->node_access && (pathloc->node_access != ROOT_NODE_ACCESS)) {
        free (pathloc->node_access);
        pathloc->node_access = NULL;
    }
    return 0;
}


rtems_filesystem_operations_table  rtems_tftp_ops = {
    rtems_tftp_eval_path,            /* eval_path */
    rtems_tftp_evaluate_for_make,    /* evaluate_for_make */
    NULL,                            /* link */
    NULL,                            /* unlink */
    rtems_tftp_node_type,            /* node_type */
    NULL,                            /* mknod */
    NULL,                            /* chown */
    rtems_tftp_free_node_info,       /* freenodinfo */
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
    rtems_tftp_open,   /* open */     
    rtems_tftp_close,  /* close */    
    rtems_tftp_read,   /* read */     
    rtems_tftp_write,  /* write */    
    NULL,              /* ioctl */    
    NULL,              /* lseek */    
    NULL,              /* fstat */    
    NULL,              /* fchmod */   
    rtems_tftp_ftruncate, /* ftruncate */
    NULL,              /* fpathconf */
    NULL,              /* fsync */    
    NULL,              /* fdatasync */
    NULL,              /* fcntl */
    NULL               /* rmnod */
};
