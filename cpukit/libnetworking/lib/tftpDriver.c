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
 * Modifications to support reference counting in the file system are
 * Copyright (c) 2012 embedded brains GmbH.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <rtems.h>
#include <rtems/libio_.h>
#include <rtems/seterr.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/tftp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifdef RTEMS_TFTP_DRIVER_DEBUG
int rtems_tftp_driver_debug = 1;
#endif

/*
 * Range of UDP ports to try
 */
#define UDP_PORT_BASE        3180

/*
 * Default limits
 */
#define PACKET_FIRST_TIMEOUT_MILLISECONDS  400L
#define PACKET_TIMEOUT_MILLISECONDS        6000L
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
        uint16_t      opcode;
        char                filename_mode[TFTP_BUFSIZE];
    } tftpRWRQ;

    /*
     * DATA packet
     */
    struct tftpDATA {
        uint16_t      opcode;
        uint16_t      blocknum;
        uint8_t       data[TFTP_BUFSIZE];
    } tftpDATA;

    /*
     * ACK packet
     */
    struct tftpACK {
        uint16_t      opcode;
        uint16_t      blocknum;
    } tftpACK;

    /*
     * ERROR packet
     */
    struct tftpERROR {
        uint16_t      opcode;
        uint16_t      errorCode;
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
    uint16_t      blocknum;

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
 * Flags for filesystem info.
 */
#define TFTPFS_VERBOSE (1 << 0)

/*
 * TFTP File system info.
 */
typedef struct tftpfs_info_s {
  uint32_t flags;
  rtems_id tftp_mutex;
  int nStreams;
  struct tftpStream ** volatile tftpStreams;
} tftpfs_info_t;

#define tftpfs_info_mount_table(_mt) ((tftpfs_info_t*) ((_mt)->fs_info))
#define tftpfs_info_pathloc(_pl)     ((tftpfs_info_t*) ((_pl)->mt_entry->fs_info))
#define tftpfs_info_iop(_iop)        (tftpfs_info_pathloc (&((_iop)->pathinfo)))

/*
 * Number of streams open at the same time
 */

static const rtems_filesystem_operations_table  rtems_tftp_ops;
static const rtems_filesystem_file_handlers_r   rtems_tftp_handlers;

static bool rtems_tftp_is_directory(
    const char *path,
    size_t pathlen
)
{
    return path [pathlen - 1] == '/';
}

int rtems_tftpfs_initialize(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
)
{
  rtems_status_code  sc;
  const char *device = mt_entry->dev;
  size_t devicelen = strlen (device);
  tftpfs_info_t *fs;
  char *root_path;

  if (devicelen == 0)
      rtems_set_errno_and_return_minus_one (ENXIO);

  fs = malloc (sizeof (*fs));
  root_path = malloc (devicelen + 2);
  if (root_path == NULL || fs == NULL)
      goto error;

  root_path = memcpy (root_path, device, devicelen);
  root_path [devicelen] = '/';
  root_path [devicelen + 1] = '\0';

  fs->flags = 0;
  fs->nStreams = 0;
  fs->tftpStreams = 0;
  
  mt_entry->fs_info = fs;
  mt_entry->mt_fs_root->location.node_access = root_path;
  mt_entry->mt_fs_root->location.handlers = &rtems_tftp_handlers;
  mt_entry->ops = &rtems_tftp_ops;
  
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
    &fs->tftp_mutex
  );

  if (sc != RTEMS_SUCCESSFUL)
      goto error;

  if (data) {
      char* config = (char*) data;
      char* token;
      char* saveptr;
      token = strtok_r (config, " ", &saveptr);
      while (token) {
          if (strcmp (token, "verbose") == 0)
              fs->flags |= TFTPFS_VERBOSE;
          token = strtok_r (NULL, " ", &saveptr);
      }
  }
  
  return 0;

error:

  free (fs);
  free (root_path);

  rtems_set_errno_and_return_minus_one (ENOMEM);
}

/*
 * Release a stream and clear the pointer to it
 */
