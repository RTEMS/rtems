/* FIXME: 1. Parse command is a hack.  We can do better.
 *        2. Some sort of access control?
 *        3. OSV: Timeouts on sockets.
 *        4. OSV: hooks support seems to be a joke, as it requires storing of
 *           entire input file in memory. Seem to be better to change it to
 *           something more reasonable, like having
 *           'hook_write(void const *buf, int count)' routine that will be
 *           called multiple times while file is being received.
 *        5. OSV: Remove hack with "/dev/null"?
 *        6. OSV: data_addr field of SessionInfo structure is not initialized.
 *           This will lead to undefined behavior if FTP client doesn't send
 *           PORT command. Seems we need to implement usage of default port
 *           (ftp port - 1). I didn't find any FTP client that doesn't send
 *           PORT command though.
 *        7. OSV: while server claims TYPE ASCII command succeeds (to make
 *           happy some clients), ASCII mode isn't actually implemented.
 *        8. Passive Mode?
 *
 *  FTP Server Daemon
 *
 *  Submitted by: Jake Janovetz <janovetz@tempest.ece.uiuc.edu>
 *
 *  Changed by:   Sergei Organov <osv@javad.ru> (OSV)
 *    Changes:
 *      - use pool of pre-created threads to handle sessions
 *      - LIST output now similar to what "/bin/ls -al" would output, thus
 *        FTP clients could parse it.
 *      - LIST NAME now works (both for files and directories)
 *      - keep track of CWD for every session separately
 *      - ability to specify root directory name in configuration table
 *      - options sent in commands are ignored, thus LIST -al FILE works
 *      - added support for NLST, CDUP and MDTM commands
 *      - buffers are allocated on stack instead of heap where possible
 *      - drop using of task notepad to pass parameters - use function
 *        arguments instead
 *      - various bug-fixes, e.g., use of PF_INET in socket() instead of
 *        AF_INET, use snprintf() instead of sprintf() everywhere for safety,
 *        etc.
 *
 *  $Id$
 */

/**************************************************************************
 *                                 ftpd.c                                 *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    This file contains the daemon which services requests that appear   *
 *    on the FTP port.  This server is compatible with FTP, but it        *
 *    also provides 'hooks' to make it usable in situations where files   *
 *    are not used/necessary.  Once the server is started, it runs        *
 *    forever.                                                            *
 *                                                                        *
 *                                                                        *
 *    Organization:                                                       *
 *                                                                        *
 *       The FTP daemon is started upon boot along with a (configurable)  *
 *       number of tasks to handle sessions. It runs all the time and     *
 *       waits for connections on the known FTP port (21).  When          *
 *       a connection is made, it wakeups a 'session' task.  That         *
 *       session then interacts with the remote host.  When the session   *
 *       is complete, the session task goes to sleep.  The daemon still   *
 *       runs, however.                                                   *
 *                                                                        *
 *                                                                        *
 * Supported commands are:                                                *
 *                                                                        *
 * RETR xxx     - Sends a file from the client.                           *
 * STOR xxx     - Receives a file from the client.  xxx = filename.       *
 * LIST xxx     - Sends a file list to the client.                        *
 * NLST xxx     - Sends a file list to the client.                        *
 * USER         - Does nothing.                                           *
 * PASS         - Does nothing.                                           *
 * SYST         - Replies with the system type (`RTEMS').                 *
 * DELE xxx     - Delete file xxx.                                        *
 * MKD xxx      - Create directory xxx.                                   *
 * RMD xxx      - Remove directory xxx.                                   *
 * PWD          - Print working directory.                                *
 * CWD xxx      - Change working directory.                               *
 * CDUP         - Change to upper directory.                              *
 * SITE CHMOD xxx yyy - Change permissions on file yyy to xxx.            *
 * PORT a,b,c,d,x,y   - Setup for a data port to IP address a.b.c.d       *
 *                      and port (x*256 + y).                             *
 * MDTM xxx     - Send date/time to the client. xxx = filename.           *
 *                                                                        *
 *                                                                        *
 * The public routines contained in this file are:                        *
 *                                                                        *
 *    rtems_initialize_ftpd - Initializes and starts the server daemon,   *
 *                            then returns to its caller.                 *
 *                                                                        *
 *------------------------------------------------------------------------*
 * Jake Janovetz                                                          *
 * University of Illinois                                                 *
 * 1406 West Green Street                                                 *
 * Urbana IL  61801                                                       *
 **************************************************************************
 * Change History:                                                        *
 *  12/01/97   - Creation (JWJ)                                           *
 *  2001-01-08 - Changes by OSV                                           *
 *************************************************************************/

/**************************************************************************
 * Meanings of first and second digits of reply codes:
 *
 * Reply:  Description:
 *-------- --------------
 *  1yz    Positive preliminary reply.  The action is being started but
 *         expect another reply before sending another command.
 *  2yz    Positive completion reply.  A new command can be sent.
 *  3yz    Positive intermediate reply.  The command has been accpeted
 *         but another command must be sent.
 *  4yz    Transient negative completion reply.  The requested action did
 *         not take place, but the error condition is temporary so the
 *         command can be reissued later.
 *  5yz    Permanent negative completion reply.  The command was not
 *         accepted and should not be retried.
 *-------------------------------------------------------------------------
 *  x0z    Syntax errors.
 *  x1z    Information.
 *  x2z    Connections.  Replies referring to the control or data
 *         connections.
 *  x3z    Authentication and accounting.  Replies for the login or
 *         accounting commands.
 *  x4z    Unspecified.
 *  x5z    Filesystem status.
 *************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/error.h>
#include <syslog.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/ftp.h>
#include <netinet/in.h>

#include "ftpd.h"


#ifdef __GNUC__
/* change to #if 1 to disable syslog entirely */
#if 0
#undef  syslog
#define syslog(a, b, ...) while(0){}
#endif
#endif

#define FTPD_SERVER_MESSAGE  "RTEMS FTP server (Version 1.1-JWJ) ready."

