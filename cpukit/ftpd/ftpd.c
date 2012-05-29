/* FIXME: 1. Parse command is a hack.  We can do better.
 *        2. OSV: hooks support seems to be bad, as it requires storing of
 *           entire input file in memory.  Seem to be better to change it to
 *           something more reasonable, like having
 *           'hook_write(void const *buf, int count)' routine that will be
 *           called multiple times while file is being received.
 *        3. OSV: Remove hack with "/dev/null"?
 *
 *  FTP Server Daemon
 *
 *  Submitted by: Jake Janovetz <janovetz@tempest.ece.uiuc.edu>
 *
 *  Changed by:   Sergei Organov <osv@javad.ru> (OSV)
 *                Arnout Vandecappelle <arnout@mind.be> (AV)
 *                Sebastien Bourdeauducq <sebastien@milkymist.org> (MM)
 *                
 *
 *  Changes:
 *
 *    2010-12-02        Sebastien Bourdeauducq <sebastien@milkymist.org>
 *
 *      * Support spaces in filenames
 * 
 *    2010-04-29        Arnout Vandecappelle (Essensium/Mind) <arnout@mind.be>
 * 
 *      * Added USER/PASS authentication.
 * 
 *    2001-01-31        Sergei Organov <osv@javad.ru>
 *
 *      * Hacks with current dir and root dir removed in favor of new libio
 *        support for task-local current and root directories.
 *
 *    2001-01-30        Sergei Organov <osv@javad.ru>
 *
 *      * Bug in `close_data_socket()' introduced by previous change fixed.
 *      * `command_pasv()' changed to set timeout on socket we are listening on
 *        and code fixed to don't close socket twice on error.
 *      * `serr()' changed to clear `errno'.
 *      * `data_socket()' changed to clear `errno' before `bind()'.
 *      * `session()' changed to clear `errno' before processing session.
 *
 *    2001-01-29        Sergei Organov <osv@javad.ru>
 *
 *      * `close_data_socket()' fixed to close both active and passive sockets
 *      * Initialize info->data_socket to -1 in `daemon()'
 *      * Initialize `fname' to empty string  in `exec_command()'
 *
 *    2001-01-22        Sergei Organov <osv@javad.ru>
 *
 *      * Timeouts on sockets implemented. 'idle' field added to
 *        configuration. No timeout by default to keep backward compatibility.
 *        Note: SITE IDLE command not implemented yet.
 *      * Basic global access control implemented. 'access' field added to
 *        configuration. No access limitations by default to keep backward
 *        compatibility.
 *
 *    2001-01-17        Sergei Organov <osv@javad.ru>
 *
 *      * Anchor data socket for active mode (using self IP and port 20.)
 *      * Fixed default data port support (still not tested).
 *      * Don't allow IP address different from originating host in
 *        PORT command to improve security.
 *      * Fixed bug in MDTM command.
 *      * Check for correctness of parsing of argument in command_port().
 *      * Fixed squeeze_path() to don't allow names like 'NAME/smth' where
 *        'NAME' is not a directory.
 *      * Command parsing a little bit improved: command names are now
 *        converted to upper-case to be more compatible with RFC (command
 *        names are not case-sensitive.)
 *      * Reformat comments so that they have RTEMS look-and-feel.
 *
 *    2001-01-16        Sergei Organov <osv@javad.ru>
 *
 *      * Fixed DELE, SITE CHMOD, RMD, MKD broken by previous changes
 *      * True ASCII mode implemented (doesn't work for hooks and /dev/null)
 *      * Passive mode implemented, PASV command added.
 *      * Default port for data connection could be used (untested, can't find
 *        ftp client that doesn't send PORT command)
 *      * SYST reply changed to UNIX, as former RTEMS isn't registered name.
 *      * Reply codes reviewed and fixed.
 *
 *    2001-01-08        Sergei Organov <osv@javad.ru>
 *
 *      * use pool of pre-created threads to handle sessions
 *      * LIST output now similar to what "/bin/ls -al" would output, thus
 *        FTP clients could parse it.
 *      * LIST NAME now works (both for files and directories)
 *      * keep track of CWD for every session separately
 *      * ability to specify root directory name in configuration table
 *      * options sent in commands are ignored, thus LIST -al FILE works
 *      * added support for NLST, CDUP and MDTM commands
 *      * buffers are allocated on stack instead of heap where possible
 *      * drop using of task notepad to pass parameters - use function
 *        arguments instead
 *      * various bug-fixes, e.g., use of PF_INET in socket() instead of
 *        AF_INET, use snprintf() instead of sprintf() everywhere for safety,
 *        etc.
 */

