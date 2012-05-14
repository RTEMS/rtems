/***********************************************************/
/*
 *
 *  The telnet DAEMON
 *
 *  Author: 17,may 2001
 *
 *   WORK: fernando.ruiz@ctv.es
 *   HOME: correo@fernando-ruiz.com
 *
 * After start the net you can start this daemon.
 * It uses the previously inited pseudo-terminales (pty.c)
 * getting a new terminal with getpty(). This function
 * gives a terminal name passing a opened socket like parameter.
 *
 * With register_telnetd() you add a new command in the shell to start
 * this daemon interactively. (Login in /dev/console of course)
 *
 * Sorry but OOB is not still implemented. (This is the first version)
 *
 * Till Straumann <strauman@slac.stanford.edu>
 *  - made the 'shell' interface more generic, i.e. it is now
 *    possible to have 'telnetd' run an arbitrary 'shell'
 *    program.
 *
 * Copyright (c) 2009 embedded brains GmbH and others.
 *
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/error.h>
#include <rtems/pty.h>
#include <rtems/shell.h>
#include <rtems/telnetd.h>
#include <rtems/bspIo.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>

#include <rtems/userenv.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>

#define PARANOIA

extern char *telnet_get_pty(int socket);
extern int   telnet_pty_initialize(void);

struct shell_args {
  char *devname;
  void *arg;
  char  peername[16];
  char  delete_myself;
};

typedef union uni_sa {
  struct sockaddr_in sin;
  struct sockaddr     sa;
} uni_sa;

static int sockpeername(int sock, char *buf, int bufsz);

rtems_id telnetd_dflt_spawn(
  const char *name,
  unsigned priority,
  unsigned stackSize,
  void (*fn)(void*),
  void *fnarg
);

/***********************************************************/
static rtems_id telnetd_task_id = RTEMS_ID_NONE;

rtems_id (*telnetd_spawn_task)(
  const char *,
  unsigned,
  unsigned,
  void (*)(void*),
  void *
) = telnetd_dflt_spawn;

static char *grab_a_Connection(
  int des_socket,
  uni_sa *srv,
  char *peername,
  int sz
)
{
  char *rval = 0;
#if 0
  socklen_t size_adr = sizeof(srv->sin);
#else
  /* 4.6 doesn't have socklen_t */
  uint32_t size_adr = sizeof(srv->sin);
#endif
  int acp_sock;

  acp_sock = accept(des_socket,&srv->sa,&size_adr);

  if (acp_sock<0) {
    perror("telnetd:accept");
    goto bailout;
  };

  if ( !(rval=telnet_get_pty(acp_sock)) ) {
    syslog( LOG_DAEMON | LOG_ERR, "telnetd: unable to obtain PTY");
    /* NOTE: failing 'do_get_pty()' closed the socket */
    goto bailout;
  }

  if (sockpeername(acp_sock, peername, sz))
    strncpy(peername, "<UNKNOWN>", sz);

#ifdef PARANOIA
  syslog(LOG_DAEMON | LOG_INFO,
      "telnetd: accepted connection from %s on %s",
      peername,
      rval);
#endif

bailout:

  return rval;
}


static void release_a_Connection(char *devname, char *peername, FILE **pstd, int n)
{

#ifdef PARANOIA
  syslog( LOG_DAEMON | LOG_INFO,
      "telnetd: releasing connection from %s on %s",
      peername,
      devname );
#endif

  while (--n>=0)
    if (pstd[n]) fclose(pstd[n]);

}

static int sockpeername(int sock, char *buf, int bufsz)
{
  uni_sa peer;
  int    rval = sock < 0;
#if 0
  socklen_t len  = sizeof(peer.sin);
#else
  /* 4.6 doesn't have socklen_t */
  uint32_t len  = sizeof(peer.sin);
#endif

  if ( !rval )
    rval = getpeername(sock, &peer.sa, &len);

  if ( !rval )
    rval = !inet_ntop( AF_INET, &peer.sin.sin_addr, buf, bufsz );

  return rval;
}

static void
spawned_shell(void *arg);