#define FTPD_SYSTYPE "RTEMS"

/* Seem to be unused */
#if 0
#define FTPD_WELCOME_MESSAGE \
   "Welcome to the RTEMS FTP server.\n" \
   "\n" \
   "Login accepted.\n"
#endif

/* Various buffer sizes */
enum
{
  FTPD_BUFSIZE  = 256,       /* Size for temporary buffers */
  FTPD_DATASIZE = 1024,      /* Size for file transfer buffers */
  FTPD_STACKSIZE = 8 * 1024, /* Tasks stack size */
};

/* Event to be used by session tasks for waiting */
enum
{
  FTPD_RTEMS_EVENT = RTEMS_EVENT_1
};

/* Configuration table */
extern struct rtems_ftpd_configuration rtems_ftpd_configuration;

/* this is not prototyped in strict ansi mode */
FILE *fdopen (int fildes, const char *mode);

/*
 * SessionInfo structure.
 *
 * The following structure is allocated for each session.
 */
typedef struct
{
  struct sockaddr_in  data_addr;   /* Data address for PORT commands */
  FILE                *ctrl_fp;    /* File pointer for control connection */
  int                 socket;      /* Socket descriptor for ctrl connection */
  char                cwd[FTPD_BUFSIZE];  /* Current working directory */
  /* Login -- future use -- */
  int                 xfer_mode;   /* Transfer mode (ASCII/binary) */
  rtems_id            tid;         /* Task id */
} FTPD_SessionInfo_t;


/*
 * TaskPool structure.
 */
typedef struct
{
  FTPD_SessionInfo_t    *info;
  FTPD_SessionInfo_t    **queue;
  int                   count;
  int                   head;
  int                   tail;
  rtems_id              mutex;
  rtems_id              sem;
} FTPD_TaskPool_t;

/*
 * Task pool instance.
 */
static FTPD_TaskPool_t task_pool;

/*
 * Root node for FTPD without trailing slash. Even '/' node is denoted as
 * empty string here.
 */
static char root[FTPD_BUFSIZE];


/*PAGE
 *
 * serr
 *
 * Return errror string corresponding to current 'errno'.
 *
 */
static char const*
serr(void)
{
  return strerror(errno);
}


/*
 *  Utility routines to manage root directory and session local
 *  current working directory.
 */


/*PAGE
 *
 * squeeze_path
 *
 * Squeezes path according to OS rules, i.e., eliminates /./, /../, and //
 * from the path. Does nothing if the path is relative, i.e. doesn't begin
 * with '/'. The trailing slash is always removed, even when alone, i.e. "/"
 * will be "" after squeeze.
 *
 * Input parameters:
 *   path - the path to be squeezed
 *
 * Output parameters:
 *   path - squeezed path
 *
 */
static void
squeeze_path(char* path)
{
  if(path[0] == '/')
  {
    char* e = path + 1;
    int rest = strlen(e);
    while(rest >= 0)
    {
      int len;
      char* s = e;
      e = strchr(s, '/');
      if(e)
        ++e;
      else
        e = s + rest + 1;
      len = e - s;
      rest -= len;
      if(len == 1 || (len == 2 && s[0] == '.'))
      {
        if(rest >= 0)
          memmove(s, e, rest + 1);
        else
          *s++ = '\0';
        e = s;
      }
      else if(len == 3 && s[0] == '.' && s[1] == '.')
      {
        char* ps = s;
        if(ps - 1 > path) {
          do
            --ps;
          while(ps[-1] != '/');
        }
        if(rest >= 0)
          memmove(ps, e, rest + 1);
        else
          *ps++ = '\0';
        e = ps;
      }
    }
    if(e[-2] == '/')
      e[-2] = '\0';
  }
}


/*PAGE
 *
 * make_path
 *
 * Makes full path given file name, current working directory and root
 * directory (file scope variable 'root').
 *
 * Input parameters:
 *   cwd  - current working directory
 *   name - file name
 *   root (file scope variable) - FTPD root directory
 *
 * Output parameters:
 *   buf - full path
 *   returns pointer to non-root part of the 'buf', i.e. to first character
 *           different from '/' after root part.
 *
 */
static char const*
make_path(char* buf, char const* cwd, char const* name)
{
  char* res = NULL;

  int rlen = strlen(root);
  int clen = strlen(cwd);
  int nlen = strlen(name);
  int len = rlen + nlen;

  if (name[0] != '/')
  {
    ++len;
    if (clen > 0)
      len += clen + 1;
  }

  if (FTPD_BUFSIZE > len)
  {
    char* b = buf;
    memcpy(b, root, rlen); b += rlen;
    if (name[0] != '/')
    {
      *b++ = '/';
      if (clen > 0)
      {
        memcpy(b, cwd, clen); b += clen;
        *b++ = '/';
      }
    }
    memcpy(b, name, nlen); b += nlen;
    *b = '\0';

    res = buf + rlen;
    while(rlen-- > 0 && res[-1] == '/')
      --res;
    squeeze_path(res);
  }

  return res;
}

/*
 * Task pool management routines
 */


/*PAGE
 *
 * task_pool_done
 *
 * Cleanup task pool.
 *
 * Input parameters:
 *   count - number of entries in task pool to cleanup
 *
 * Output parameters:
 *   NONE
 *
 */
static void
task_pool_done(int count)
{
  int i;
  for(i = 0; i < count; ++i)
    rtems_task_delete(task_pool.info[i].tid);
  if(task_pool.info)
    free(task_pool.info);
  if(task_pool.queue)
    free(task_pool.queue);
  if(task_pool.mutex != (rtems_id)-1)
    rtems_semaphore_delete(task_pool.mutex);
  if(task_pool.sem != (rtems_id)-1)
    rtems_semaphore_delete(task_pool.sem);
  task_pool.info = 0;
  task_pool.queue = 0;
  task_pool.count = 0;
  task_pool.sem = -1;
  task_pool.mutex = -1;
}

/* Forward declare */
static void session(rtems_task_argument arg);