/*************************************************************************
 *                                 ftpd.c
 *************************************************************************
 * Description:
 *
 *    This file contains the daemon which services requests that appear
 *    on the FTP port.  This server is compatible with FTP, but it
 *    also provides 'hooks' to make it usable in situations where files
 *    are not used/necessary.  Once the server is started, it runs
 *    forever.
 *
 *
 *    Organization:
 *
 *       The FTP daemon is started upon boot along with a (configurable)
 *       number of tasks to handle sessions.  It runs all the time and
 *       waits for connections on the known FTP port (21).  When
 *       a connection is made, it wakes-up a 'session' task.  That
 *       session then interacts with the remote host.  When the session
 *       is complete, the session task goes to sleep.  The daemon still
 *       runs, however.
 *
 *
 * Supported commands are:
 *
 * RETR xxx     - Sends a file from the client.
 * STOR xxx     - Receives a file from the client.  xxx = filename.
 * LIST xxx     - Sends a file list to the client.
 * NLST xxx     - Sends a file list to the client.
 * USER         - Does nothing.
 * PASS         - Does nothing.
 * SYST         - Replies with the system type (`RTEMS').
 * DELE xxx     - Delete file xxx.
 * MKD xxx      - Create directory xxx.
 * RMD xxx      - Remove directory xxx.
 * PWD          - Print working directory.
 * CWD xxx      - Change working directory.
 * CDUP         - Change to upper directory.
 * SITE CHMOD xxx yyy - Change permissions on file yyy to xxx.
 * PORT a,b,c,d,x,y   - Setup for a data port to IP address a.b.c.d
 *                      and port (x*256 + y).
 * MDTM xxx     - Send file modification date/time to the client.
 *                xxx = filename.
 * PASV         - Use passive mode data connection.
 *
 *
 * The public routines contained in this file are:
 *
 *    rtems_initialize_ftpd - Initializes and starts the server daemon,
 *                            then returns to its caller.
 *
 *------------------------------------------------------------------------
 * Jake Janovetz
 * University of Illinois
 * 1406 West Green Street
 * Urbana IL  61801
 *************************************************************************
 * Change History:
 *  12/01/97   - Creation (JWJ)
 *  2001-01-08 - Changes by OSV
 *  2010-04-29 - Authentication added by AV
 *************************************************************************/

/*************************************************************************
 * Meanings of first and second digits of reply codes:
 *
 * Reply:  Description:
 *-------- --------------
 *  1yz    Positive preliminary reply.  The action is being started but
 *         expect another reply before sending another command.
 *  2yz    Positive completion reply.  A new command can be sent.
 *  3yz    Positive intermediate reply.  The command has been accepted
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <ctype.h>

#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/error.h>
#include <rtems/libio.h>
#include <rtems/userenv.h>
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

#define FTPD_SYSTYPE "UNIX Type: L8"

/* Seem to be unused */
#if 0
#define FTPD_WELCOME_MESSAGE \
   "Welcome to the RTEMS FTP server.\n" \
   "\n" \
   "Login accepted.\n"
#endif

/* Event to be used by session tasks for waiting */
enum
{
  FTPD_RTEMS_EVENT = RTEMS_EVENT_1
};

/* Configuration table */
extern struct rtems_ftpd_configuration rtems_ftpd_configuration;

/* this is not prototyped in strict ansi mode */
FILE *fdopen (int fildes, const char *mode);

/*SessionInfo structure.
 *
 * The following structure is allocated for each session.
 */
typedef struct
{
  struct sockaddr_in  ctrl_addr;   /* Control connection self address */
  struct sockaddr_in  data_addr;   /* Data address set by PORT command */
  struct sockaddr_in  def_addr;    /* Default address for data */
  int                 use_default; /* 1 - use default address for data */
  FILE                *ctrl_fp;    /* File pointer for control connection */
  int                 ctrl_socket; /* Socket for ctrl connection */
  int                 pasv_socket; /* Socket for PASV connection */
  int                 data_socket; /* Socket for data connection */
  int                 idle;        /* Timeout in seconds */
  int                 xfer_mode;   /* Transfer mode (ASCII/binary) */
  rtems_id            tid;         /* Task id */
  char                *user;       /* user name (0 if not supplied) */
  char                *pass;       /* password (0 if not supplied) */
  bool                auth;        /* true if user/pass was valid, false if not or not supplied */
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
 * Root directory
 */

static char const* ftpd_root = "/";

/*
 * Default idle timeout for sockets in seconds.
 */
static int ftpd_timeout = 0;

/*
 * Global access flags.
 */
static int ftpd_access = 0;

/*
 * serr
 *
 * Return error string corresponding to current 'errno'.
 *
 */
static char const*
serr(void)
{
  int err = errno;
  errno = 0;
  return strerror(err);
}

/*
 * Utility routines for access control.
 *
 */

static int
can_read(void)
{
  return (ftpd_access & FTPD_NO_READ) == 0;
}

static int
can_write(void)
{
  return (ftpd_access & FTPD_NO_WRITE) == 0;
}

/*
 * Task pool management routines
 *
 */


/*
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

/*
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
static void session(rtems_task_argument arg); /* Forward declare */

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
    if (++id > 'z')
      id = 'a';
  }
  task_pool.count = count;
  return 1;
}

/*
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
task_pool_obtain(void)
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
    rtems_semaphore_release(task_pool.mutex);
  }
  return info;
}

/*
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

/*
 * Function: send_reply
 *
 *
 *    This procedure sends a reply to the client via the control
 *    connection.
 *
 *
 * Input parameters:
 *   code  - 3-digit reply code.
 *   text  - Reply text.
 *
 * Output parameters:
 *   NONE
 */
static void
send_reply(FTPD_SessionInfo_t  *info, int code, char *text)
{
  text = text != NULL ? text : "";
  fprintf(info->ctrl_fp, "%d %.70s\r\n", code, text);
  fflush(info->ctrl_fp);
}


/*
 * close_socket
 *
 * Close socket.
 *
 * Input parameters:
 *   s - socket descriptor.
 *   seconds - number of seconds the timeout should be,
 *             if >= 0 - infinite timeout (no timeout).
 *
 * Output parameters:
 *   returns 1 on success, 0 on failure.
 */
static int
set_socket_timeout(int s, int seconds)
{
  int res = 0;
  struct timeval tv;
  int len = sizeof(tv);

  if(seconds < 0)
    seconds = 0;
  tv.tv_usec = 0;
  tv.tv_sec  = seconds;
  if(0 != setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &tv, len))
    syslog(LOG_ERR, "ftpd: Can't set send timeout on socket: %s.", serr());
  else if(0 != setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, len))
    syslog(LOG_ERR, "ftpd: Can't set receive timeout on socket: %s.", serr());
  else
    res = 1;
  return res;
}