static void
releaseStream (tftpfs_info_t *fs, int s)
{
    if (fs->tftpStreams[s] && (fs->tftpStreams[s]->socket >= 0))
        close (fs->tftpStreams[s]->socket);
    rtems_semaphore_obtain (fs->tftp_mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    free (fs->tftpStreams[s]);
    fs->tftpStreams[s] = NULL;
    rtems_semaphore_release (fs->tftp_mutex);
}

static void
rtems_tftpfs_shutdown (rtems_filesystem_mount_table_entry_t* mt_entry)
{
  tftpfs_info_t *fs = tftpfs_info_mount_table (mt_entry);
  int            s;
  for (s = 0; s < fs->nStreams; s++)
      releaseStream (fs, s);
  rtems_semaphore_delete (fs->tftp_mutex);
  free (fs);
  free (mt_entry->mt_fs_root->location.node_access);
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
        uint16_t      opcode;
        uint16_t      errorCode;
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
        tv.tv_sec = PACKET_FIRST_TIMEOUT_MILLISECONDS / 1000L;
        tv.tv_usec = (PACKET_FIRST_TIMEOUT_MILLISECONDS % 1000L) * 1000L;
    }
    else {
        tv.tv_sec = PACKET_TIMEOUT_MILLISECONDS / 1000L;
        tv.tv_usec = (PACKET_TIMEOUT_MILLISECONDS % 1000L) * 1000L;
    }
    setsockopt (tp->socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);
    for (;;) {
        union {
            struct sockaddr s;
            struct sockaddr_in i;
        } from;
        socklen_t fromlen = sizeof from;
        len = recvfrom (tp->socket, &tp->pkbuf,
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

static void rtems_tftp_eval_path(rtems_filesystem_eval_path_context_t *self)
{
    int eval_flags = rtems_filesystem_eval_path_get_flags (self);

    if ((eval_flags & RTEMS_FS_MAKE) == 0) {
        int rw = RTEMS_FS_PERMS_READ | RTEMS_FS_PERMS_WRITE;

        if ((eval_flags & rw) != rw) {
            rtems_filesystem_location_info_t *currentloc =
                rtems_filesystem_eval_path_get_currentloc (self);
            char *current = currentloc->node_access;
            size_t currentlen = strlen (current);
            const char *path = rtems_filesystem_eval_path_get_path (self);
            size_t pathlen = rtems_filesystem_eval_path_get_pathlen (self);
            size_t len = currentlen + pathlen;

            rtems_filesystem_eval_path_clear_path (self);

            current = realloc (current, len + 1);
            if (current != NULL) {
                memcpy (current + currentlen, path, pathlen);
                current [len] = '\0';
                if (!rtems_tftp_is_directory (current, len)) {
                    fixPath (current);
                }
                currentloc->node_access = current;
            } else {
                rtems_filesystem_eval_path_error (self, ENOMEM);
            }
        } else {
            rtems_filesystem_eval_path_error (self, EINVAL);
        }
    } else {
        rtems_filesystem_eval_path_error (self, EIO);
    }
}

/*
 * The routine which does most of the work for the IMFS open handler
 */
static int rtems_tftp_open_worker(
    rtems_libio_t *iop,
    char          *full_path_name,
    int            oflag
)
{
    tftpfs_info_t        *fs;
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
     * Get the file system info.
     */
    fs = tftpfs_info_iop (iop);
    
    /*
     * Extract the host name component
     */
    hostname = full_path_name;
    cp1 = strchr (full_path_name, ':');
    if (!cp1)
        hostname = "BOOTP_HOST";
    else {
        *cp1 = '\0';
        ++cp1;
    }

    /*
     * Convert hostname to Internet address
     */
    if (strcmp (hostname, "BOOTP_HOST") == 0)
        farAddress = rtems_bsdnet_bootp_server_address;
    else if (inet_aton (hostname, &farAddress) == 0) {
        struct hostent *he = gethostbyname(hostname);
        if (he == NULL)
            return ENOENT;
        memcpy (&farAddress, he->h_addr, sizeof (farAddress));
    }
    
    /*
     * Extract file pathname component
     */
    if (strcmp (cp1, "BOOTP_FILE") == 0) {
        cp1 = rtems_bsdnet_bootp_boot_file_name;
    }
    if (*cp1 == '\0')
        return ENOENT;
    remoteFilename = cp1;
    if (strlen (remoteFilename) > (TFTP_BUFSIZE - 10))
        return ENOENT;

    /*
     * Find a free stream
     */
    sc = rtems_semaphore_obtain (fs->tftp_mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        return EBUSY;
    for (s = 0 ; s < fs->nStreams ; s++) {
        if (fs->tftpStreams[s] == NULL)
        break;
    }
    if (s == fs->nStreams) {
        /*
         * Reallocate stream pointers
         * Guard against the case where realloc() returns NULL.
         */
        struct tftpStream **np;

        np = realloc (fs->tftpStreams, ++fs->nStreams * sizeof *fs->tftpStreams);
        if (np == NULL) {
            rtems_semaphore_release (fs->tftp_mutex);
            return ENOMEM;
        }
        fs->tftpStreams = np;
    }
    tp = fs->tftpStreams[s] = malloc (sizeof (struct tftpStream));
    rtems_semaphore_release (fs->tftp_mutex);
    if (tp == NULL)
        return ENOMEM;
    iop->data0 = s;
    iop->data1 = tp;

    /*
     * Create the socket
     */
    if ((tp->socket = socket (AF_INET, SOCK_DGRAM, 0)) < 0) {
        releaseStream (fs, s);
        return ENOMEM;
    }

    /*
     * Bind the socket to a local address
     */
    retryCount = 0;
    now = rtems_clock_get_ticks_since_boot();
    for (;;) {
        int try = (now + retryCount) % 10;

        tp->myAddress.sin_family = AF_INET;
        tp->myAddress.sin_port = htons (UDP_PORT_BASE + fs->nStreams * try + s);
        tp->myAddress.sin_addr.s_addr = htonl (INADDR_ANY);
        if (bind (tp->socket, (struct sockaddr *)&tp->myAddress, sizeof tp->myAddress) >= 0)
            break;
        if (++retryCount == 10) {
            releaseStream (fs, s);
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
        if ((oflag & O_ACCMODE) == O_RDONLY) {
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
            releaseStream (fs, s);
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
                tp->nleft = len - 2 * sizeof (uint16_t  );
                tp->eof = (tp->nleft < TFTP_BUFSIZE);
                if (sendAck (tp) != 0) {
                    releaseStream (fs, s);
                    return EIO;
                }
                break;
            }
            if (tp->writing
             && (opcode == TFTP_OPCODE_ACK)
             && (ntohs (tp->pkbuf.tftpACK.blocknum) == 0)) {
                tp->nused = 0;
                tp->blocknum = 1;
                break;
            }
            if (opcode == TFTP_OPCODE_ERROR) {
                int e = tftpErrno (tp);
                releaseStream (fs, s);
                return e;
            }
        }

        /*
         * Keep trying
         */
        if (++retryCount >= OPEN_RETRY_LIMIT) {
            releaseStream (fs, s);
            return EIO;
        }
    }
    return 0;
}

static int rtems_tftp_open(
    rtems_libio_t *iop,
    const char    *new_name,
    int            oflag,
    mode_t         mode
)
{
    tftpfs_info_t *fs;
    char          *full_path_name;
    int           err;

    full_path_name = iop->pathinfo.node_access;

    if (rtems_tftp_is_directory (full_path_name, strlen (full_path_name))) {
        rtems_set_errno_and_return_minus_one (ENOTSUP);
    }

    /*
     * Get the file system info.
     */
    fs = tftpfs_info_iop (iop);

    if (fs->flags & TFTPFS_VERBOSE)
      printf ("TFTPFS: %s\n", full_path_name);

    err = rtems_tftp_open_worker (iop, full_path_name, oflag);
    if (err != 0) {
       rtems_set_errno_and_return_minus_one (err);
    }

    return 0;
}

/*
 * Read from a TFTP stream
 */
static ssize_t rtems_tftp_read(
    rtems_libio_t *iop,
    void          *buffer,
    size_t         count
)
{
    char              *bp;
    struct tftpStream *tp = iop->data1;
    int               retryCount;
    int               nwant;

    if (!tp)
        rtems_set_errno_and_return_minus_one( EIO );
    
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
                uint16_t   nextBlock = tp->blocknum + 1;
                if ((opcode == TFTP_OPCODE_DATA)
                 && (ntohs (tp->pkbuf.tftpDATA.blocknum) == nextBlock)) {
                    tp->nused = 0;
                    tp->nleft = len - 2 * sizeof (uint16_t);
                    tp->eof = (tp->nleft < TFTP_BUFSIZE);
                    tp->blocknum++;
                    if (sendAck (tp) != 0)
                        rtems_set_errno_and_return_minus_one (EIO);
                    break;
                }
                if (opcode == TFTP_OPCODE_ERROR)
                    rtems_set_errno_and_return_minus_one (tftpErrno (tp));
            }

            /*
             * Keep trying?
             */
            if (++retryCount == IO_RETRY_LIMIT)
                rtems_set_errno_and_return_minus_one (EIO);
            if (sendAck (tp) != 0)
                rtems_set_errno_and_return_minus_one (EIO);
        }
    }
    return count - nwant;
}

/*
 * Flush a write buffer and wait for acknowledgement
 */
static int rtems_tftp_flush (struct tftpStream *tp)
{
    int wlen, rlen;
    int retryCount = 0;

    wlen = tp->nused + 2 * sizeof (uint16_t  );
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
             && (ntohs (tp->pkbuf.tftpACK.blocknum) == tp->blocknum)) {
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
    tftpfs_info_t     *fs;
    struct tftpStream *tp = iop->data1;
    int                e = 0;
    
    /*
     * Get the file system info.
     */
    fs = tftpfs_info_iop (iop);
    
    if (!tp) 
        rtems_set_errno_and_return_minus_one (EIO);
    
    if (tp->writing)
        e = rtems_tftp_flush (tp);
    if (!tp->eof && !tp->firstReply) {
        /*
         * Tell the other end to stop
         */
        rtems_interval ticksPerSecond;
        sendStifle (tp, &tp->farAddress);
        ticksPerSecond = rtems_clock_get_ticks_per_second();
        rtems_task_wake_after (1 + ticksPerSecond / 10);
    }
    releaseStream (fs, iop->data0);
    rtems_set_errno_and_return_minus_one (e);
}

static ssize_t rtems_tftp_write(
    rtems_libio_t   *iop,
    const void      *buffer,
    size_t           count
)
{
    const char        *bp;
    struct tftpStream *tp = iop->data1;
    int               nleft, nfree, ncopy;

    /*
     * Bail out if an error has occurred
     */
    if (!tp->writing)
        rtems_set_errno_and_return_minus_one (EIO);

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
                rtems_set_errno_and_return_minus_one (e);
            }
        }
    }
    return count;
}

