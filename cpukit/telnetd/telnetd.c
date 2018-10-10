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
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>

#include <rtems.h>
#include <rtems/error.h>
#include <rtems/pty.h>
#include <rtems/shell.h>
#include <rtems/telnetd.h>
#include <rtems/userenv.h>

#ifdef RTEMS_NETWORKING
#include <rtems/rtems_bsdnet.h>
#endif

#define PARANOIA

typedef struct telnetd_context telnetd_context;

typedef struct telnetd_session {
  rtems_pty_context  pty;
  char               peername[16];
  telnetd_context   *ctx;
} telnetd_session;

struct telnetd_context {
  rtems_telnetd_config_table config;
  int                        server_socket;
  uint16_t                   active_clients;
};

typedef union uni_sa {
  struct sockaddr_in sin;
  struct sockaddr     sa;
} uni_sa;

/***********************************************************/
static telnetd_context telnetd_instance;

static telnetd_session *grab_a_Connection(telnetd_context *ctx)
{
  telnetd_session *session;
  uni_sa peer;
  socklen_t address_len;
  int acp_sock;

  if (ctx->active_clients >= ctx->config.client_maximum) {
    return NULL;
  }

  session = malloc(sizeof(*session));
  if (session == NULL) {
    perror("telnetd:malloc");
    return NULL;
  }

  address_len = sizeof(peer.sin);
  acp_sock = accept(ctx->server_socket, &peer.sa, &address_len);
  if (acp_sock<0) {
    perror("telnetd:accept");
    free(session);
    return NULL;
  };

  if (telnet_get_pty(&session->pty, acp_sock) == NULL) {
    syslog( LOG_DAEMON | LOG_ERR, "telnetd: unable to obtain PTY");
    /* NOTE: failing 'do_get_pty()' closed the socket */
    free(session);
    return NULL;
  }

  if (
    inet_ntop(
      AF_INET,
      &peer.sin.sin_addr,
      session->peername,
      sizeof(session->peername)
    ) == NULL
  ) {
    strlcpy(session->peername, "<UNKNOWN>", sizeof(session->peername));
  }

#ifdef PARANOIA
  syslog(LOG_DAEMON | LOG_INFO,
      "telnetd: accepted connection from %s on %s",
      session->peername,
      session->pty.name);
#endif

  ++ctx->active_clients;
  session->ctx = ctx;
  return session;
}


static void release_a_Connection(
  telnetd_context *ctx,
  telnetd_session *session,
  FILE **pstd,
  int n
)
{
#ifdef PARANOIA
  syslog(
    LOG_DAEMON | LOG_INFO,
    "telnetd: releasing connection from %s on %s",
    session->peername,
    session->pty.name
  );
#endif

  --ctx->active_clients;

  while (--n>=0)
    if (pstd[n]) fclose(pstd[n]);

  unlink(session->pty.name);
}

static rtems_id telnetd_spawn_task(
  rtems_name name,
  rtems_task_priority priority,
  size_t stack_size,
  rtems_task_entry entry,
  void *arg
)
{
  rtems_status_code sc;
  rtems_id task_id;

  sc = rtems_task_create(
    name,
    priority,
    stack_size,
    RTEMS_DEFAULT_MODES,
    RTEMS_FLOATING_POINT,
    &task_id
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return RTEMS_ID_NONE;
  }

  (void)rtems_task_start(task_id, entry, (rtems_task_argument) arg);
  return task_id;
}

static void
telnetd_session_task(rtems_task_argument arg);

/***********************************************************/
static void
telnetd_server_task(rtems_task_argument arg)
{
  telnetd_session   *session = NULL;
  rtems_id           task_id;
  telnetd_context   *ctx = (telnetd_context *) arg;

  /* we don't redirect stdio as this probably
   * was started from the console anyway ..
   */
  do {
    session = grab_a_Connection(ctx);

    if (session == NULL) {
      /* if something went wrong, sleep for some time */
      sleep(10);
      continue;
    }

    task_id = telnetd_spawn_task(
      rtems_build_name('T', 'N', 'T', 'a'),
      ctx->config.priority,
      ctx->config.stack_size,
      telnetd_session_task,
      session
    );
    if (task_id == RTEMS_ID_NONE) {
      FILE *dummy;

      /* hmm - the pty driver slot can only be
       * released by opening and subsequently
       * closing the PTY - this also closes
       * the underlying socket. So we mock up
       * a stream...
       */

      if ( !(dummy=fopen(session->pty.name,"r+")) )
        perror("Unable to dummy open the pty, losing a slot :-(");
      release_a_Connection(ctx, session, &dummy, 1);
      free(session);
      sleep(2); /* don't accept connections too fast */
    }
  } while(1);
}