/*
 * close_socket
 *
 * Close socket.
 *
 * Input parameters:
 *   s - socket descriptor to be closed.
 *
 * Output parameters:
 *   returns 1 on success, 0 on failure
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

/*
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
  int s = info->pasv_socket;
  if(0 > s)
  {
    int on = 1;
    s = socket(PF_INET, SOCK_STREAM, 0);
    if(0 > s)
      send_reply(info, 425, "Can't create data socket.");
    else if(0 > setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)))
    {
      close_socket(s);
      s = -1;
    }
    else
    {
      struct sockaddr_in data_source;
      int tries;

      /* anchor socket to avoid multi-homing problems */
      data_source = info->ctrl_addr;
      data_source.sin_port = htons(20); /* ftp-data port */
      for(tries = 1; tries < 10; ++tries)
      {
        errno = 0;
        if(bind(s, (struct sockaddr *)&data_source, sizeof(data_source)) >= 0)
          break;
        if (errno != EADDRINUSE)
          tries = 10;
        else
          rtems_task_wake_after(tries * 10);
      }
      if(tries >= 10)
      {
        send_reply(info, 425, "Can't bind data socket.");
        close_socket(s);
        s = -1;
      }
      else
      {
        struct sockaddr_in *data_dest =
          (info->use_default) ? &info->def_addr : &info->data_addr;
        if(0 > connect(s, (struct sockaddr *)data_dest, sizeof(*data_dest)))
        {
          send_reply(info, 425, "Can't connect data socket.");
          close_socket(s);
          s = -1;
        }
      }
    }
  }
  info->data_socket = s;
  info->use_default = 1;
  if(s >= 0)
    set_socket_timeout(s, info->idle);
  return s;
}

/*
 * close_data_socket
 *
 * Close data socket for session.
 *
 * Input parameters:
 *   info - corresponding SessionInfo structure
 *
 * Output parameters:
 *   NONE
 *
 */
static void
close_data_socket(FTPD_SessionInfo_t *info)
{
  /* As at most one data socket could be open simultaneously and in some cases
     data_socket == pasv_socket, we select socket to close, then close it. */
  int s = info->data_socket;
  if(0 > s)
    s = info->pasv_socket;
  if(!close_socket(s))
    syslog(LOG_ERR, "ftpd: Error closing data socket.");
  info->data_socket = -1;
  info->pasv_socket = -1;
  info->use_default = 1;
}

/*
 * close_stream
 *
 * Close control stream of session.
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
      info->ctrl_socket = -1;
  }

  if (!close_socket(info->ctrl_socket))
    syslog(LOG_ERR, "ftpd: Could not close control socket: %s", serr());

  info->ctrl_fp = NULL;
  info->ctrl_socket = -1;
}


/*
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

/*
 * command_retrieve
 *
 * Perform the "RETR" command (send file to client).
 *
 * Input parameters:
 *   info - corresponding SessionInfo structure
 *   char *filename  - source filename.
 *
 * Output parameters:
 *   NONE
 *
 */
