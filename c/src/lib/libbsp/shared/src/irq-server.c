/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief Generic BSP interrupt server implementation.
 */

/*
 * Copyright (c) 2009, 2010
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

#include <stdlib.h>

#include <rtems.h>
#include <rtems/chain.h>

#include <bsp/irq-generic.h>

#define BSP_INTERRUPT_EVENT RTEMS_EVENT_13

typedef struct bsp_interrupt_server_entry {
  rtems_chain_node node;
  rtems_vector_number vector;
  rtems_interrupt_handler handler;
  void *arg;
} bsp_interrupt_server_entry;

static rtems_id bsp_interrupt_server_id = RTEMS_ID_NONE;

static RTEMS_CHAIN_DEFINE_EMPTY(bsp_interrupt_server_chain);

static rtems_status_code bsp_interrupt_server_is_initialized(void)
{
  if (bsp_interrupt_server_id != RTEMS_ID_NONE) {
    return RTEMS_SUCCESSFUL;
  } else {
    return RTEMS_INCORRECT_STATE;
  }
}

static unsigned bsp_interrupt_server_errors;

static void bsp_interrupt_server_trigger(void *arg)
{
  bsp_interrupt_server_entry *e = arg;

  bsp_interrupt_vector_disable(e->vector);

  if (e->node.next == NULL) {
    rtems_chain_append(&bsp_interrupt_server_chain, &e->node);
  } else {
    ++bsp_interrupt_server_errors;
  }

  rtems_event_send(bsp_interrupt_server_id, BSP_INTERRUPT_EVENT);
}

static bsp_interrupt_server_entry *bsp_interrupt_server_get_entry(void)
{
  rtems_interrupt_level level;
  bsp_interrupt_server_entry *e;

  rtems_interrupt_disable(level);
  e = (bsp_interrupt_server_entry *)
    rtems_chain_get_unprotected(&bsp_interrupt_server_chain);
  if (e != NULL) {
    e->node.next = NULL;
  }
  rtems_interrupt_enable(level);

  return e;
}

static void bsp_interrupt_server_task(rtems_task_argument arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  while (true) {
    rtems_event_set events = 0;
    bsp_interrupt_server_entry *e = NULL;

    sc = rtems_event_receive(
      BSP_INTERRUPT_EVENT,
      RTEMS_EVENT_ALL | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
    );
    if (sc != RTEMS_SUCCESSFUL) {
      break;
    }

    while ((e = bsp_interrupt_server_get_entry()) != NULL) {
      (*e->handler)(e->arg);

      bsp_interrupt_vector_enable(e->vector);
    }
  }

  rtems_task_delete(RTEMS_SELF);
}

typedef struct {
  rtems_interrupt_handler handler;
  void *arg;
  bsp_interrupt_server_entry *entry;
} bsp_interrupt_server_iterate_entry;

static void bsp_interrupt_server_per_handler_routine(
  void *iterate_arg,
  const char *info,
  rtems_option options,
  rtems_interrupt_handler handler,
  void *handler_arg
)
{
  bsp_interrupt_server_iterate_entry *ie = iterate_arg;
  bsp_interrupt_server_entry *e = handler_arg;

  if (handler == bsp_interrupt_server_trigger) {
    if (e->handler == ie->handler && e->arg == ie->arg) {
      ie->entry = e;
    }
  }
}

rtems_status_code rtems_interrupt_server_handler_install(
  rtems_id server,
  rtems_vector_number vector,
  const char *info,
  rtems_option options,
  rtems_interrupt_handler handler,
  void *arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  bsp_interrupt_server_entry *e = NULL;

  sc = bsp_interrupt_server_is_initialized();
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  if (server != RTEMS_ID_NONE) {
    return RTEMS_NOT_IMPLEMENTED;
  }

  if (RTEMS_INTERRUPT_IS_SHARED(options)) {
    return RTEMS_NOT_IMPLEMENTED;
  }

  e = calloc(1, sizeof(*e));
  if (e == NULL) {
    return RTEMS_NO_MEMORY;
  }

  e->vector = vector;
  e->handler = handler;
  e->arg = arg;

  sc = rtems_interrupt_handler_install(
    vector,
    info,
    options,
    bsp_interrupt_server_trigger,
    e
  );
  if (sc != RTEMS_SUCCESSFUL) {
    free(e);

    return sc;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_interrupt_server_handler_remove(
  rtems_id server,
  rtems_vector_number vector,
  rtems_interrupt_handler handler,
  void *arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  bsp_interrupt_server_iterate_entry ie = {
    .handler = handler,
    .arg = arg,
    .entry = NULL
  };

  sc = bsp_interrupt_server_is_initialized();
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  if (server != RTEMS_ID_NONE) {
    return RTEMS_NOT_IMPLEMENTED;
  }

  /* Query corresponding interrupt server entry */
  sc = rtems_interrupt_handler_iterate(
    vector,
    bsp_interrupt_server_per_handler_routine,
    &ie
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  } else if (ie.entry == NULL) {
    return RTEMS_INVALID_ID;
  }

  sc = rtems_interrupt_handler_remove(
    vector,
    ie.entry->handler,
    ie.entry->arg
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  free(ie.entry);

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_interrupt_server_initialize(
  rtems_task_priority priority,
  size_t stack_size,
  rtems_mode modes,
  rtems_attribute attributes,
  rtems_id *server
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (server != NULL) {
    return RTEMS_NOT_IMPLEMENTED;
  }

  sc = rtems_task_create(
    rtems_build_name('I', 'R', 'Q', 'S'),
    priority,
    stack_size,
    modes,
    attributes,
    &bsp_interrupt_server_id
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return RTEMS_TOO_MANY;
  }

  sc = rtems_task_start(
    bsp_interrupt_server_id,
    bsp_interrupt_server_task,
    0
  );
  if (sc != RTEMS_SUCCESSFUL) {
    /* In this case we could also panic */
    rtems_task_delete(bsp_interrupt_server_id);
    bsp_interrupt_server_id = RTEMS_ID_NONE;

    return RTEMS_TOO_MANY;
  }

  return RTEMS_SUCCESSFUL;
}
