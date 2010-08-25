/**
 * @file
 *
 * @ingroup RTEMSMedia
 *
 * @brief Media implementation.
 */

/*
 * Copyright (c) 2009, 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <rtems.h>
#include <rtems/chain.h>
#include <rtems/media.h>

#define EVENT RTEMS_EVENT_13

typedef struct {
  rtems_chain_node node;
  rtems_media_event event;
  const char *src;
  rtems_media_worker worker;
  void *worker_arg;
} message;

static RTEMS_CHAIN_DEFINE_EMPTY(message_chain);

static rtems_id server_id = RTEMS_ID_NONE;

static void media_server(rtems_task_argument arg __attribute__((unused)))
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  while (true) {
    message *msg = NULL;

    sc = rtems_chain_get_with_wait(
      &message_chain,
      EVENT,
      RTEMS_NO_TIMEOUT,
      (rtems_chain_node **) &msg
    );
    assert(sc == RTEMS_SUCCESSFUL);
    assert(msg != NULL);

    rtems_media_post_event(
      msg->event,
      msg->src,
      NULL,
      msg->worker,
      msg->worker_arg
    );

    free(msg);
  }
}

rtems_status_code rtems_media_server_initialize(
  rtems_task_priority priority,
  size_t stack_size,
  rtems_mode modes,
  rtems_attribute attributes
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (server_id == RTEMS_ID_NONE) {
    sc = rtems_media_initialize();
    if (sc != RTEMS_SUCCESSFUL) {
      goto error;
    }

    sc = rtems_task_create(
      rtems_build_name('M', 'D', 'I', 'A'),
      priority,
      stack_size,
      modes,
      attributes,
      &server_id
    );
    if (sc != RTEMS_SUCCESSFUL) {
      goto error;
    }

    sc = rtems_task_start(server_id, media_server, 0);
    if (sc != RTEMS_SUCCESSFUL) {
      goto error;
    }
  }
  
  return RTEMS_SUCCESSFUL;

error:

  if (server_id != RTEMS_ID_NONE) {
    rtems_task_delete(server_id);
  }

  return RTEMS_NO_MEMORY;
}

rtems_status_code rtems_media_server_post_event(
  rtems_media_event event,
  const char *src,
  rtems_media_worker worker,
  void *worker_arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  size_t src_size = strlen(src) + 1;
  message *msg = malloc(sizeof(*msg) + src_size);

  if (msg != NULL) {
    char *s = (char *) msg + sizeof(*msg);

    memcpy(s, src, src_size);

    msg->event = event;
    msg->src = s;
    msg->worker = worker;
    msg->worker_arg = worker_arg;

    sc = rtems_chain_append_with_notification(
      &message_chain,
      &msg->node,
      server_id,
      EVENT
    );
    if (sc != RTEMS_SUCCESSFUL) {
      sc = RTEMS_NOT_CONFIGURED;
    }
  } else {
    sc = RTEMS_NO_MEMORY;
  }

  return sc;
}