static void
command_retrieve(FTPD_SessionInfo_t  *info, char const *filename)
{
  int                 s = -1;
  int                 fd = -1;
  char                buf[FTPD_DATASIZE];
  struct stat         stat_buf;
  int                 res = 0;

  if(!can_read() || !info->auth)
  {
    send_reply(info, 550, "Access denied.");
    return;
  }

  if (0 > (fd = open(filename, O_RDONLY)))
  {
    send_reply(info, 550, "Error opening file.");
    return;
  }

  if (fstat(fd, &stat_buf) == 0 && S_ISDIR(stat_buf.st_mode))
  {
    if (-1 != fd)
      close(fd);
    send_reply(info, 550, "Is a directory.");
    return;
  }

  send_mode_reply(info);

  s = data_socket(info);

  if (0 <= s)
  {
    int n = -1;

    if(info->xfer_mode == TYPE_I)
    {
      while ((n = read(fd, buf, FTPD_DATASIZE)) > 0)
      {
        if(send(s, buf, n, 0) != n)
          break;
      }
    }
    else if (info->xfer_mode == TYPE_A)
    {
      int rest = 0;
      while (rest == 0 && (n = read(fd, buf, FTPD_DATASIZE)) > 0)
      {
        char const* e = buf;
        char const* b;
        int i;
        rest = n;
        do
        {
          char lf = '\0';

          b = e;
          for(i = 0; i < rest; ++i, ++e)
          {
            if(*e == '\n')
            {
              lf = '\n';
              break;
            }
          }
          if(send(s, b, i, 0) != i)
            break;
          if(lf == '\n')
          {
            if(send(s, "\r\n", 2, 0) != 2)
              break;
            ++e;
            ++i;
          }
        }
        while((rest -= i) > 0);
      }
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

  if (-1 != fd)
    close(fd);

  if (0 == res)
    send_reply(info, 451, "File read error.");
  else
    send_reply(info, 226, "Transfer complete.");

  close_data_socket(info);

  return;
}


/*
 * discard
 *
 * Analog of `write' routine that just discards passed data
 *
 * Input parameters:
 *   fd    - file descriptor (ignored)
 *   buf   - data to write (ignored)
 *   count - number of bytes in `buf'
 *
 * Output parameters:
 *   returns `count'
 *
 */
static ssize_t
discard(int fd, void const* buf, size_t count)
{
  (void)fd;
  (void)buf;
  return count;
}

/*
 * command_store
 *
 * Performs the "STOR" command (receive data from client).
 *
 * Input parameters:
 *   info - corresponding SessionInfo structure
 *   char *filename   - Destination filename.
 *
 * Output parameters:
 *   NONE
 */
static void
command_store(FTPD_SessionInfo_t *info, char const *filename)
{
  int                    s;
  int                    n;
  unsigned long          size = 0;
  struct rtems_ftpd_hook *usehook = NULL;
  char                   buf[FTPD_DATASIZE];
  int                    res = 1;
  int                    bare_lfs = 0;
  int                    null = 0;
  typedef ssize_t (*WriteProc)(int, void const*, size_t);
  WriteProc              wrt = &write;

  if(!can_write() || !info->auth)
  {
    send_reply(info, 550, "Access denied.");
    return;
  }

  send_mode_reply(info);

  s = data_socket(info);
  if(0 > s)
    return;

  null = !strcmp("/dev/null", filename);
  if (null)
  {
    /* File "/dev/null" just throws data away.
     *  FIXME: this is hack.  Using `/dev/null' filesystem entry would be
     *  better.
     */
    wrt = &discard;
  }

  if (!null && rtems_ftpd_configuration.hooks != NULL)
  {

    /* Search our list of hooks to see if we need to do something special. */
    struct rtems_ftpd_hook *hook;
    int i;

    i = 0;
    hook = &rtems_ftpd_configuration.hooks[i++];
    while (hook->filename != NULL)
    {
      if (!strcmp(hook->filename, filename))
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
     * called multiple times instead.  Alternatively, the support could be
     * removed entirely in favor of configuring RTEMS pseudo-device with
     * given name.
     */

    char                *bigBufr;
    size_t filesize = rtems_ftpd_configuration.max_hook_filesize + 1;

    /*
     * Allocate space for our "file".
     */
    bigBufr = (char *)malloc(filesize);
    if (bigBufr == NULL)
    {
      send_reply(info, 451, "Local resource failure: malloc.");
      close_data_socket(info);
      return;
    }

    /*
     * Retrieve the file into our buffer space.
     */
    size = 0;
    while ((n = recv(s, bigBufr + size, filesize - size, 0)) > 0)
    {
      size += n;
    }
    if (size >= filesize)
    {
      send_reply(info, 451, "File too long: buffer size exceeded.");
      free(bigBufr);
      close_data_socket(info);
      return;
    }

    /*
     * Call our hook.
     */
    res = (usehook->hook_function)(bigBufr, size) == 0;
    free(bigBufr);
    if(!res)
    {
      send_reply(info, 451, "File processing failed.");
      close_data_socket(info);
      return;
    }
  }
  else
  {
    /* Data transfer to regular file or /dev/null. */
    int fd = 0;

    if(!null)
      fd = creat(filename,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

    if (0 > fd)
    {
      send_reply(info, 550, "Error creating file.");
      close_data_socket(info);
      return;
    }

    if(info->xfer_mode == TYPE_I)
    {
      while ((n = recv(s, buf, FTPD_DATASIZE, 0)) > 0)
      {
        if (wrt(fd, buf, n) != n)
        {
          res = 0;
          break;
        }
      }
    }
    else if(info->xfer_mode == TYPE_A)
    {
      int rest = 0;
      int pended_cr = 0;
      while (res && rest == 0 && (n = recv(s, buf, FTPD_DATASIZE, 0)) > 0)
      {
        char const* e = buf;
        char const* b;
        int i;

        rest = n;
        if(pended_cr && *e != '\n')
        {
          char const lf = '\r';
          pended_cr = 0;
          if(wrt(fd, &lf, 1) != 1)
          {
            res = 0;
            break;
          }
        }
        do
        {
          int count;
          int sub = 0;

          b = e;
          for(i = 0; i < rest; ++i, ++e)
          {
            int pcr = pended_cr;
            pended_cr = 0;
            if(*e == '\r')
            {
              pended_cr = 1;
            }
            else if(*e == '\n')
            {
              if(pcr)
              {
                sub = 2;
                ++i;
                ++e;
                break;
              }
              ++bare_lfs;
            }
          }
          if(res == 0)
            break;
          count = i - sub - pended_cr;
          if(count > 0 && wrt(fd, b, count) != count)
          {
            res = 0;
            break;
          }
          if(sub == 2 && wrt(fd, e - 1, 1) != 1)
            res = 0;
        }
        while((rest -= i) > 0);
      }
    }

    if (0 > close(fd) || res == 0)
    {
      send_reply(info, 452, "Error writing file.");
      close_data_socket(info);
      return;
    }
  }

  if (bare_lfs > 0)
  {
    snprintf(buf, FTPD_BUFSIZE,
      "Transfer complete. WARNING! %d bare linefeeds received in ASCII mode.",
      bare_lfs);
    send_reply(info, 226, buf);
  }
  else
    send_reply(info, 226, "Transfer complete.");
  close_data_socket(info);

}


/*
 * send_dirline
 *
 * Sends one line of LIST command reply corresponding to single file.
 *
 * Input parameters:
 *   s - socket descriptor to send data to
 *   wide - if 0, send only file name.  If not 0, send 'stat' info as well in
 *          "ls -l" format.
 *   curTime - current time
 *   path - path to be prepended to what is given by 'add'
 *   add  - path to be appended to what is given by 'path', the resulting path
 *          is then passed to 'stat()' routine
 *   name - file name to be reported in output
 *   buf  - buffer for temporary data
 *
 * Output parameters:
 *   returns 0 on failure, 1 on success
 *
 */
static int
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
          return 0;
        buf[plen] = '\0';
      }
    }
    if(plen + alen >= FTPD_BUFSIZE)
      return 0;
    strcpy(buf + plen, add);

    if (stat(buf, &stat_buf) == 0)
    {
      int len;
      struct tm bt;
      time_t tf = stat_buf.st_mtime;
      enum { SIZE = 80 };
      time_t SIX_MONTHS = (365L*24L*60L*60L)/2L;
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

      if(send(s, buf, len, 0) != len)
        return 0;
    }
  }
  else
  {
    int len = snprintf(buf, FTPD_BUFSIZE, "%s\r\n", fname);
    if(send(s, buf, len, 0) != len)
      return 0;
  }
  return 1;
}