/***********************************************************/
static void
rtems_task_telnetd(void *task_argument)
{
  int                des_socket;
  uni_sa             srv;
  char              *devname;
  char               peername[16];
  int                i=1;
  int                size_adr;
  struct shell_args *arg = NULL;

  if ((des_socket=socket(PF_INET,SOCK_STREAM,0))<0) {
    perror("telnetd:socket");
    telnetd_task_id = RTEMS_ID_NONE;
    rtems_task_delete(RTEMS_SELF);
  };
  setsockopt(des_socket,SOL_SOCKET,SO_KEEPALIVE,&i,sizeof(i));

  memset(&srv,0,sizeof(srv));
  srv.sin.sin_family=AF_INET;
  srv.sin.sin_port=htons(23);
  size_adr=sizeof(srv.sin);
  if ((bind(des_socket,&srv.sa,size_adr))<0) {
    perror("telnetd:bind");
    close(des_socket);
    telnetd_task_id = RTEMS_ID_NONE;
    rtems_task_delete(RTEMS_SELF);
  };
  if ((listen(des_socket,5))<0) {
    perror("telnetd:listen");
          close(des_socket);
    telnetd_task_id = RTEMS_ID_NONE;
    rtems_task_delete(RTEMS_SELF);
  };

  /* we don't redirect stdio as this probably
   * was started from the console anyways..
   */
  do {
    if (rtems_telnetd_config.keep_stdio) {
      bool start = true;
      char device_name [32];

      ttyname_r( 1, device_name, sizeof( device_name));
      if (rtems_telnetd_config.login_check != NULL) {
        start = rtems_shell_login_prompt(
          stdin,
          stderr,
          device_name,
          rtems_telnetd_config.login_check
        );
      }
      if (start) {
        rtems_telnetd_config.command( device_name, arg->arg);
      } else {
        syslog(
          LOG_AUTHPRIV | LOG_WARNING,
          "telnetd: to many wrong passwords entered from %s",
          device_name
        );
      }
    } else {
      devname = grab_a_Connection(des_socket, &srv, peername, sizeof(peername));

      if ( !devname ) {
        /* if something went wrong, sleep for some time */
        sleep(10);
        continue;
      }

      arg = malloc( sizeof(*arg) );

      arg->devname = devname;
      arg->arg = rtems_telnetd_config.arg;
      strncpy(arg->peername, peername, sizeof(arg->peername));

      telnetd_task_id = telnetd_spawn_task(
        devname,
        rtems_telnetd_config.priority,
        rtems_telnetd_config.stack_size,
        spawned_shell,
        arg
      );
      if (telnetd_task_id == RTEMS_ID_NONE) {
        FILE *dummy;

        if ( telnetd_spawn_task != telnetd_dflt_spawn ) {
          fprintf(stderr,"Telnetd: Unable to spawn child task\n");
        }

        /* hmm - the pty driver slot can only be
         * released by opening and subsequently
         * closing the PTY - this also closes
         * the underlying socket. So we mock up
         * a stream...
         */

        if ( !(dummy=fopen(devname,"r+")) )
          perror("Unable to dummy open the pty, losing a slot :-(");
        release_a_Connection(devname, peername, &dummy, 1);
        free(arg);
        sleep(2); /* don't accept connections too fast */
      }
    }
  } while(1);

  /* TODO: how to free the connection semaphore? But then -
   *       stopping the daemon is probably only needed during
   *       development/debugging.
   *       Finalizer code should collect all the connection semaphore
   *       counts and eventually clean up...
   */
  close(des_socket);
  telnetd_task_id = RTEMS_ID_NONE;
}

rtems_status_code rtems_telnetd_initialize( void)
{
  if (telnetd_task_id != RTEMS_ID_NONE) {
    fprintf(stderr, "telnetd already started\n");
    return RTEMS_RESOURCE_IN_USE;
  }

  if (rtems_telnetd_config.command == NULL) {
    fprintf(stderr, "telnetd setup with invalid command\n");
    return RTEMS_IO_ERROR;
  }

  if ( !telnet_pty_initialize() ) {
    fprintf(stderr, "telnetd cannot initialize PTY driver\n");
    return RTEMS_IO_ERROR;
  }

  /* Check priority */
  if (rtems_telnetd_config.priority <= 0) {
    rtems_telnetd_config.priority = rtems_bsdnet_config.network_task_priority;
  }
  if (rtems_telnetd_config.priority < 2) {
    rtems_telnetd_config.priority = 100;
  }

  /* Check stack size */
  if (rtems_telnetd_config.stack_size <= 0) {
    rtems_telnetd_config.stack_size = (size_t)32 * 1024;
  }

  /* Spawn task */
  telnetd_task_id = telnetd_spawn_task(
    "TNTD",
    rtems_telnetd_config.priority,
    rtems_telnetd_config.stack_size,
    rtems_task_telnetd,
    0
  );
  if (telnetd_task_id == RTEMS_ID_NONE) {
    return RTEMS_IO_ERROR;
  }

  /* Print status */
  if (!rtems_telnetd_config.keep_stdio) {
    fprintf(
      stderr,
      "telnetd started with stacksize = %u and priority = %d\n",
      (unsigned) rtems_telnetd_config.stack_size,
      (unsigned) rtems_telnetd_config.priority
    );
  }

  return RTEMS_SUCCESSFUL;
}