/*
 * Dummy version to let fopen(xxxx,"w") work properly.
 */
static int rtems_tftp_ftruncate(
    rtems_libio_t   *iop __attribute__((unused)),
    off_t            count __attribute__((unused))
)
{
    return 0;
}

static rtems_filesystem_node_types_t rtems_tftp_node_type(
    const rtems_filesystem_location_info_t *loc
)
{
    const char *path = loc->node_access;
    size_t pathlen = strlen (path);

    return rtems_tftp_is_directory (path, pathlen) ?
        RTEMS_FILESYSTEM_DIRECTORY
            : RTEMS_FILESYSTEM_MEMORY_FILE;
}

static int rtems_tftp_clone(
    rtems_filesystem_location_info_t *loc
)
{
    int rv = 0;

    loc->node_access = strdup (loc->node_access);

    if (loc->node_access == NULL) {
        errno = ENOMEM;
        rv = -1;
    }

    return rv;
}

static void rtems_tftp_free_node_info(
    const rtems_filesystem_location_info_t *loc
)
{
    free (loc->node_access);
}

static bool rtems_tftp_are_nodes_equal(
  const rtems_filesystem_location_info_t *a,
  const rtems_filesystem_location_info_t *b
)
{
  return strcmp (a->node_access, b->node_access) == 0;
}