/*
 * command_list
 *
 * Send file list to client.
 *
 * Input parameters:
 *   info - corresponding SessionInfo structure
 *   char *fname  - File (or directory) to list.
 *
 * Output parameters:
 *   NONE
 */
static void
command_list(FTPD_SessionInfo_t *info, char const *fname, int wide)
{
  int                 s;
  DIR                 *dirp = 0;
  struct dirent       *dp = 0;
  struct stat         stat_buf;
  char                buf[FTPD_BUFSIZE];
  time_t curTime;
  int sc = 1;

  if(!info->auth)
  {
    send_reply(info, 550, "Access denied.");
    return;
  }

  send_reply(info, 150, "Opening ASCII mode data connection for LIST.");

  s = data_socket(info);
  if(0 > s)
  {
    syslog(LOG_ERR, "ftpd: Error connecting to data socket.");
    return;
  }

  if(fname[0] == '\0')
    fname = ".";

  if (0 > stat(fname, &stat_buf))
  {
    snprintf(buf, FTPD_BUFSIZE,
      "%s: No such file or directory.\r\n", fname);
    send(s, buf, strlen(buf), 0);
  }
  else if (S_ISDIR(stat_buf.st_mode) && (NULL == (dirp = opendir(fname))))
  {
    snprintf(buf, FTPD_BUFSIZE,
      "%s: Can not open directory.\r\n", fname);
    send(s, buf, strlen(buf), 0);
  }
  else
  {
    time(&curTime);
    if(!dirp && *fname)
      sc = sc && send_dirline(s, wide, curTime, fname, "", fname, buf);
    else {
      /* FIXME: need "." and ".." only when '-a' option is given */
      sc = sc && send_dirline(s, wide, curTime, fname, "", ".", buf);
      sc = sc && send_dirline(s, wide, curTime, fname,
        (strcmp(fname, ftpd_root) ? ".." : ""), "..", buf);
      while (sc && (dp = readdir(dirp)) != NULL)
        sc = sc &&
          send_dirline(s, wide, curTime, fname, dp->d_name, dp->d_name, buf);
    }
  }

  if(dirp)
    closedir(dirp);
  close_data_socket(info);

  if(sc)
    send_reply(info, 226, "Transfer complete.");
  else
    send_reply(info, 426, "Connection aborted.");
}


/*
 * command_cwd
 *
 * Change current working directory.
 *
 * Input parameters:
 *   info - corresponding SessionInfo structure
 *   dir  - directory name passed in CWD command
 *
 * Output parameters:
 *   NONE
 *
 */
static void
command_cwd(FTPD_SessionInfo_t  *info, char *dir)
{
  if(!info->auth)
  {
    send_reply(info, 550, "Access denied.");
    return;
  }

  if(chdir(dir) == 0)
    send_reply(info, 250, "CWD command successful.");
  else
    send_reply(info, 550, "CWD command failed.");
}


/*
 * command_pwd
 *
 * Send current working directory to client.
 *
 * Input parameters:
 *   info - corresponding SessionInfo structure
 *
 * Output parameters:
 *   NONE
 */
static void
command_pwd(FTPD_SessionInfo_t  *info)
{
  char buf[FTPD_BUFSIZE];
  char const* cwd;
  errno = 0;
  buf[0] = '"';

  if(!info->auth)
  {
    send_reply(info, 550, "Access denied.");
    return;
  }

  cwd = getcwd(buf + 1, FTPD_BUFSIZE - 4);
  if(cwd)
  {
    int len = strlen(cwd);
    static char const txt[] = "\" is the current directory.";
    int size = sizeof(txt);
    if(len + size + 1 >= FTPD_BUFSIZE)
      size = FTPD_BUFSIZE - len - 2;
    memcpy(buf + len + 1, txt, size);
    buf[len + size] = '\0';
    send_reply(info, 250, buf);
  }
  else {
    snprintf(buf, FTPD_BUFSIZE, "Error: %s.", serr());
    send_reply(info, 452, buf);
  }
}

/*
 * command_mdtm
 *
 * Handle FTP MDTM command (send file modification time to client)/
 *
 * Input parameters:
 *   info - corresponding SessionInfo structure
 *   fname - file name passed in MDTM command
 *
 * Output parameters:
 *   info->cwd is set to new CWD value.
 */
