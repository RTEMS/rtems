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
#include <stdio.h>
#include <stdlib.h>

#include <rtems.h>
#include <rtems/chain.h>

#include <rtems/media.h>

typedef struct {
  rtems_media_event event;
  char *src;
  rtems_media_worker worker;
  void *arg;
} media_server_event;

static rtems_id message_queue_id = RTEMS_ID_NONE;

static void rtems_media_server(rtems_task_argument arg __attribute__((unused)))
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  while (true) {
    media_server_event msg;
    size_t msg_size = sizeof(msg);

    sc = rtems_message_queue_receive(
      message_queue_id,
      &msg,
      &msg_size,
      RTEMS_WAIT,
      RTEMS_NO_TIMEOUT
    );
    if (sc != RTEMS_SUCCESSFUL) {
      break;
    }

    rtems_media_post_event(msg.event, msg.src, NULL, msg.worker, msg.arg);

    free(msg.src);
  }

  rtems_task_delete(RTEMS_SELF);
}

rtems_status_code rtems_media_server_initialize(
  rtems_task_priority priority,
  size_t stack_size,
  rtems_mode modes,
  rtems_attribute attributes,
  uint32_t message_count
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_id task_id = RTEMS_ID_NONE;

  if (message_queue_id == RTEMS_ID_NONE) {
    sc = rtems_media_initialize();
    if (sc != RTEMS_SUCCESSFUL) {
      goto error;
    }

    sc = rtems_message_queue_create(
      rtems_build_name('M', 'D', 'I', 'A'),
      message_count,
      sizeof(media_server_event),
      RTEMS_DEFAULT_ATTRIBUTES,
      &message_queue_id
    );
    if (sc != RTEMS_SUCCESSFUL) {
      goto error;
    }

    sc = rtems_task_create(
      rtems_build_name('M', 'D', 'I', 'A'),
      priority,
      stack_size,
      modes,
      attributes,
      &task_id
    );
    if (sc != RTEMS_SUCCESSFUL) {
      goto error;
    }

    sc = rtems_task_start(task_id, rtems_media_server, 0);
    if (sc != RTEMS_SUCCESSFUL) {
      goto error;
    }
  }
  
  return RTEMS_SUCCESSFUL;

error:

  if (task_id != RTEMS_ID_NONE) {
    rtems_task_delete(task_id);
  }

  rtems_message_queue_delete(message_queue_id);

  return RTEMS_NO_MEMORY;
}

rtems_status_code rtems_media_server_post_event(
  rtems_media_event event,
  const char *src,
  rtems_media_worker worker,
  void *arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  media_server_event msg = {
    .event = event,
    .src = strdup(src),
    .worker = worker,
    .arg = arg
  };

  if (msg.src == NULL) {
    return RTEMS_NO_MEMORY;
  }

  sc = rtems_message_queue_send(
    message_queue_id,
    &msg,
    sizeof(msg)
  );
  if (sc != RTEMS_SUCCESSFUL) {
    free(msg.src);

    return RTEMS_UNSATISFIED;
  }

  return RTEMS_SUCCESSFUL;
}