static void telnetd_destroy_context(telnetd_context *ctx)
{
  if (ctx->server_socket >= 0) {
    close(ctx->server_socket);
  }
}

static rtems_status_code telnetd_create_server_socket(telnetd_context *ctx)
{
  uni_sa srv;
  socklen_t address_len;
  int enable;

  ctx->server_socket = socket(PF_INET, SOCK_STREAM, 0);
  if (ctx->server_socket < 0) {
    syslog(LOG_DAEMON | LOG_ERR, "telnetd: cannot create server socket");
    return RTEMS_UNSATISFIED;
  }

  enable = 1;
  (void)setsockopt(
    ctx->server_socket,
    SOL_SOCKET,
    SO_KEEPALIVE,
    &enable,
    sizeof(enable)
  );

  memset(&srv, 0, sizeof(srv));
  srv.sin.sin_family = AF_INET;
  srv.sin.sin_port = htons(23);
  address_len = sizeof(srv.sin);

  if (bind(ctx->server_socket, &srv.sa, address_len) != 0) {
    syslog(LOG_DAEMON | LOG_ERR, "telnetd: cannot bind server socket");
    return RTEMS_RESOURCE_IN_USE;
  };

  if (listen(ctx->server_socket, ctx->config.client_maximum) != 0) {
    syslog(LOG_DAEMON | LOG_ERR, "telnetd: cannot listen on server socket");
    return RTEMS_UNSATISFIED;
  };

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_telnetd_start(const rtems_telnetd_config_table* config)
{
  telnetd_context *ctx = &telnetd_instance;
  rtems_id task_id;
  rtems_status_code sc;

  if (config->command == NULL) {
    syslog(LOG_DAEMON | LOG_ERR, "telnetd: configuration with invalid command");
    return RTEMS_INVALID_ADDRESS;
  }

  if (ctx->config.command != NULL) {
    syslog(LOG_DAEMON | LOG_ERR, "telnetd: already started");
    return RTEMS_RESOURCE_IN_USE;
  }

  ctx->config = *config;

  /* Check priority */
#ifdef RTEMS_NETWORKING
  if (ctx->config.priority == 0) {
    ctx->config.priority = rtems_bsdnet_config.network_task_priority;
  }
#endif
  if (ctx->config.priority == 0) {
    ctx->config.priority = 100;
  }

  /* Check stack size */
  if (ctx->config.stack_size == 0) {
    ctx->config.stack_size = (size_t)32 * 1024;
  }

  if (ctx->config.client_maximum == 0) {
    ctx->config.client_maximum = 5;
  }

  sc = telnetd_create_server_socket(ctx);
  if (sc != RTEMS_SUCCESSFUL) {
    telnetd_destroy_context(ctx);
    return sc;
  }

  task_id = telnetd_spawn_task(
    rtems_build_name('T', 'N', 'T', 'D'),
    ctx->config.priority,
    RTEMS_MINIMUM_STACK_SIZE,
    telnetd_server_task,
    ctx
  );
  if (task_id == RTEMS_ID_NONE) {
    ctx->config.command = NULL;
    syslog(LOG_DAEMON | LOG_ERR, "telnetd: cannot create server task");
    telnetd_destroy_context(ctx);
    return RTEMS_UNSATISFIED;
  }

  syslog(LOG_DAEMON | LOG_INFO, "telnetd: started successfully");
  return RTEMS_SUCCESSFUL;
}

/* utility wrapper */
static void
telnetd_session_task(rtems_task_argument arg)
{
  rtems_status_code    sc;
  FILE                *nstd[3]={0};
  FILE                *ostd[3]={ stdin, stdout, stderr };
  int                  i=0;
  telnetd_session    *session = (telnetd_session *) arg;
  telnetd_context    *ctx = session->ctx;
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
    if ( !(nstd[i]=fopen(session->pty.name,"r+")) ) {
      perror("unable to open stdio");
      goto cleanup;
    }
  }

  stdin  = nstd[0];
  stdout = nstd[1];
  stderr = nstd[2];

  /* call their routine */
  if (ctx->config.login_check != NULL) {
    start = rtems_shell_login_prompt(
      stdin,
      stderr,
      session->pty.name,
      ctx->config.login_check
    );
    login_failed = !start;
  }
  if (start) {
    ctx->config.command( session->pty.name, ctx->config.arg);
  }

  stdin  = ostd[0];
  stdout = ostd[1];
  stderr = ostd[2];

  if (login_failed) {
    syslog(
      LOG_AUTHPRIV | LOG_WARNING,
      "telnetd: to many wrong passwords entered from %s",
      session->peername
    );
  }

cleanup:
  release_a_Connection(ctx, session, nstd, i);
  free(session);
}