static void
command_mdtm(FTPD_SessionInfo_t  *info, char const* fname)
{
  struct stat stbuf;
  char buf[FTPD_BUFSIZE];

  if(!info->auth)
  {
    send_reply(info, 550, "Access denied.");
    return;
  }

  if (0 > stat(fname, &stbuf))
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

/*
 * command_port
 *
 * This procedure fills address for data connection given the IP address and
 * port of the remote machine.
 *
 * Input parameters:
 *   info - corresponding SessionInfo structure
 *   args - arguments to the "PORT" command.
 *
 * Output parameters:
 *   info->data_addr is set according to arguments of the PORT command.
 *   info->use_default is set to 0 on success, 1 on failure.
 */
static void
command_port(FTPD_SessionInfo_t *info, char const *args)
{
  enum { NUM_FIELDS = 6 };
  unsigned int a[NUM_FIELDS];
  int n;

  close_data_socket(info);

  n = sscanf(args, "%u,%u,%u,%u,%u,%u", a+0, a+1, a+2, a+3, a+4, a+5);
  if(NUM_FIELDS == n)
  {
    int i;
    union {
      uint8_t b[NUM_FIELDS];
      struct {
        uint32_t ip;
        uint16_t port;
      } u ;
    } ip_info;

    for(i = 0; i < NUM_FIELDS; ++i)
    {
      if(a[i] > 255)
        break;
      ip_info.b[i] = (uint8_t)a[i];
    }

    if(i == NUM_FIELDS)
    {
      /* Note: while it contradicts with RFC959, we don't allow PORT command
       * to specify IP address different than those of the originating client
       * for the sake of safety. */
      if (ip_info.u.ip == info->def_addr.sin_addr.s_addr)
      {
        info->data_addr.sin_addr.s_addr = ip_info.u.ip;
        info->data_addr.sin_port        = ip_info.u.port;
        info->data_addr.sin_family      = AF_INET;
        memset(info->data_addr.sin_zero, 0, sizeof(info->data_addr.sin_zero));

        info->use_default = 0;
        send_reply(info, 200, "PORT command successful.");
        return; /* success */
      }
      else
      {
        send_reply(info, 425, "Address doesn't match peer's IP.");
        return;
      }
    }
  }
  send_reply(info, 501, "Syntax error.");
}


/*
 * command_pasv
 *
 * Handle FTP PASV command.
 * Open socket, listen for and accept connection on it.
 *
 * Input parameters:
 *   info - corresponding SessionInfo structure
 *
 * Output parameters:
 *   info->pasv_socket is set to the descriptor of the data socket
 */
static void
command_pasv(FTPD_SessionInfo_t *info)
{
  int s = -1;
  int err = 1;

  close_data_socket(info);

  s = socket(PF_INET, SOCK_STREAM, 0);
  if (s < 0)
    syslog(LOG_ERR, "ftpd: Error creating PASV socket: %s", serr());
  else
  {
    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);

    addr = info->ctrl_addr;
    addr.sin_port = htons(0);

    if (0 > bind(s, (struct sockaddr *)&addr, addrLen))
      syslog(LOG_ERR, "ftpd: Error binding PASV socket: %s", serr());
    else if (0 > listen(s, 1))
      syslog(LOG_ERR, "ftpd: Error listening on PASV socket: %s", serr());
    else if(set_socket_timeout(s, info->idle))
    {
      char buf[FTPD_BUFSIZE];
      unsigned char const *ip, *p;

      getsockname(s, (struct sockaddr *)&addr, &addrLen);
      ip = (unsigned char const*)&(addr.sin_addr);
      p  = (unsigned char const*)&(addr.sin_port);
      snprintf(buf, FTPD_BUFSIZE, "Entering passive mode (%u,%u,%u,%u,%u,%u).",
        ip[0], ip[1], ip[2], ip[3], p[0], p[1]);
      send_reply(info, 227, buf);

      info->pasv_socket = accept(s, (struct sockaddr *)&addr, &addrLen);
      if (0 > info->pasv_socket)
        syslog(LOG_ERR, "ftpd: Error accepting PASV connection: %s", serr());
      else
      {
        close_socket(s);
        s = -1;
        err = 0;
      }
    }
  }
  if(err)
  {
    /* (OSV) The note is from FreeBSD FTPD.
     * Note: a response of 425 is not mentioned as a possible response to
     * the PASV command in RFC959.  However, it has been blessed as a
     * legitimate response by Jon Postel in a telephone conversation
     * with Rick Adams on 25 Jan 89. */
    send_reply(info, 425, "Can't open passive connection.");
    close_socket(s);
  }
}


/*
 * skip_options
 *
 * Utility routine to skip options (if any) from input command.
 *
 * Input parameters:
 *   p  - pointer to pointer to command
 *
 * Output parameters:
 *   p  - is changed to point to first non-option argument
 */
static void
skip_options(char **p)
{
  char* buf = *p;
  char* last = NULL;
  while(1) {
    while(isspace((unsigned char)*buf))
      ++buf;
    if(*buf == '-') {
      if(*++buf == '-') { /* `--' should terminate options */
        if(isspace((unsigned char)*++buf)) {
          last = buf;
          do ++buf;
          while(isspace((unsigned char)*buf));
          break;
        }
      }
      while(*buf && !isspace((unsigned char)*buf))
        ++buf;
      last = buf;
    }
    else
      break;
  }
  if(last)
    *last = '\0';
  *p = buf;
}

/*
 * split_command
 *
 * Split command into command itself, options, and arguments. Command itself
 * is converted to upper case.
 *
 * Input parameters:
 *   buf - initial command string
 *
 * Output parameter:
 *   buf  - is modified by inserting '\0' at ends of split entities
 *   cmd  - upper-cased command code
 *   opts - string containing all the options
 *   args - string containing all the arguments
 */
static void
split_command(char *buf, char **cmd, char **opts, char **args)
{
  char* eoc;
  char* p = buf;
  while(isspace((unsigned char)*p))
    ++p;
  *cmd = p;
  while(*p && !isspace((unsigned char)*p))
  {
    *p = toupper((unsigned char)*p);
    ++p;
  }
  eoc = p;
  if(*p)
    *p++ = '\0';
  while(isspace((unsigned char)*p))
    ++p;
  *opts = p;
  skip_options(&p);
  *args = p;
  if(*opts == p)
    *opts = eoc;
  while(*p && *p != '\r' && *p != '\n')
    ++p;
  if(*p)
    *p++ = '\0';
}

/*
 * exec_command
 *
 * Parse and execute FTP command.
 *
 * FIXME: This section is somewhat of a hack.  We should have a better
 *        way to parse commands.
 *
 * Input parameters:
 *   info - corresponding SessionInfo structure
 *   cmd  - command to be executed (upper-case)
 *   args - arguments of the command
 *
 * Output parameters:
 *    NONE
 */