static const rtems_filesystem_operations_table  rtems_tftp_ops = {
    .lock_h = rtems_filesystem_default_lock,
    .unlock_h = rtems_filesystem_default_unlock,
    .eval_path_h = rtems_tftp_eval_path,
    .link_h = rtems_filesystem_default_link,
    .are_nodes_equal_h = rtems_tftp_are_nodes_equal,
    .node_type_h = rtems_tftp_node_type,
    .mknod_h = rtems_filesystem_default_mknod,
    .rmnod_h = rtems_filesystem_default_rmnod,
    .fchmod_h = rtems_filesystem_default_fchmod,
    .chown_h = rtems_filesystem_default_chown,
    .clonenod_h = rtems_tftp_clone,
    .freenod_h = rtems_tftp_free_node_info,
    .mount_h = rtems_filesystem_default_mount,
    .fsmount_me_h = rtems_tftpfs_initialize,
    .unmount_h = rtems_filesystem_default_unmount,
    .fsunmount_me_h = rtems_tftpfs_shutdown,
    .utime_h = rtems_filesystem_default_utime,
    .symlink_h = rtems_filesystem_default_symlink,
    .readlink_h = rtems_filesystem_default_readlink,
    .rename_h = rtems_filesystem_default_rename,
    .statvfs_h = rtems_filesystem_default_statvfs
};

static const rtems_filesystem_file_handlers_r rtems_tftp_handlers = {
   .open_h = rtems_tftp_open,
   .close_h = rtems_tftp_close,
   .read_h = rtems_tftp_read,
   .write_h = rtems_tftp_write,
   .ioctl_h = rtems_filesystem_default_ioctl,
   .lseek_h = rtems_filesystem_default_lseek,
   .fstat_h = rtems_filesystem_default_fstat,
   .ftruncate_h = rtems_tftp_ftruncate,
   .fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
   .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
   .fcntl_h = rtems_filesystem_default_fcntl
};
