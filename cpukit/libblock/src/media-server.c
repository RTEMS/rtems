/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSMedia
 *
 * @brief Media implementation.
 */

/*
 * Copyright (C) 2009, 2010 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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

static void media_server(rtems_task_argument arg RTEMS_UNUSED)
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
    rtems_chain_initialize_node(&msg->node);

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