static void
exec_command(FTPD_SessionInfo_t *info, char* cmd, char* args)
{
  char fname[FTPD_BUFSIZE];
  int wrong_command = 0;

  fname[0] = '\0';

  if (!strcmp("PORT", cmd))
  {
    command_port(info, args);
  }
  else if (!strcmp("PASV", cmd))
  {
    command_pasv(info);
  }
  else if (!strcmp("RETR", cmd))
  {
    strncpy(fname, args, 254);
    command_retrieve(info, fname);
  }
  else if (!strcmp("STOR", cmd))
  {
    strncpy(fname, args, 254);
    command_store(info, fname);
  }
  else if (!strcmp("LIST", cmd))
  {
    strncpy(fname, args, 254);
    command_list(info, fname, 1);
  }
  else if (!strcmp("NLST", cmd))
  {
    strncpy(fname, args, 254);
    command_list(info, fname, 0);
  }
  else if (!strcmp("MDTM", cmd))
  {
    strncpy(fname, args, 254);
    command_mdtm(info, fname);
  }
  else if (!strcmp("SYST", cmd))
  {
    send_reply(info, 215, FTPD_SYSTYPE);
  }
  else if (!strcmp("TYPE", cmd))
  {
    if (args[0] == 'I')
    {
      info->xfer_mode = TYPE_I;
      send_reply(info, 200, "Type set to I.");
    }
    else if (args[0] == 'A')
    {
      info->xfer_mode = TYPE_A;
      send_reply(info, 200, "Type set to A.");
    }
    else
    {
      info->xfer_mode = TYPE_I;
      send_reply(info, 504, "Type not implemented.  Set to I.");
    }
  }
  else if (!strcmp("USER", cmd))
  {
    sscanf(args, "%254s", fname);
    if (info->user)
      free(info->user);
    if (info->pass)
      free(info->pass);
    info->pass = NULL;
    info->user = strdup(fname);
    if (rtems_ftpd_configuration.login &&
      !rtems_ftpd_configuration.login(info->user, NULL)) {
      info->auth = false;
      send_reply(info, 331, "User name okay, need password.");
    } else {
      info->auth = true;
      send_reply(info, 230, "User logged in.");
    }
  }
  else if (!strcmp("PASS", cmd))
  {
    sscanf(args, "%254s", fname);
    if (info->pass)
      free(info->pass);
    info->pass = strdup(fname);
    if (!info->user) {
      send_reply(info, 332, "Need account to log in");
    } else {
      if (rtems_ftpd_configuration.login &&
        !rtems_ftpd_configuration.login(info->user, info->pass)) {
        info->auth = false;
        send_reply(info, 530, "Not logged in.");
      } else {
        info->auth = true;
        send_reply(info, 230, "User logged in.");
      }
    }
  }
  else if (!strcmp("DELE", cmd))
  {
    if(!can_write() || !info->auth)
    {
      send_reply(info, 550, "Access denied.");
    }
    else if (
      strncpy(fname, args, 254) &&
      unlink(fname) == 0)
    {
      send_reply(info, 257, "DELE successful.");
    }
    else
    {
      send_reply(info, 550, "DELE failed.");
    }
  }
  else if (!strcmp("SITE", cmd))
  {
    char* opts;
    split_command(args, &cmd, &opts, &args);
    if(!strcmp("CHMOD", cmd))
    {
      int mask;

      if(!can_write() || !info->auth)
      {
        send_reply(info, 550, "Access denied.");
      }
      else {
        char *c;
        c = strchr(args, ' ');
        if((c != NULL) && (sscanf(args, "%o", &mask) == 1) && strncpy(fname, c+1, 254) 
          && (chmod(fname, (mode_t)mask) == 0))
          send_reply(info, 257, "CHMOD successful.");
        else
          send_reply(info, 550, "CHMOD failed.");
      }
    }
    else
      wrong_command = 1;
  }
  else if (!strcmp("RMD", cmd))
  {
    if(!can_write() || !info->auth)
    {
      send_reply(info, 550, "Access denied.");
    }
    else if (
      strncpy(fname, args, 254) &&
      rmdir(fname) == 0)
    {
      send_reply(info, 257, "RMD successful.");
    }
    else
    {
      send_reply(info, 550, "RMD failed.");
    }
  }
  else if (!strcmp("MKD", cmd))
  {
    if(!can_write() || !info->auth)
    {
      send_reply(info, 550, "Access denied.");
    }
    else if (
      strncpy(fname, args, 254) &&
      mkdir(fname, S_IRWXU | S_IRWXG | S_IRWXO) == 0)
    {
      send_reply(info, 257, "MKD successful.");
    }
    else
    {
      send_reply(info, 550, "MKD failed.");
    }
  }
  else if (!strcmp("CWD", cmd))
  {
    strncpy(fname, args, 254);
    command_cwd(info, fname);
  }
  else if (!strcmp("CDUP", cmd))
  {
    command_cwd(info, "..");
  }
  else if (!strcmp("PWD", cmd))
  {
    command_pwd(info);
  }
  else
    wrong_command = 1;

  if(wrong_command)
    send_reply(info, 500, "Command not understood.");
}


/*
 * session
 *
 * This task handles single session.  It is waked up when the FTP daemon gets a
 * service request from a remote machine.  Here, we watch for commands that
 * will come through the control connection.  These commands are then parsed
 * and executed until the connection is closed, either unintentionally or
 * intentionally with the "QUIT" command.
 *
 * Input parameters:
 *   arg - pointer to corresponding SessionInfo.
 *
 * Output parameters:
 *   NONE
 */
