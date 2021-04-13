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
 * Copyright (c) 2009, 2018 embedded brains GmbH and others.
 *
 *   embedded brains GmbH
 *   Dornierstr. 4
 *   D-82178 Puchheim
 *   Germany
 *   <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/queue.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>

#include <rtems.h>
#include <rtems/pty.h>
#include <rtems/shell.h>
#include <rtems/telnetd.h>
#include <rtems/thread.h>
#include <rtems/userenv.h>

#ifdef RTEMS_NETWORKING
#include <rtems/rtems_bsdnet.h>
#endif

#define TELNETD_EVENT_SUCCESS RTEMS_EVENT_0

#define TELNETD_EVENT_ERROR RTEMS_EVENT_1

typedef struct telnetd_context telnetd_context;

typedef struct telnetd_session {
  rtems_pty_context            pty;
  char                         peername[16];
  telnetd_context             *ctx;
  rtems_id                     task_id;
  LIST_ENTRY(telnetd_session)  link;
} telnetd_session;

struct telnetd_context {
  rtems_telnetd_config_table   config;
  int                          server_socket;
  rtems_id                     task_id;
  rtems_mutex                  mtx;
  LIST_HEAD(, telnetd_session) free_sessions;
  telnetd_session              sessions[RTEMS_ZERO_LENGTH_ARRAY];
};

typedef union {
  struct sockaddr_in sin;
  struct sockaddr    sa;
} telnetd_address;

RTEMS_NO_RETURN static void telnetd_session_fatal_error(
  const telnetd_context *ctx
)
{
  (void)rtems_event_send(ctx->task_id, TELNETD_EVENT_ERROR);
  rtems_task_exit();
}

static bool telnetd_login(telnetd_context *ctx, telnetd_session *session)
{
  bool success;

  if (ctx->config.login_check == NULL) {
    return true;
  }

  success = rtems_shell_login_prompt(
    stdin,
    stderr,
    session->pty.name,
    ctx->config.login_check
  );

  if (!success) {
    syslog(
      LOG_AUTHPRIV | LOG_WARNING,
      "telnetd: too many wrong passwords entered from %s",
      session->peername
    );
  }

  return success;
}

static void telnetd_session_task(rtems_task_argument arg)
{
  rtems_status_code sc;
  telnetd_session *session;
  telnetd_context *ctx;
  const char *path;

  session = (telnetd_session *) arg;
  ctx = session->ctx;

  sc = rtems_libio_set_private_env();
  if (sc != RTEMS_SUCCESSFUL) {
    telnetd_session_fatal_error(ctx);
  }

  path = rtems_pty_get_path(&session->pty);

  stdin = fopen(path, "r+");
  if (stdin == NULL) {
    telnetd_session_fatal_error(ctx);
  }

  stdout = fopen(path, "r+");
  if (stdout == NULL) {
    telnetd_session_fatal_error(ctx);
  }

  stderr = fopen(path, "r+");
  if (stderr == NULL) {
    telnetd_session_fatal_error(ctx);
  }

  (void)rtems_event_send(ctx->task_id, TELNETD_EVENT_SUCCESS);

  while (true) {
    rtems_event_set events;

    (void)rtems_event_system_receive(
      RTEMS_EVENT_SYSTEM_SERVER,
      RTEMS_WAIT | RTEMS_EVENT_ALL,
      RTEMS_NO_TIMEOUT,
      &events
    );

    syslog(
      LOG_DAEMON | LOG_INFO,
      "telnetd: accepted connection from %s on %s",
      session->peername,
      path
    );

    if (telnetd_login(ctx, session)) {
      (*ctx->config.command)(session->pty.name, ctx->config.arg);
    }

    syslog(
      LOG_DAEMON | LOG_INFO,
      "telnetd: releasing connection from %s on %s",
      session->peername,
      path
    );

    rtems_pty_close_socket(&session->pty);

    rtems_mutex_lock(&ctx->mtx);
    LIST_INSERT_HEAD(&ctx->free_sessions, session, link);
    rtems_mutex_unlock(&ctx->mtx);
  }
}

static void telnetd_sleep_after_error(void)
{
      /* If something went wrong, sleep for some time */
      rtems_task_wake_after(10 * rtems_clock_get_ticks_per_second());
}

static void telnetd_server_task(rtems_task_argument arg)
{
  telnetd_context *ctx;

  ctx = (telnetd_context *) arg;

  while (true) {
    telnetd_address peer;
    socklen_t address_len;
    int session_socket;
    telnetd_session *session;

    address_len = sizeof(peer.sin);
    session_socket = accept(ctx->server_socket, &peer.sa, &address_len);
    if (session_socket < 0) {
      syslog(LOG_DAEMON | LOG_ERR, "telnetd: cannot accept session");
      telnetd_sleep_after_error();
      continue;
    };

    rtems_mutex_lock(&ctx->mtx);
    session = LIST_FIRST(&ctx->free_sessions);

    if (session == NULL) {
      rtems_mutex_unlock(&ctx->mtx);

      (void)close(session_socket);
      syslog(LOG_DAEMON | LOG_ERR, "telnetd: no free session available");
      telnetd_sleep_after_error();
      continue;
    }

    LIST_REMOVE(session, link);
    rtems_mutex_unlock(&ctx->mtx);

    rtems_pty_set_socket(&session->pty, session_socket);

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

    (void)rtems_event_system_send(session->task_id, RTEMS_EVENT_SYSTEM_SERVER);
  }
}

