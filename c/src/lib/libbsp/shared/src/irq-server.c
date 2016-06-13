/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief Generic BSP interrupt server implementation.
 */

/*
 * Copyright (c) 2009, 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <stdlib.h>

#include <rtems.h>
#include <rtems/chain.h>
#include <rtems/score/assert.h>

#include <bsp/irq-generic.h>

RTEMS_INTERRUPT_LOCK_DEFINE(
  static,
  bsp_interrupt_server_lock,
  "Interrupt Server"
)

typedef struct bsp_interrupt_server_action {
  struct bsp_interrupt_server_action *next;
  rtems_interrupt_handler handler;
  void *arg;
} bsp_interrupt_server_action;

typedef struct {
  rtems_chain_node node;
  rtems_vector_number vector;
  bsp_interrupt_server_action *actions;
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
  rtems_interrupt_lock_context lock_context;
  bsp_interrupt_server_entry *e = arg;

  bsp_interrupt_vector_disable(e->vector);

  rtems_interrupt_lock_acquire(&bsp_interrupt_server_lock, &lock_context);

  if (rtems_chain_is_node_off_chain(&e->node)) {
    rtems_chain_append_unprotected(&bsp_interrupt_server_chain, &e->node);
  } else {
    ++bsp_interrupt_server_errors;
  }

  rtems_interrupt_lock_release(&bsp_interrupt_server_lock, &lock_context);

  rtems_event_system_send(bsp_interrupt_server_id, RTEMS_EVENT_SYSTEM_SERVER);
}

typedef struct {
  bsp_interrupt_server_entry *entry;
  rtems_option *options;
} bsp_interrupt_server_iterate_entry;

static void bsp_interrupt_server_per_handler_routine(
  void *iterate_arg,
  const char *info,
  rtems_option options,
  rtems_interrupt_handler handler,
  void *handler_arg
)
{
  if (handler == bsp_interrupt_server_trigger) {
    bsp_interrupt_server_iterate_entry *ie = iterate_arg;

    ie->entry = handler_arg;
    *ie->options = options;
  }
}

static bsp_interrupt_server_entry *bsp_interrupt_server_query_entry(
  rtems_vector_number vector,
  rtems_option *trigger_options
)
{
  bsp_interrupt_server_iterate_entry ie = {
    .entry = NULL,
    .options = trigger_options
  };

  rtems_interrupt_handler_iterate(
    vector,
    bsp_interrupt_server_per_handler_routine,
    &ie
  );

  return ie.entry;
}

typedef struct {
  rtems_vector_number vector;
  rtems_option options;
  rtems_interrupt_handler handler;
  void *arg;
  rtems_id task;
  rtems_status_code sc;
} bsp_interrupt_server_helper_data;

static void bsp_interrupt_server_install_helper(void *arg)
{
  bsp_interrupt_server_helper_data *hd = arg;
  rtems_status_code sc;
  bsp_interrupt_server_entry *e;
  bsp_interrupt_server_action *a;
  rtems_option trigger_options;

  a = calloc(1, sizeof(*a));
  if (a == NULL) {
    hd->sc = RTEMS_NO_MEMORY;
    rtems_event_transient_send(hd->task);
    return;
  }

  a->handler = hd->handler;
  a->arg = hd->arg;

  bsp_interrupt_lock();

  e = bsp_interrupt_server_query_entry(hd->vector, &trigger_options);
  if (e == NULL) {
    e = calloc(1, sizeof(*e));
    if (e != NULL) {
      e->vector = hd->vector;
      e->actions = a;

      sc = rtems_interrupt_handler_install(
        hd->vector,
        "IRQS",
        hd->options & RTEMS_INTERRUPT_UNIQUE,
        bsp_interrupt_server_trigger,
        e
      );
      if (sc != RTEMS_SUCCESSFUL) {
        free(e);
      }
    } else {
      sc = RTEMS_NO_MEMORY;
    }
  } else if (
    RTEMS_INTERRUPT_IS_UNIQUE(hd->options)
      || RTEMS_INTERRUPT_IS_UNIQUE(trigger_options)
  ) {
    sc = RTEMS_RESOURCE_IN_USE;
  } else {
    bsp_interrupt_server_action **link = &e->actions;
    bsp_interrupt_server_action *c;

    sc = RTEMS_SUCCESSFUL;

    while ((c = *link) != NULL) {
      if (c->handler == hd->handler && c->arg == hd->arg) {
        sc = RTEMS_TOO_MANY;
        break;
      }

      link = &c->next;
    }

    if (sc == RTEMS_SUCCESSFUL) {
      *link = a;
    }
  }

  bsp_interrupt_unlock();

  if (sc != RTEMS_SUCCESSFUL) {
    free(a);
  }

  hd->sc = sc;
  rtems_event_transient_send(hd->task);
}

static void bsp_interrupt_server_remove_helper(void *arg)
{
  bsp_interrupt_server_helper_data *hd = arg;
  rtems_status_code sc;
  bsp_interrupt_server_entry *e;
  rtems_option trigger_options;

  bsp_interrupt_lock();

  e = bsp_interrupt_server_query_entry(hd->vector, &trigger_options);
  if (e != NULL) {
    bsp_interrupt_server_action **link = &e->actions;
    bsp_interrupt_server_action *c;

    while ((c = *link) != NULL) {
      if (c->handler == hd->handler && c->arg == hd->arg) {
        break;
      }

      link = &c->next;
    }

    if (c != NULL) {
      bool remove_last = e->actions->next == NULL;

      if (remove_last) {
        rtems_interrupt_handler_remove(
          hd->vector,
          bsp_interrupt_server_trigger,
          e
        );
      }

      *link = c->next;
      free(c);

      if (remove_last) {
        free(e);
      }

      sc = RTEMS_SUCCESSFUL;
    } else {
      sc = RTEMS_UNSATISFIED;
    }
  } else {
    sc = RTEMS_INVALID_ID;
  }

  bsp_interrupt_unlock();

  hd->sc = sc;
  rtems_event_transient_send(hd->task);
}

static rtems_status_code bsp_interrupt_server_call_helper(
  rtems_vector_number vector,
  rtems_option options,
  rtems_interrupt_handler handler,
  void *arg,
  void (*helper)(void *)
)
{
  bsp_interrupt_server_helper_data hd = {
    .vector = vector,
    .options = options,
    .handler = handler,
    .arg = arg,
    .task = rtems_task_self()
  };
  bsp_interrupt_server_action a = {
    .handler = helper,
    .arg = &hd
  };
  bsp_interrupt_server_entry e = {
    .vector = BSP_INTERRUPT_VECTOR_MAX + 1,
    .actions = &a
  };

  bsp_interrupt_server_trigger(&e);
  rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);

  return hd.sc;
}

static bsp_interrupt_server_entry *bsp_interrupt_server_get_entry(void)
{
  rtems_interrupt_lock_context lock_context;
  bsp_interrupt_server_entry *e;
  rtems_chain_control *chain;

  rtems_interrupt_lock_acquire(&bsp_interrupt_server_lock, &lock_context);
  chain = &bsp_interrupt_server_chain;

  if (!rtems_chain_is_empty(chain)) {
    e = (bsp_interrupt_server_entry *)
      rtems_chain_get_first_unprotected(chain);
    rtems_chain_set_off_chain(&e->node);
  } else {
    e = NULL;
  }

  rtems_interrupt_lock_release(&bsp_interrupt_server_lock, &lock_context);

  return e;
}

static void bsp_interrupt_server_task(rtems_task_argument arg)
{
  while (true) {
    rtems_event_set events;
    bsp_interrupt_server_entry *e;

    rtems_event_system_receive(
      RTEMS_EVENT_SYSTEM_SERVER,
      RTEMS_EVENT_ALL | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
    );

    while ((e = bsp_interrupt_server_get_entry()) != NULL) {
      bsp_interrupt_server_action *action = e->actions;
      rtems_vector_number vector = e->vector;

      do {
        bsp_interrupt_server_action *current = action;
        action = action->next;
        (*current->handler)(current->arg);
      } while (action != NULL);

      bsp_interrupt_vector_enable(vector);
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
  rtems_status_code sc;

  sc = bsp_interrupt_server_is_initialized();
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  if (server != RTEMS_ID_NONE) {
    return RTEMS_NOT_IMPLEMENTED;
  }

  return bsp_interrupt_server_call_helper(
    vector,
    options,
    handler,
    arg,
    bsp_interrupt_server_install_helper
  );
}

rtems_status_code rtems_interrupt_server_handler_remove(
  rtems_id server,
  rtems_vector_number vector,
  rtems_interrupt_handler handler,
  void *arg
)
{
  rtems_status_code sc;

  sc = bsp_interrupt_server_is_initialized();
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  if (server != RTEMS_ID_NONE) {
    return RTEMS_NOT_IMPLEMENTED;
  }

  return bsp_interrupt_server_call_helper(
    vector,
    0,
    handler,
    arg,
    bsp_interrupt_server_remove_helper
  );
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
  _Assert(sc == RTEMS_SUCCESSFUL);

  return RTEMS_SUCCESSFUL;
}