static void
session(rtems_task_argument arg)
{
  FTPD_SessionInfo_t  *const info = (FTPD_SessionInfo_t  *)arg;
  int chroot_made = 0;

  rtems_libio_set_private_env();

  /* chroot() can fail here because the directory may not exist yet. */
  chroot_made = chroot(ftpd_root) == 0;

  while(1)
  {
    rtems_event_set set;

    rtems_event_receive(FTPD_RTEMS_EVENT, RTEMS_EVENT_ANY, RTEMS_NO_TIMEOUT,
      &set);

    chroot_made = chroot_made || chroot(ftpd_root) == 0;

    errno = 0;

    send_reply(info, 220, FTPD_SERVER_MESSAGE);

    while (1)
    {
      char buf[FTPD_BUFSIZE];
      char *cmd, *opts, *args;

      if (fgets(buf, FTPD_BUFSIZE, info->ctrl_fp) == NULL)
      {
        syslog(LOG_INFO, "ftpd: Connection aborted.");
        break;
      }

      split_command(buf, &cmd, &opts, &args);

      if (!strcmp("QUIT", cmd))
      {
        send_reply(info, 221, "Goodbye.");
        break;
      }
      else
      {
        exec_command(info, cmd, args);
      }
    }

    /* Close connection and put ourselves back into the task pool. */
    close_data_socket(info);
    close_stream(info);
    free(info->user);
    free(info->pass);
    task_pool_release(info);

    chdir("/");
  }
}


/*
 * daemon
 *
 * This task runs forever.  It waits for service requests on the FTP port
 * (port 21 by default).  When a request is received, it opens a new session
 * to handle those requests until the connection is closed.
 *
 * Input parameters:
 *   NONE
 *
 * Output parameters:
 *   NONE
 */
static void
daemon(rtems_task_argument args __attribute__((unused)))
{
  int                 s;
  socklen_t	      addrLen;
  struct sockaddr_in  addr;
  FTPD_SessionInfo_t  *info = NULL;


  s = socket(PF_INET, SOCK_STREAM, 0);
  if (s < 0)
    syslog(LOG_ERR, "ftpd: Error creating socket: %s", serr());

  addr.sin_family      = AF_INET;
  addr.sin_port        = htons(rtems_ftpd_configuration.port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

  if (0 > bind(s, (struct sockaddr *)&addr, sizeof(addr)))
    syslog(LOG_ERR, "ftpd: Error binding control socket: %s", serr());
  else if (0 > listen(s, 1))
    syslog(LOG_ERR, "ftpd: Error listening on control socket: %s", serr());
  else while (1)
  {
    int ss;
    addrLen = sizeof(addr);
    ss = accept(s, (struct sockaddr *)&addr, &addrLen);
    if (0 > ss)
      syslog(LOG_ERR, "ftpd: Error accepting control connection: %s", serr());
    else if(!set_socket_timeout(ss, ftpd_timeout))
      close_socket(ss);
    else
    {
      info = task_pool_obtain();
      if (NULL == info)
      {
        close_socket(ss);
      }
      else
      {
        info->ctrl_socket = ss;
        if ((info->ctrl_fp = fdopen(info->ctrl_socket, "r+")) == NULL)
        {
          syslog(LOG_ERR, "ftpd: fdopen() on socket failed: %s", serr());
          close_stream(info);
          task_pool_release(info);
        }
        else
        {
          /* Initialize corresponding SessionInfo structure */
          info->def_addr = addr;
          if(0 > getsockname(ss, (struct sockaddr *)&addr, &addrLen))
          {
            syslog(LOG_ERR, "ftpd: getsockname(): %s", serr());
            close_stream(info);
            task_pool_release(info);
          }
          else
          {
            info->use_default = 1;
            info->ctrl_addr  = addr;
            info->pasv_socket = -1;
            info->data_socket = -1;
            info->xfer_mode   = TYPE_A;
            info->data_addr.sin_port =
              htons(ntohs(info->ctrl_addr.sin_port) - 1);
            info->idle = ftpd_timeout;
            info->user = NULL;
            info->pass = NULL;
            if (rtems_ftpd_configuration.login)
              info->auth = false;
            else
              info->auth = true;
            /* Wakeup the session task.  The task will call task_pool_release
               after it closes connection. */
            rtems_event_send(info->tid, FTPD_RTEMS_EVENT);
          }
        }
      }
    }
  }
  rtems_task_delete(RTEMS_SELF);
}


/*
 * rtems_ftpd_start
 *
 * Here, we start the FTPD task which waits for FTP requests and services
 * them.  This procedure returns to its caller once the task is started.
 *
 *
 * Input parameters:
 *
 * Output parameters:
 *    returns RTEMS_SUCCESSFUL on successful start of the daemon.
 */
int
rtems_initialize_ftpd(void)
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

  ftpd_timeout = rtems_ftpd_configuration.idle;
  if (ftpd_timeout < 0)
    ftpd_timeout = 0;
  rtems_ftpd_configuration.idle = ftpd_timeout;

  ftpd_access = rtems_ftpd_configuration.access;

  ftpd_root = "/";
  if ( rtems_ftpd_configuration.root &&
       rtems_ftpd_configuration.root[0] == '/' )
    ftpd_root = rtems_ftpd_configuration.root;

  rtems_ftpd_configuration.root = ftpd_root;

  if (rtems_ftpd_configuration.tasks_count <= 0)
    rtems_ftpd_configuration.tasks_count = 1;
  count = rtems_ftpd_configuration.tasks_count;

  if (!task_pool_init(count, priority))
  {
    syslog(LOG_ERR, "ftpd: Could not initialize task pool.");
    return RTEMS_UNSATISFIED;
  }

  sc = rtems_task_create(rtems_build_name('F', 'T', 'P', 'D'),
    priority, RTEMS_MINIMUM_STACK_SIZE,
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

  syslog(LOG_INFO, "ftpd: FTP daemon started (%d session%s max)",
    count, ((count > 1) ? "s" : ""));

  return RTEMS_SUCCESSFUL;
}