/* utility wrapper */
static void
spawned_shell(void *targ)
{
  rtems_status_code    sc;
  FILE                *nstd[3]={0};
  FILE                *ostd[3]={ stdin, stdout, stderr };
  int                  i=0;
  struct shell_args  *arg = targ;
  bool login_failed = false;
  bool start = true;

  sc=rtems_libio_set_private_env();

  /* newlib hack/workaround. Before we change stdin/out/err we must make
         * sure the internal data are initialized (fileno(stdout) has this sideeffect).
   * This should probably be done from RTEMS' libc support layer...
   * (T.S., newlibc-1.13; 2005/10)
         */

  fileno(stdout);

  if (RTEMS_SUCCESSFUL != sc) {
    rtems_error(sc,"rtems_libio_set_private_env");
    goto cleanup;
  }

  /* redirect stdio */
  for (i=0; i<3; i++) {
    if ( !(nstd[i]=fopen(arg->devname,"r+")) ) {
      perror("unable to open stdio");
      goto cleanup;
    }
  }

  stdin  = nstd[0];
  stdout = nstd[1];
  stderr = nstd[2];

  #if 0
    printk("STDOUT is now %x (%x) (FD %i/%i)\n",
           stdout,nstd[1],fileno(stdout),fileno(nstd[1]));
    printf("hello\n");
    write(fileno(stdout),"hellofd\n",8);
  #endif

  /* call their routine */
  if (rtems_telnetd_config.login_check != NULL) {
    start = rtems_shell_login_prompt(
      stdin,
      stderr,
      arg->devname,
      rtems_telnetd_config.login_check
    );
    login_failed = !start;
  }
  if (start) {
    rtems_telnetd_config.command( arg->devname, arg->arg);
  }

  stdin  = ostd[0];
  stdout = ostd[1];
  stderr = ostd[2];

  if (login_failed) {
    syslog(
      LOG_AUTHPRIV | LOG_WARNING,
      "telnetd: to many wrong passwords entered from %s",
      arg->peername
    );
  }

cleanup:
  release_a_Connection(arg->devname, arg->peername, nstd, i);
  free(arg);
}

struct wrap_delete_args {
  void (*t)(void *);
  void           *a;
};

static rtems_task
wrap_delete(rtems_task_argument arg)
{
  struct wrap_delete_args     *pwa = (struct wrap_delete_args *)arg;
  register void              (*t)(void *) = pwa->t;
  register void               *a   = pwa->a;

  /* free argument before calling function (which may never return if
   * they choose to delete themselves)
   */
  free(pwa);
  t(a);
  rtems_task_delete(RTEMS_SELF);
}

rtems_id
telnetd_dflt_spawn(const char *name, unsigned int priority, unsigned int stackSize, void (*fn)(void *), void* fnarg)
{
  rtems_status_code        sc;
  rtems_id                 task_id = RTEMS_ID_NONE;
  char                     nm[4] = {'X','X','X','X' };
  struct wrap_delete_args *pwa = malloc(sizeof(*pwa));

  strncpy(nm, name, 4);

  if ( !pwa ) {
    perror("Telnetd: no memory\n");
    return RTEMS_ID_NONE;
  }

  pwa->t = fn;
  pwa->a = fnarg;

  if ((sc=rtems_task_create(
    rtems_build_name(nm[0], nm[1], nm[2], nm[3]),
      (rtems_task_priority)priority,
      stackSize,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES | RTEMS_FLOATING_POINT,
      &task_id)) ||
    (sc=rtems_task_start(
      task_id,
      wrap_delete,
      (rtems_task_argument)pwa))) {
        free(pwa);
        rtems_error(sc,"Telnetd: spawning task failed");
        return RTEMS_ID_NONE;
  }
  return task_id;
}

/* convenience routines for CEXP (retrieve stdio descriptors
 * from reent structure)
 */
#ifdef stdin
static __inline__ FILE *
_stdin(void)  { return stdin; }
#undef stdin
FILE *stdin(void)  { return _stdin(); }
#endif
#ifdef stdout
static __inline__ FILE *
_stdout(void) { return stdout; }
#undef stdout
FILE *stdout(void) { return _stdout(); }
#endif
#ifdef stderr
static __inline__ FILE *
_stderr(void) { return stderr; }
#undef stderr
FILE *stderr(void) { return _stderr(); }
#endif

/* MUST NOT USE stdin & friends below here !!!!!!!!!!!!! */
