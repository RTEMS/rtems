/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief Generic BSP interrupt server implementation.
 */

/*
 * Copyright (c) 2009
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

#include <bsp/irq-generic.h>

typedef struct bsp_interrupt_server_entry {
  rtems_vector_number vector;
  rtems_interrupt_handler handler;
  void *arg;
  struct bsp_interrupt_server_entry *volatile next;
} bsp_interrupt_server_entry;

static rtems_id bsp_interrupt_server_semaphore = RTEMS_ID_NONE;

/* LIFO list head */
static bsp_interrupt_server_entry *volatile
bsp_interrupt_server_list_head = NULL;

static rtems_status_code bsp_interrupt_server_is_initialized(void)
{
  if (bsp_interrupt_server_semaphore != RTEMS_ID_NONE) {
    return RTEMS_SUCCESSFUL;
  } else {
    return RTEMS_INCORRECT_STATE;
  }
}

static void bsp_interrupt_server_trigger(void *arg)
{
  bsp_interrupt_server_entry *e = arg;
  rtems_interrupt_level level;

  bsp_interrupt_vector_disable(e->vector);

  /* Add interrupt server entry to the list */
  rtems_interrupt_disable(level);
  e->next = bsp_interrupt_server_list_head;
  bsp_interrupt_server_list_head = e;
  rtems_interrupt_enable(level);

  rtems_semaphore_release(bsp_interrupt_server_semaphore);
}

static void bsp_interrupt_server_task(rtems_task_argument arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_id sema = bsp_interrupt_server_semaphore;
  rtems_interrupt_level level;
  bsp_interrupt_server_entry *e = NULL;

  while (true) {
    sc = rtems_semaphore_obtain(sema, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL) {
      break;
    }

    /* Fetch next interrupt server entry from the list */
    rtems_interrupt_disable(level);
    e = bsp_interrupt_server_list_head;
    bsp_interrupt_server_list_head = e->next;
    rtems_interrupt_enable(level);

    (*e->handler)(e->arg);

    bsp_interrupt_vector_enable(e->vector);
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

  e = malloc(sizeof(bsp_interrupt_server_entry));
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
  rtems_id sema_id = RTEMS_ID_NONE;
  rtems_id task_id = RTEMS_ID_NONE;
  rtems_interrupt_level level;

  if (server != NULL) {
    return RTEMS_NOT_IMPLEMENTED;
  }

  sc = rtems_semaphore_create(
    rtems_build_name('I', 'R', 'Q', 'S'),
    0,
    RTEMS_LOCAL | RTEMS_FIFO | RTEMS_COUNTING_SEMAPHORE,
    0,
    &sema_id
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  sc = rtems_task_create(
    rtems_build_name('I', 'R', 'Q', 'S'),
    priority,
    stack_size,
    modes,
    attributes,
    &task_id
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_semaphore_delete(sema_id);

    return sc;
  }

  /* Initialize global data (this must be done before the task starts) */
  rtems_interrupt_disable(level);
  if (bsp_interrupt_server_semaphore == RTEMS_ID_NONE) {
    bsp_interrupt_server_semaphore = sema_id;
    sc = RTEMS_SUCCESSFUL;
  } else {
    sc = RTEMS_INCORRECT_STATE;
  }
  rtems_interrupt_enable(level);
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_semaphore_delete(sema_id);
    rtems_task_delete(task_id);

    return sc;
  }

  sc = rtems_task_start(
    task_id,
    bsp_interrupt_server_task,
    0
  );
  if (sc != RTEMS_SUCCESSFUL) {
    /* In this case we could also panic */
    bsp_interrupt_server_semaphore = RTEMS_ID_NONE;
    rtems_semaphore_delete(sema_id);
    rtems_task_delete(task_id);

    return sc;
  }

  return RTEMS_SUCCESSFUL;
}