/*PAGE
 *
 * task_pool_init
 *
 * Initialize task pool.
 *
 * Input parameters:
 *   count    - number of entries in task pool to create
 *   priority - priority tasks are started with
 *
 * Output parameters:
 *   returns 1 on success, 0 on failure.
 *
 */
static int
task_pool_init(int count, rtems_task_priority priority)
{
  int i;
  rtems_status_code sc;
  char id = 'a';

  task_pool.count = 0;
  task_pool.head = task_pool.tail = 0;
  task_pool.mutex = (rtems_id)-1;
  task_pool.sem   = (rtems_id)-1;

  sc = rtems_semaphore_create(
    rtems_build_name('F', 'T', 'P', 'M'),
    1,
    RTEMS_DEFAULT_ATTRIBUTES
    | RTEMS_BINARY_SEMAPHORE
    | RTEMS_INHERIT_PRIORITY
    | RTEMS_PRIORITY,
    RTEMS_NO_PRIORITY,
    &task_pool.mutex);

  if(sc == RTEMS_SUCCESSFUL)
    sc = rtems_semaphore_create(
      rtems_build_name('F', 'T', 'P', 'S'),
      count,
      RTEMS_DEFAULT_ATTRIBUTES,
      RTEMS_NO_PRIORITY,
      &task_pool.sem);

  if(sc != RTEMS_SUCCESSFUL) {
    task_pool_done(0);
    syslog(LOG_ERR, "ftpd: Can not create semaphores");
    return 0;
  }

  task_pool.info = (FTPD_SessionInfo_t*)
    malloc(sizeof(FTPD_SessionInfo_t) * count);
  task_pool.queue = (FTPD_SessionInfo_t**)
    malloc(sizeof(FTPD_SessionInfo_t*) * count);
  if (NULL == task_pool.info || NULL == task_pool.queue)
  {
    task_pool_done(0);
    syslog(LOG_ERR, "ftpd: Not enough memory");
    return 0;
  }

  for(i = 0; i < count; ++i)
  {
    FTPD_SessionInfo_t *info = &task_pool.info[i];
    sc = rtems_task_create(rtems_build_name('F', 'T', 'P', id),
      priority, FTPD_STACKSIZE,
      RTEMS_PREEMPT | RTEMS_NO_TIMESLICE |
      RTEMS_NO_ASR | RTEMS_INTERRUPT_LEVEL(0),
      RTEMS_NO_FLOATING_POINT | RTEMS_LOCAL,
      &info->tid);
    if (sc == RTEMS_SUCCESSFUL)
    {
      sc = rtems_task_start(
        info->tid, session, (rtems_task_argument)info);
      if (sc != RTEMS_SUCCESSFUL)
        task_pool_done(i);
    }
    else
      task_pool_done(i + 1);
    if (sc != RTEMS_SUCCESSFUL)
    {
      syslog(LOG_ERR, "ftpd: Could not create/start FTPD session: %s",
        rtems_status_text(sc));
      return 0;
    }
    task_pool.queue[i] = task_pool.info + i;
    info->ctrl_fp = NULL;
    info->socket  = -1;
    if (++id > 'z')
      id = 'a';
  }
  task_pool.count = count;
  return 1;
}

/*PAGE
 *
 * task_pool_obtain
 *
 * Obtain free task from task pool.
 *
 * Input parameters:
 *   NONE
 *
 * Output parameters:
 *   returns pointer to the corresponding SessionInfo structure on success,
 *           NULL if there are no free tasks in the pool.
 *
 */