static void telnetd_destroy_context(telnetd_context *ctx)
{
  telnetd_session *session;

  LIST_FOREACH(session, &ctx->free_sessions, link) {
    if (session->task_id != 0) {
      (void)rtems_task_delete(session->task_id);
    }

    (void)unlink(rtems_pty_get_path(&session->pty));
  }

  if (ctx->server_socket >= 0) {
    (void)close(ctx->server_socket);
  }

  rtems_mutex_destroy(&ctx->mtx);
  free(ctx);
}

static rtems_status_code telnetd_create_server_socket(telnetd_context *ctx)
{
  telnetd_address srv;
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
  srv.sin.sin_port = htons(ctx->config.port);
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

static rtems_status_code telnetd_create_session_tasks(telnetd_context *ctx)
{
  uint16_t i;

  ctx->task_id = rtems_task_self();

  for (i = 0; i < ctx->config.client_maximum; ++i) {
    telnetd_session *session;
    rtems_status_code sc;
    const char *path;
    rtems_event_set events;

    session = &ctx->sessions[i];
    session->ctx = ctx;
    rtems_mutex_init(&ctx->mtx, "Telnet");
    LIST_INSERT_HEAD(&ctx->free_sessions, session, link);

    sc = rtems_task_create(
      rtems_build_name('T', 'N', 'T', 'a' + i % 26),
      ctx->config.priority,
      ctx->config.stack_size,
      RTEMS_DEFAULT_MODES,
      RTEMS_FLOATING_POINT,
      &session->task_id
    );
    if (sc != RTEMS_SUCCESSFUL) {
      syslog(LOG_DAEMON | LOG_ERR, "telnetd: cannot create session task");
      return RTEMS_UNSATISFIED;
    }

    path = rtems_pty_initialize(&session->pty, i);
    if (path == NULL) {
      syslog(LOG_DAEMON | LOG_ERR, "telnetd: cannot create session PTY");
      return RTEMS_UNSATISFIED;
    }

    (void)rtems_task_start(
      session->task_id,
      telnetd_session_task,
      (rtems_task_argument) session
    );

    (void)rtems_event_receive(
      TELNETD_EVENT_SUCCESS | TELNETD_EVENT_ERROR,
      RTEMS_WAIT | RTEMS_EVENT_ANY,
      RTEMS_NO_TIMEOUT,
      &events
    );

    if ((events & TELNETD_EVENT_ERROR) != 0) {
      syslog(LOG_DAEMON | LOG_ERR, "telnetd: cannot initialize session task");
      return RTEMS_UNSATISFIED;
    }
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_telnetd_start(const rtems_telnetd_config_table* config)
{
  telnetd_context *ctx;
  rtems_status_code sc;
  uint16_t client_maximum;

  if (config->command == NULL) {
    syslog(LOG_DAEMON | LOG_ERR, "telnetd: configuration with invalid command");
    return RTEMS_INVALID_ADDRESS;
  }

  if (config->client_maximum == 0) {
    client_maximum = 5;
  } else {
    client_maximum = config->client_maximum;
  }

  ctx = calloc(
    1,
    sizeof(*ctx) + client_maximum * sizeof(ctx->sessions[0])
  );
  if (ctx == NULL) {
    syslog(LOG_DAEMON | LOG_ERR, "telnetd: cannot allocate server context");
    return RTEMS_UNSATISFIED;
  }

  ctx->config = *config;
  ctx->config.client_maximum = client_maximum;
  ctx->server_socket = -1;
  LIST_INIT(&ctx->free_sessions);

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

  if (ctx->config.port == 0) {
    ctx->config.port = 23;
  }

  sc = telnetd_create_server_socket(ctx);
  if (sc != RTEMS_SUCCESSFUL) {
    telnetd_destroy_context(ctx);
    return sc;
  }

  sc = telnetd_create_session_tasks(ctx);
  if (sc != RTEMS_SUCCESSFUL) {
    telnetd_destroy_context(ctx);
    return sc;
  }

  sc = rtems_task_create(
    rtems_build_name('T', 'N', 'T', 'D'),
    ctx->config.priority,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_FLOATING_POINT,
    &ctx->task_id
  );
  if (sc != RTEMS_SUCCESSFUL) {
    syslog(LOG_DAEMON | LOG_ERR, "telnetd: cannot create server task");
    telnetd_destroy_context(ctx);
    return RTEMS_UNSATISFIED;
  }

  (void)rtems_task_start(
    ctx->task_id,
    telnetd_server_task,
    (rtems_task_argument) ctx
  );

  syslog(
    LOG_DAEMON | LOG_INFO,
    "telnetd: started successfully on port %" PRIu16, ctx->config.port
  );
  return RTEMS_SUCCESSFUL;
}