static FTPD_SessionInfo_t*
task_pool_obtain()
{
  FTPD_SessionInfo_t* info = 0;
  rtems_status_code sc;
  sc = rtems_semaphore_obtain(task_pool.sem, RTEMS_NO_WAIT, RTEMS_NO_TIMEOUT);
  if (sc == RTEMS_SUCCESSFUL)
  {
    rtems_semaphore_obtain(task_pool.mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    info = task_pool.queue[task_pool.head];
    if(++task_pool.head >= task_pool.count)
      task_pool.head = 0;
    info->socket = -1;
    info->ctrl_fp = NULL;
    rtems_semaphore_release(task_pool.mutex);
  }
  return info;
}

/*PAGE
 *
 * task_pool_release
 *
 * Return task obtained by 'obtain()' back to the task pool.
 *
 * Input parameters:
 *   info  - pointer to corresponding SessionInfo structure.
 *
 * Output parameters:
 *   NONE
 *
 */
static void
task_pool_release(FTPD_SessionInfo_t* info)
{
  rtems_semaphore_obtain(task_pool.mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  task_pool.queue[task_pool.tail] = info;
  if(++task_pool.tail >= task_pool.count)
    task_pool.tail = 0;
  rtems_semaphore_release(task_pool.mutex);
  rtems_semaphore_release(task_pool.sem);
}

/*
 * End of task pool routines
 */

/*PAGE
 *
 * close_socket
 *
 * Close socket.
 *
 * Input parameters:
 *   s - socket descriptor to be closed.
 *
 * Output parameters:
 *   returns 1 on success, 0 on failure
 *
 */
static int
close_socket(int s)
{
  if (0 <= s)
  {
    if (0 != close(s))
    {
      shutdown(s, 2);
      if (0 != close(s))
        return 0;
    }
  }
  return 1;
}

/*PAGE
 *
 * close_stream
 *
 * Close data stream of session.
 *
 * Input parameters:
 *   info - corresponding SessionInfo structure
 *
 * Output parameters:
 *   NONE
 *
 */
static void
close_stream(FTPD_SessionInfo_t* info)
{
  if (NULL != info->ctrl_fp)
  {
    if (0 != fclose(info->ctrl_fp))
    {
      syslog(LOG_ERR, "ftpd: Could not close control stream: %s", serr());
    }
    else
      info->socket = -1;
  }

  if (!close_socket(info->socket))
    syslog(LOG_ERR, "ftpd: Could not close control socket: %s", serr());

  info->ctrl_fp = NULL;
  info->socket = -1;
}


/**************************************************************************
 * Function: send_reply                                                   *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    This procedure sends a reply to the client via the control          *
 *    connection.                                                         *
 *                                                                        *
 *                                                                        *
 * Inputs:                                                                *
 *                                                                        *
 *    int  code  - The 3-digit reply code.                                *
 *    char *text - Reply text.                                            *
 *                                                                        *
 * Output:                                                                *
 *                                                                        *
 *    none                                                                *
 *                                                                        *
 *************************************************************************/
static void
send_reply(FTPD_SessionInfo_t  *info, int code, char *text)
{
  char const* s;


  /***********************************************************************
   * code must be a 3-digit number.
   **********************************************************************/
  if ((code < 100) || (code > 999))
  {
    syslog(LOG_ERR, "ftpd: Code not 3-digits.");
    return;
  }

  /***********************************************************************
   * If a text reply exists, add it to the reply data.
   **********************************************************************/
  s  = (info->xfer_mode == TYPE_A) ? "\r" : "";
  if (text != NULL)
    fprintf(info->ctrl_fp, "%d %.70s%s\n", code, text, s);
  else
    fprintf(info->ctrl_fp, "%d%s\n", code, s);
  fflush(info->ctrl_fp);
}


/*PAGE
 *
 * send_mode_reply
 *
 * Sends BINARY/ASCII reply string depending on current transfer mode.
 *
 * Input parameters:
 *   info - corresponding SessionInfo structure
 *
 * Output parameters:
 *   NONE
 *
 */
static void
send_mode_reply(FTPD_SessionInfo_t *info)
{
  if(info->xfer_mode == TYPE_I)
    send_reply(info, 150, "Opening BINARY mode data connection.");
  else
    send_reply(info, 150, "Opening ASCII mode data connection.");
}

/*PAGE
 *
 * data_socket
 *
 * Create data socket for session.
 *
 * Input parameters:
 *   info - corresponding SessionInfo structure
 *
 * Output parameters:
 *   returns socket descriptor, or -1 if failure
 *
 */
static int
data_socket(FTPD_SessionInfo_t *info)
{
  int s = socket(PF_INET, SOCK_STREAM, 0);
  if(0 > s)
    send_reply(info, 420, "Server error - could not create socket.");
  else if(0 > connect(s, (struct sockaddr *)&info->data_addr,
    sizeof(struct sockaddr)))
  {
    send_reply(info, 420, "Server error - could not connect socket.");
    close_socket(s);
    s = -1;
  }
  return s;
}

/**************************************************************************
 * Function: command_retrieve                                             *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    This performs the "RETR" command.  A data connection must already   *
 *    be open (via the "PORT" command.)  Here, we send the data to the    *
 *    connection.                                                         *
 *                                                                        *
 *                                                                        *
 * Inputs:                                                                *
 *                                                                        *
 *    char *filename   - Source filename.                                 *
 *                                                                        *
 * Output:                                                                *
 *                                                                        *
 *    int  - 0 for reply sent.                                            *
 *           1 for no reply sent.                                         *
 *                                                                        *
 *************************************************************************/
static int
command_retrieve(FTPD_SessionInfo_t  *info, char *filename)
{
  int                 s = -1;
  int                 n;
  int                 fd = -1;
  char                buf[FTPD_DATASIZE];
  int                 res = 0;

  char const* r = make_path(buf, info->cwd, filename);

  if (NULL == r || 0 > (fd = open(buf, O_RDONLY)))
  {
    send_reply(info, 450, "Error opening file.");
    return(res);
  }

  send_mode_reply(info);

  /***********************************************************************
   * Connect to the data connection (PORT made in an earlier PORT call).
   **********************************************************************/
  s = data_socket(info);
  if (0 <= s)
  {
    /***********************************************************************
     * Send the data over the ether.
     **********************************************************************/
    while ((n = read(fd, buf, FTPD_DATASIZE)) > 0)
    {
      send(s, buf, n, 0);
    }

    if (0 == n)
    {
      if (0 == close(fd))
      {
        fd = -1;
        res = 1;
      }
    }
  }

  if (0 == res)
    send_reply(info, 450, "Retrieve failed.");
  else
    send_reply(info, 210, "File sent successfully.");

  if (-1 != fd)
    close(fd);

  if (!close_socket(s))
    syslog(LOG_ERR, "ftpd: Error closing data socket");

  return(res);
}


/**************************************************************************
 * Function: command_store                                                *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    This performs the "STOR" command.  A data connection must already   *
 *    be open (via the "PORT" command.)  Here, we get the data from the   *
 *    connection and figure out what to do with it.                       *
 *                                                                        *
 *                                                                        *
 * Inputs:                                                                *
 *                                                                        *
 *    char *filename   - Destination filename.                            *
 *                                                                        *
 * Output:                                                                *
 *                                                                        *
 *    int  - 0 for success.                                               *
 *           1 for failure.                                               *
 *                                                                        *
 *************************************************************************/
static int
command_store(FTPD_SessionInfo_t *info, char *filename)
{
  int                    s;
  int                    n;
  unsigned long          size = 0;
  struct rtems_ftpd_hook *usehook = NULL;
  char                   buf[FTPD_DATASIZE];

  int null = !strcmp("/dev/null", filename);

  if(!null)
  {
    if (NULL == make_path(buf, info->cwd, filename))
    {
      send_reply(info, 450, "Error creating file.");
      return(0);
    }
  }

  send_mode_reply(info);

  s = data_socket(info);
  if(0 > s)
  {
    return(1);
  }


  /***********************************************************************
   * File: "/dev/null" just throws the data away.
   * Otherwise, search our list of hooks to see if we need to do something
   *   special.
   * OSV: FIXME: this is hack. Using /dev/null filesystem entry would be
   *             better. However, it's not clear how to handle root directory
   *             other than '/' then.
   **********************************************************************/
  if (null)
  {
    while ((n = read(s, buf, FTPD_DATASIZE)) > 0);
  }
  else if (rtems_ftpd_configuration.hooks != NULL)
  {
    struct rtems_ftpd_hook *hook;
    int i;

    i = 0;
    hook = &rtems_ftpd_configuration.hooks[i++];
    while (hook->filename != NULL)
    {
      if (!strcmp(hook->filename, buf))
      {
        usehook = hook;
        break;
      }
      hook = &rtems_ftpd_configuration.hooks[i++];
    }
  }

  if (usehook != NULL)
  {
    /*
     * OSV: FIXME: Small buffer could be used and hook routine
     * called multiple times instead. Alternatively, the support could be
     * removed entirely in favor of configuring RTEMS pseudo-device with
     * given name.
     */

    char                *bigBufr;
    size_t filesize = rtems_ftpd_configuration.max_hook_filesize + 1;

    /***********************************************************************
     * Allocate space for our "file".
     **********************************************************************/
    bigBufr = (char *)malloc(filesize);
    if (bigBufr == NULL)
    {
      send_reply(info, 440, "Server error - malloc fail.");
      return(1);
    }

    /***********************************************************************
     * Retrieve the file into our buffer space.
     **********************************************************************/
    size = 0;
    while ((n = read(s, bigBufr + size, filesize - size)) > 0)
    {
      size += n;
    }
    if (size >= filesize)
    {
      send_reply(info, 440, "Server error - Buffer size exceeded.");
      free(bigBufr);
      close_socket(s);
      return(1);
    }
    close_socket(s);

    /***********************************************************************
     * Call our hook.
     **********************************************************************/
    if ((usehook->hook_function)(bigBufr, size) == 0)
    {
      send_reply(info, 210, "File transferred successfully.");
    }
    else
    {
      send_reply(info, 440, "File transfer failed.");
    }
    free(bigBufr);
  }
  else
  {
    int fd =
      creat(buf, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

    if (0 > fd)
    {
      send_reply(info, 450, "Error creating file.");
      close_socket(s);
      return(0);
    }

    while ((n = read(s, buf, FTPD_DATASIZE)) > 0)
    {
      if (0 > write(fd, buf, n))
      {
        send_reply(info, 450, "Error during write.");
        close(fd);
        close_socket(s);
        return(1);
      }
    }
    if (0 > close(fd))
    {
      send_reply(info, 450, "Error during write.");
      close_socket(s);
      return(1);
    }
    close_socket(s);
    send_reply(info, 226, "Transfer complete.");
  }

  return(0);
}


/*PAGE
 *
 * send_dirline
 *
 * Sends one line of LIST command reply corresponding to single file.
 *
 * Input parameters:
 *   s - socket descriptor to send data to
 *   wide - if 0, send only file name. If not 0, send 'stat' info as well in
 *          "ls -l" format.
 *   curTime - current time
 *   path - path to be prepended to what is given by 'add'
 *   add  - path to be appended to what is given by 'path', the resulting path
 *          is then passed to 'stat()' routine
 *   name - file name to be reported in output
 *   buf  - buffer for temporary data
 *
 * Output parameters:
 *   NONE
 *
 */
static void
send_dirline(int s, int wide, time_t curTime, char const* path,
  char const* add, char const* fname, char* buf)
{
  if(wide)
  {
    struct stat stat_buf;

    int plen = strlen(path);
    int alen = strlen(add);
    if(plen == 0)
    {
      buf[plen++] = '/';
      buf[plen] = '\0';
    }
    else
    {
      strcpy(buf, path);
      if(alen > 0 && buf[plen - 1] != '/')
      {
        buf[plen++] = '/';
        if(plen >= FTPD_BUFSIZE)
          return;
        buf[plen] = '\0';
      }
    }
    if(plen + alen >= FTPD_BUFSIZE)
      return;
    strcpy(buf + plen, add);

    if (stat(buf, &stat_buf) == 0)
    {
      int len;
      struct tm bt;
      time_t tf = stat_buf.st_mtime;
      enum { SIZE = 80 };
      enum { SIX_MONTHS = 365*24*60*60/2 };
      char timeBuf[SIZE];
      gmtime_r(&tf, &bt);
      if(curTime > tf + SIX_MONTHS || tf > curTime + SIX_MONTHS)
        strftime (timeBuf, SIZE, "%b %d  %Y", &bt);
      else
        strftime (timeBuf, SIZE, "%b %d %H:%M", &bt);

      len = snprintf(buf, FTPD_BUFSIZE,
        "%c%c%c%c%c%c%c%c%c%c  1 %5d %5d %11u %s %s\r\n",
        (S_ISLNK(stat_buf.st_mode)?('l'):
          (S_ISDIR(stat_buf.st_mode)?('d'):('-'))),
        (stat_buf.st_mode & S_IRUSR)?('r'):('-'),
        (stat_buf.st_mode & S_IWUSR)?('w'):('-'),
        (stat_buf.st_mode & S_IXUSR)?('x'):('-'),
        (stat_buf.st_mode & S_IRGRP)?('r'):('-'),
        (stat_buf.st_mode & S_IWGRP)?('w'):('-'),
        (stat_buf.st_mode & S_IXGRP)?('x'):('-'),
        (stat_buf.st_mode & S_IROTH)?('r'):('-'),
        (stat_buf.st_mode & S_IWOTH)?('w'):('-'),
        (stat_buf.st_mode & S_IXOTH)?('x'):('-'),
        (int)stat_buf.st_uid,
        (int)stat_buf.st_gid,
        (int)stat_buf.st_size,
        timeBuf,
        fname
      );

      send(s, buf, len, 0);
    }
  }
  else
  {
    int len = snprintf(buf, FTPD_BUFSIZE, "%s\r\n", fname);
    send(s, buf, len, 0);
  }
}

/**************************************************************************
 * Function: command_list                                      *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    Sends a file list through a data connection.  The data              *
 *    connection must have already been opened with the "PORT" command.   *
 *                                                                        *
 *                                                                        *
 * Inputs:                                                                *
 *                                                                        *
 *    char *fname  - File (or directory) to list.                         *
 *                                                                        *
 * Output:                                                                *
 *                                                                        *
 *    none                                                                *
 *                                                                        *
 *************************************************************************/
static void
command_list(FTPD_SessionInfo_t *info, char const *fname, int wide)
{
  int                 s;
  DIR                 *dirp = 0;
  struct dirent       *dp = 0;
  struct stat         stat_buf;
  char                buf[FTPD_BUFSIZE];
  char                path[FTPD_BUFSIZE];
  time_t curTime;
  char const* res;
  char const* cwd = info->cwd;

  send_reply(info, 150, "Opening ASCII mode data connection for LIST.");

  s = data_socket(info);
  if(0 > s)
  {
    syslog(LOG_ERR, "ftpd: Error connecting to data socket.");
    return;
  }

  if(fname[0] == '\0')
    fname = ".";

  res = make_path(path, cwd, fname);

  if (NULL == res || 0 > stat(path, &stat_buf))
  {
    snprintf(buf, FTPD_BUFSIZE,
      "%s: No such file or directory.\r\n", fname);
    send(s, buf, strlen(buf), 0);
  }
  else if (S_ISDIR(stat_buf.st_mode) && (NULL == (dirp = opendir(path))))
  {
    snprintf(buf, FTPD_BUFSIZE,
      "%s: Can not open directory.\r\n", fname);
    send(s, buf, strlen(buf), 0);
  }
  else
  {
    time(&curTime);
    if(!dirp)
      send_dirline(s, wide, curTime, path, "", fname, buf);
    else {
      /* FIXME: need "." and ".." only when '-a' option is given */
      send_dirline(s, wide, curTime, path, "", ".", buf);
      if(!strcmp(path, root))
        send_dirline(s, wide, curTime, path, "", "..", buf);
      else
        send_dirline(s, wide, curTime, path, "..", "..", buf);
      while ((dp = readdir(dirp)) != NULL)
        send_dirline(s, wide, curTime, path, dp->d_name, dp->d_name, buf);
    }
  }

  if(dirp)
    closedir(dirp);
  close_socket(s);
  send_reply(info, 226, "Transfer complete.");
}


/*PAGE
 *
 * rtems_ftpd_cwd
 *
 * Change current working directory. We use 'chdir' here only to validate the
 * new directory. We keep track of current working directory ourselves because
 * current working directory in RTEMS isn't thread local, but we need it to be
 * session local.
 *
 * Input parameters:
 *   info - corresponding SessionInfo structure
 *   dir  - directory name passed in CWD command
 *
 * Output parameters:
 *   info->cwd is set to new CWD value.
 *
 */
static void
rtems_ftpd_cwd(FTPD_SessionInfo_t  *info, char *dir)
{
  char buf[FTPD_BUFSIZE];
  char const* cwd = make_path(buf, info->cwd, dir);
  if(cwd && chdir(buf) == 0)
  {
    send_reply(info, 250, "CWD command successful.");
    strcpy(info->cwd, cwd);
  }
  else
  {
    send_reply(info, 550, "CWD command failed.");
  }
}



/*PAGE
 *
 * command_mdtm
 *
 * Handle FTP MDTM command
 *
 * Input parameters:
 *   info - corresponding SessionInfo structure
 *   fname - file name passed in MDTM command
 *
 * Output parameters:
 *   info->cwd is set to new CWD value.
 *
 */
static void
command_mdtm(FTPD_SessionInfo_t  *info, char const* fname)
{
  struct stat stbuf;
  char buf[FTPD_BUFSIZE];

  if(*fname == '\0')
    fname = ".";

  if (stat(fname, &stbuf) < 0)
  {
    snprintf(buf, FTPD_BUFSIZE, "%s: %s.", fname, serr());
    send_reply(info, 550, buf);
  }
  else
  {
    struct tm *t = gmtime(&stbuf.st_mtime);
    snprintf(buf, FTPD_BUFSIZE, "%04d%02d%02d%02d%02d%02d",
      1900 + t->tm_year,
      t->tm_mon+1, t->tm_mday,
      t->tm_hour, t->tm_min, t->tm_sec);
    send_reply(info, 213, buf);
  }
}

/**************************************************************************
 * Function: command_port                                                 *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    This procedure opens up a data port given the IP address of the     *
 *    remote machine and the port on the remote machine.  This connection *
 *    will then be used to transfer data between the hosts.               *
 *                                                                        *
 *                                                                        *
 * Inputs:                                                                *
 *                                                                        *
 *    char *bufr - Arguments to the "PORT" command.                       *
 *                                                                        *
 *                                                                        *
 * Output:                                                                *
 *                                                                        *
 *    none                                                                *
 *                                                                        *
 *************************************************************************/
static void
command_port(FTPD_SessionInfo_t *info, char *bufr)
{
  char                *ip;
  char                *port;
  int                 ip0, ip1, ip2, ip3, port0, port1;

  sscanf(bufr, "%d,%d,%d,%d,%d,%d", &ip0, &ip1, &ip2, &ip3, &port0, &port1);
  ip = (char *)&(info->data_addr.sin_addr);
  ip[0] = ip0 & 0xff;
  ip[1] = ip1 & 0xff;
  ip[2] = ip2 & 0xff;
  ip[3] = ip3 & 0xff;
  port = (char *)&(info->data_addr.sin_port);
  port[0] = port0 & 0xff;
  port[1] = port1 & 0xff;
  info->data_addr.sin_family = AF_INET;
}


/*PAGE
 *
 * skip_options
 *
 * Utility routine to skip options (if any) from input command.
 *
 * Input parameters:
 *   p  - pointer to pointer to command
 *
 * Output parameters:
 *   p  - is changed to point to first non-option argument
 *
 */
static void
skip_options(char **p)
{
  char* buf = *p;
  while(1) {
    while(*buf == ' ')
      ++buf;
    if(*buf == '-') {
      if(*++buf == '-') { /* `--' should terminate options */
        ++buf;
        while(*buf == ' ')
          ++buf;
        break;
      }
      while(*buf != ' ' && *buf != '\0')
        ++buf;
    }
    else
      break;
  }
  *p = buf;
}

/**************************************************************************
 * Function: parse_command                                     *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    Here, we parse the commands that have come through the control      *
 *    connection.                                                         *
 *                                                                        *
 * FIXME: This section is somewhat of a hack.  We should have a better    *
 *        way to parse commands.                                          *
 *                                                                        *
 * Inputs:                                                                *
 *                                                                        *
 *    char *bufr     - Pointer to the buffer which contains the command   *
 *                     text.                                              *
 *                                                                        *
 * Output:                                                                *
 *                                                                        *
 *    none                                                                *
 *                                                                        *
 *************************************************************************/
static void
parse_command(FTPD_SessionInfo_t *info, char *bufr)
{
  char fname[FTPD_BUFSIZE];

  if (!strncmp("PORT", bufr, 4))
  {
    send_reply(info, 200, "PORT command successful.");
    command_port(info, &bufr[5]);
  }
  else if (!strncmp("RETR", bufr, 4))
  {
    sscanf(&bufr[5], "%254s", fname);
    command_retrieve(info, fname);
  }
  else if (!strncmp("STOR", bufr, 4))
  {
    sscanf(&bufr[5], "%254s", fname);
    command_store(info, fname);
  }
  else if (!strncmp("LIST", bufr, 4))
  {
    bufr += 4;
    skip_options(&bufr);
    sscanf(bufr, "%254s", fname);
    command_list(info, fname, 1);
  }
  else if (!strncmp("NLST", bufr, 4))
  {
    bufr += 4;
    skip_options(&bufr);
    sscanf(bufr, "%254s", fname);
    command_list(info, fname, 0);
  }
  else if (!strncmp("MDTM", bufr, 4))
  {
    bufr += 4;
    skip_options(&bufr);
    sscanf(bufr, "%254s", fname);
    command_mdtm(info, fname);
  }
  else if (!strncmp("USER", bufr, 4))
  {
    send_reply(info, 230, "User logged in.");
  }
  else if (!strncmp("SYST", bufr, 4))
  {
    send_reply(info, 240, FTPD_SYSTYPE);
  }
  else if (!strncmp("TYPE", bufr, 4))
  {
    if (bufr[5] == 'I')
    {
      info->xfer_mode = TYPE_I;
      send_reply(info, 200, "Type set to I.");
    }
    else if (bufr[5] == 'A')
    {
      /* FIXME: ASCII mode isn't actually supported yet. */
      info->xfer_mode = TYPE_A;
      send_reply(info, 200, "Type set to A.");
    }
    else
    {
      info->xfer_mode = TYPE_I;
      send_reply(info, 504, "Type not implemented.  Set to I.");
    }
  }
  else if (!strncmp("PASS", bufr, 4))
  {
    send_reply(info, 230, "User logged in.");
  }
  else if (!strncmp("DELE", bufr, 4))
  {
    sscanf(&bufr[4], "%254s", fname);
    if (unlink(fname) == 0)
    {
      send_reply(info, 257, "DELE successful.");
    }
    else
    {
      send_reply(info, 550, "DELE failed.");
    }
  }
  else if (!strncmp("SITE CHMOD", bufr, 10))
  {
    int mask;

    sscanf(&bufr[11], "%o %254s", &mask, fname);
    if (chmod(fname, (mode_t)mask) == 0)
    {
      send_reply(info, 257, "CHMOD successful.");
    }
    else
    {
      send_reply(info, 550, "CHMOD failed.");
    }
  }
  else if (!strncmp("RMD", bufr, 3))
  {
    sscanf(&bufr[4], "%254s", fname);
    if (rmdir(fname) == 0)
    {
      send_reply(info, 257, "RMD successful.");
    }
    else
    {
      send_reply(info, 550, "RMD failed.");
    }
  }
  else if (!strncmp("MKD", bufr, 3))
  {
    sscanf(&bufr[4], "%254s", fname);
    if (mkdir(fname, S_IRWXU | S_IRWXG | S_IRWXO) == 0)
    {
      send_reply(info, 257, "MKD successful.");
    }
    else
    {
      send_reply(info, 550, "MKD failed.");
    }
  }
  else if (!strncmp("CWD", bufr, 3))
  {
    sscanf(&bufr[4], "%254s", fname);
    rtems_ftpd_cwd(info, fname);
  }
  else if (!strncmp("CDUP", bufr, 4))
  {
    rtems_ftpd_cwd(info, "..");
  }
  else if (!strncmp("PWD", bufr, 3))
  {
    char const* cwd = "/";
    if(info->cwd[0])
      cwd = info->cwd;
    snprintf(bufr, FTPD_BUFSIZE,
      "\"%s\" is the current directory.", cwd);
    send_reply(info, 250, bufr);
  }
  else
  {
    send_reply(info, 500, "Unrecognized/unsupported command.");
  }
}


/**************************************************************************
 * Function: session                                           *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    This task is started when the FTP daemon gets a service request     *
 *    from a remote machine.  Here, we watch for commands that will       *
 *    come through the "control" connection.  These commands are then     *
 *    parsed and executed until the connection is closed, either          *
 *    unintentionally or intentionally with the "QUIT" command.           *
 *                                                                        *
 *                                                                        *
 * Inputs:                                                                *
 *                                                                        *
 *    rtems_task_argument arg - The daemon task passes the socket         *
 *                              which serves as the control connection.   *
 *                                                                        *
 * Output:                                                                *
 *                                                                        *
 *    none                                                                *
 *                                                                        *
 *************************************************************************/
static void
session(rtems_task_argument arg)
{
  char cmd[FTPD_BUFSIZE];
  FTPD_SessionInfo_t  *info = (FTPD_SessionInfo_t  *)arg;
  rtems_event_set set;

  while(1) {
    rtems_event_receive(FTPD_RTEMS_EVENT, RTEMS_EVENT_ANY, RTEMS_NO_TIMEOUT,
      &set);

    send_reply(info, 220, FTPD_SERVER_MESSAGE);

    info->cwd[0] = 0;
    info->xfer_mode = TYPE_I;

    while (1)
    {
      if (fgets(cmd, FTPD_BUFSIZE, info->ctrl_fp) == NULL)
      {
        syslog(LOG_INFO, "ftpd: Connection aborted.");
        break;
      }

      if (!strncmp("QUIT", cmd, 4))
      {
        send_reply(info, 221, "Goodbye.");
        break;
      }
      else
      {
        parse_command(info, cmd);
      }
    }

    /* Close connection and put ourselves back into the task pool. */
    close_stream(info);
    task_pool_release(info);
  }
}


/**************************************************************************
 * Function: daemon                                            *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    This task runs in the background forever.  It waits for service     *
 *    requests on the FTP port (port 21).  When a request is received,    *
 *    it opens a new session to handle those requests until the           *
 *    connection is closed.                                               *
 *                                                                        *
 *                                                                        *
 * Inputs:                                                                *
 *                                                                        *
 *    none                                                                *
 *                                                                        *
 * Output:                                                                *
 *                                                                        *
 *    none                                                                *
 *                                                                        *
 *************************************************************************/
static void
daemon()
{
  int                 s;
  int                 addrLen;
  struct sockaddr_in  remoteAddr;
  struct sockaddr_in  localAddr;
  char                sessionID;
  FTPD_SessionInfo_t  *info = NULL;


  sessionID = 'a';

  s = socket(PF_INET, SOCK_STREAM, 0);
  if (s < 0)
    syslog(LOG_ERR, "ftpd: Error creating socket: %s", serr());

  localAddr.sin_family      = AF_INET;
  localAddr.sin_port        = htons(rtems_ftpd_configuration.port);
  localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  memset(localAddr.sin_zero, '\0', sizeof(localAddr.sin_zero));

  if (0 > bind(s, (struct sockaddr *)&localAddr, sizeof(localAddr)))
    syslog(LOG_ERR, "ftpd: Error binding control socket: %s", serr());

  if (0 > listen(s, 1))
    syslog(LOG_ERR, "ftpd: Error listening on control socket: %s", serr());

  while (1)
  {
    int ss;
    addrLen = sizeof(remoteAddr);
    ss = accept(s, (struct sockaddr *)&remoteAddr, &addrLen);
    if (0 > ss)
    {
      syslog(LOG_ERR, "ftpd: Error accepting control connection: %s", serr());
    }
    else
    {
      info = task_pool_obtain();
      if (NULL == info)
      {
        close_socket(ss);
      }
      else
      {
        info->socket = ss;
        if ((info->ctrl_fp = fdopen(info->socket, "r+")) == NULL)
        {
          syslog(LOG_ERR, "ftpd: fdopen() on socket failed: %s", serr());
          close_stream(info);
          task_pool_release(info);
        }
        else
        {
          /* Wakeup the session task. The task will call task_pool_release
             after it closes connection. */
          rtems_event_send(info->tid, FTPD_RTEMS_EVENT);
        }
      }
    }
  }
}


/**************************************************************************
 * Function: rtems_ftpd_start                                             *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    Here, we start the FTPD task which waits for FTP requests and       *
 *    services them.  This procedure returns to its caller once the       *
 *    task is started.                                                    *
 *                                                                        *
 *                                                                        *
 * Inputs:                                                                *
 *                                                                        *
 * Output:                                                                *
 *                                                                        *
 *    int - RTEMS_SUCCESSFUL on successful start of the daemon.           *
 *                                                                        *
 *************************************************************************/
int
rtems_initialize_ftpd()
{
  rtems_status_code   sc;
  rtems_id            tid;
  rtems_task_priority priority;
  int count;

  if (rtems_ftpd_configuration.port == 0)
  {
    rtems_ftpd_configuration.port = FTPD_CONTROL_PORT;
  }

  if (rtems_ftpd_configuration.priority == 0)
  {
    rtems_ftpd_configuration.priority = 40;
  }
  priority = rtems_ftpd_configuration.priority;

  if (rtems_ftpd_configuration.tasks_count <= 0)
    rtems_ftpd_configuration.tasks_count = 1;
  count = rtems_ftpd_configuration.tasks_count;

  if (!task_pool_init(count, priority))
  {
    syslog(LOG_ERR, "ftpd: Could not initialize task pool.");
    return RTEMS_UNSATISFIED;
  }

  sc = rtems_task_create(rtems_build_name('F', 'T', 'P', 'D'),
    priority, FTPD_STACKSIZE,
    RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_NO_ASR |
    RTEMS_INTERRUPT_LEVEL(0),
    RTEMS_NO_FLOATING_POINT | RTEMS_LOCAL,
    &tid);

  if (sc == RTEMS_SUCCESSFUL)
  {
    sc = rtems_task_start(tid, daemon, 0);
    if (sc != RTEMS_SUCCESSFUL)
      rtems_task_delete(tid);
  }

  if (sc != RTEMS_SUCCESSFUL)
  {
    task_pool_done(count);
    syslog(LOG_ERR, "ftpd: Could not create/start FTP daemon: %s",
      rtems_status_text(sc));
    return RTEMS_UNSATISFIED;
  }

  root[0] = '\0';
  if (
    rtems_ftpd_configuration.root &&
    strlen(rtems_ftpd_configuration.root) < FTPD_BUFSIZE &&
    rtems_ftpd_configuration.root[0] == '/')
  {
    strcpy(root, rtems_ftpd_configuration.root);
    squeeze_path(root);
    rtems_ftpd_configuration.root = root;
  }

  syslog(LOG_INFO, "ftpd: FTP daemon started (%d session%s max)",
    count, ((count > 1) ? "s" : ""));
  return RTEMS_SUCCESSFUL;
}
