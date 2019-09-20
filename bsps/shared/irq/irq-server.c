/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief Generic BSP interrupt server implementation.
 */

/*
 * Copyright (c) 2009, 2019 embedded brains GmbH.  All rights reserved.
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

#define BSP_INTERRUPT_SERVER_MANAGEMENT_VECTOR (BSP_INTERRUPT_VECTOR_MAX + 1)

typedef struct {
  RTEMS_INTERRUPT_LOCK_MEMBER(lock);
  rtems_chain_control entries;
  rtems_id server;
  unsigned errors;
} bsp_interrupt_server_context;

#if defined(RTEMS_SMP)
static bsp_interrupt_server_context *bsp_interrupt_server_instances;
#else
static bsp_interrupt_server_context bsp_interrupt_server_instance;
#endif

static bsp_interrupt_server_context *bsp_interrupt_server_get_context(
  uint32_t server_index,
  rtems_status_code *sc
)
{
#if defined(RTEMS_SMP)
  if (bsp_interrupt_server_instances == NULL) {
    *sc = RTEMS_INCORRECT_STATE;
    return NULL;
  }
#else
  if (bsp_interrupt_server_instance.server == RTEMS_ID_NONE) {
    *sc = RTEMS_INCORRECT_STATE;
    return NULL;
  }
#endif

  if (server_index >= rtems_scheduler_get_processor_maximum()) {
    *sc = RTEMS_INVALID_ID;
    return NULL;
  }

  *sc = RTEMS_SUCCESSFUL;
#if defined(RTEMS_SMP)
  return &bsp_interrupt_server_instances[server_index];
#else
  return &bsp_interrupt_server_instance;
#endif
}

static void bsp_interrupt_server_trigger(void *arg)
{
  rtems_interrupt_lock_context lock_context;
  rtems_interrupt_server_entry *e = arg;
  bsp_interrupt_server_context *s = e->server;

  if (bsp_interrupt_is_valid_vector(e->vector)) {
    bsp_interrupt_vector_disable(e->vector);
  }

  rtems_interrupt_lock_acquire(&s->lock, &lock_context);

  if (rtems_chain_is_node_off_chain(&e->node)) {
    rtems_chain_append_unprotected(&s->entries, &e->node);
  } else {
    ++s->errors;
  }

  rtems_interrupt_lock_release(&s->lock, &lock_context);

  rtems_event_system_send(s->server, RTEMS_EVENT_SYSTEM_SERVER);
}

typedef struct {
  rtems_interrupt_server_entry *entry;
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

static rtems_interrupt_server_entry *bsp_interrupt_server_query_entry(
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
  bsp_interrupt_server_context *server;
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
  rtems_interrupt_server_entry *e;
  rtems_interrupt_server_action *a;
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
      e->server = hd->server;
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
#if defined(RTEMS_SMP)
  } else if (e->server != hd->server) {
    sc = RTEMS_RESOURCE_IN_USE;
#endif
  } else if (
    RTEMS_INTERRUPT_IS_UNIQUE(hd->options)
      || RTEMS_INTERRUPT_IS_UNIQUE(trigger_options)
  ) {
    sc = RTEMS_RESOURCE_IN_USE;
  } else {
    rtems_interrupt_server_action **link = &e->actions;
    rtems_interrupt_server_action *c;

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
  rtems_interrupt_server_entry *e;
  rtems_option trigger_options;

  bsp_interrupt_lock();

  e = bsp_interrupt_server_query_entry(hd->vector, &trigger_options);
  if (e != NULL) {
    rtems_interrupt_server_action **link = &e->actions;
    rtems_interrupt_server_action *c;

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
  bsp_interrupt_server_context *s,
  rtems_vector_number vector,
  rtems_option options,
  rtems_interrupt_handler handler,
  void *arg,
  void (*helper)(void *)
)
{
  bsp_interrupt_server_helper_data hd = {
    .server = s,
    .vector = vector,
    .options = options,
    .handler = handler,
    .arg = arg,
    .task = rtems_task_self()
  };
  rtems_interrupt_server_action a = {
    .handler = helper,
    .arg = &hd
  };
  rtems_interrupt_server_entry e = {
    .server = s,
    .vector = BSP_INTERRUPT_SERVER_MANAGEMENT_VECTOR,
    .actions = &a
  };

  bsp_interrupt_server_trigger(&e);
  rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);

  return hd.sc;
}

static rtems_interrupt_server_entry *bsp_interrupt_server_get_entry(
  bsp_interrupt_server_context *s
)
{
  rtems_interrupt_lock_context lock_context;
  rtems_interrupt_server_entry *e;

  rtems_interrupt_lock_acquire(&s->lock, &lock_context);

  if (!rtems_chain_is_empty(&s->entries)) {
    e = (rtems_interrupt_server_entry *)
      rtems_chain_get_first_unprotected(&s->entries);
    rtems_chain_set_off_chain(&e->node);
  } else {
    e = NULL;
  }

  rtems_interrupt_lock_release(&s->lock, &lock_context);

  return e;
}

static void bsp_interrupt_server_task(rtems_task_argument arg)
{
  bsp_interrupt_server_context *s = (bsp_interrupt_server_context *) arg;

  while (true) {
    rtems_event_set events;
    rtems_interrupt_server_entry *e;

    rtems_event_system_receive(
      RTEMS_EVENT_SYSTEM_SERVER,
      RTEMS_EVENT_ALL | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
    );

    while ((e = bsp_interrupt_server_get_entry(s)) != NULL) {
      rtems_interrupt_server_action *action = e->actions;
      rtems_vector_number vector = e->vector;

      do {
        rtems_interrupt_server_action *current = action;
        action = action->next;
        (*current->handler)(current->arg);
      } while (action != NULL);

      if (bsp_interrupt_is_valid_vector(vector)) {
        bsp_interrupt_vector_enable(vector);
      }
    }
  }
}

rtems_status_code rtems_interrupt_server_handler_install(
  uint32_t server_index,
  rtems_vector_number vector,
  const char *info,
  rtems_option options,
  rtems_interrupt_handler handler,
  void *arg
)
{
  rtems_status_code sc;
  bsp_interrupt_server_context *s;

  s = bsp_interrupt_server_get_context(server_index, &sc);
  if (s == NULL) {
    return sc;
  }

  return bsp_interrupt_server_call_helper(
    s,
    vector,
    options,
    handler,
    arg,
    bsp_interrupt_server_install_helper
  );
}

rtems_status_code rtems_interrupt_server_handler_remove(
  uint32_t server_index,
  rtems_vector_number vector,
  rtems_interrupt_handler handler,
  void *arg
)
{
  rtems_status_code sc;
  bsp_interrupt_server_context *s;

  s = bsp_interrupt_server_get_context(server_index, &sc);
  if (s == NULL) {
    return sc;
  }

  return bsp_interrupt_server_call_helper(
    s,
    vector,
    0,
    handler,
    arg,
    bsp_interrupt_server_remove_helper
  );
}

typedef struct {
  rtems_interrupt_per_handler_routine routine;
  void *arg;
} bsp_interrupt_server_handler_iterate_helper_data;

static void bsp_interrupt_server_handler_iterate_helper(void *arg)
{
  bsp_interrupt_server_helper_data *hd = arg;
  bsp_interrupt_server_handler_iterate_helper_data *hihd = hd->arg;
  rtems_status_code sc;
  rtems_interrupt_server_entry *e;
  rtems_option trigger_options;

  bsp_interrupt_lock();

  e = bsp_interrupt_server_query_entry(hd->vector, &trigger_options);
  if (e != NULL) {
    rtems_interrupt_server_action **link = &e->actions;
    rtems_interrupt_server_action *c;

    while ((c = *link) != NULL) {
      (*hihd->routine)(hihd->arg, NULL, trigger_options, c->handler, c->arg);
      link = &c->next;
    }

    sc = RTEMS_SUCCESSFUL;
  } else {
    sc = RTEMS_UNSATISFIED;
  }

  bsp_interrupt_unlock();

  hd->sc = sc;
  rtems_event_transient_send(hd->task);
}

rtems_status_code rtems_interrupt_server_handler_iterate(
  uint32_t server_index,
  rtems_vector_number vector,
  rtems_interrupt_per_handler_routine routine,
  void *arg
)
{
  rtems_status_code sc;
  bsp_interrupt_server_handler_iterate_helper_data hihd;
  bsp_interrupt_server_context *s;

  s = bsp_interrupt_server_get_context(server_index, &sc);
  if (s == NULL) {
    return sc;
  }

  if (!bsp_interrupt_is_valid_vector(vector)) {
    return RTEMS_INVALID_ID;
  }

  hihd.routine = routine;
  hihd.arg = arg;
  return bsp_interrupt_server_call_helper(
    s,
    vector,
    0,
    NULL,
    &hihd,
    bsp_interrupt_server_handler_iterate_helper
  );
}

rtems_status_code rtems_interrupt_server_initialize(
  rtems_task_priority priority,
  size_t stack_size,
  rtems_mode modes,
  rtems_attribute attributes,
  uint32_t *server_count
)
{
  uint32_t cpu_index;
  uint32_t cpu_count;
  uint32_t dummy;
  bsp_interrupt_server_context *instances;

  if (server_count == NULL) {
    server_count = &dummy;
  }

  cpu_count = rtems_scheduler_get_processor_maximum();

#if defined(RTEMS_SMP)
  instances = calloc(cpu_count, sizeof(*instances));
  if (instances == NULL) {
    return RTEMS_NO_MEMORY;
  }
#else
  instances = &bsp_interrupt_server_instance;
#endif

  for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
    bsp_interrupt_server_context *s = &instances[cpu_index];
    rtems_status_code sc;
#if defined(RTEMS_SMP)
    rtems_id scheduler;
    cpu_set_t cpu;
#endif

    rtems_interrupt_lock_initialize(&s->lock, "Interrupt Server");
    rtems_chain_initialize_empty(&s->entries);

    sc = rtems_task_create(
      rtems_build_name('I', 'R', 'Q', 'S'),
      priority,
      stack_size,
      modes,
      attributes,
      &s->server
    );
    if (sc != RTEMS_SUCCESSFUL) {
      *server_count = cpu_index;

#if defined(RTEMS_SMP)
      if (cpu_index > 0) {
        return RTEMS_SUCCESSFUL;
      }

      free(instances);
#endif

      return RTEMS_TOO_MANY;
    }

#if defined(RTEMS_SMP)
    sc = rtems_scheduler_ident_by_processor(cpu_index, &scheduler);
    if (sc != RTEMS_SUCCESSFUL) {
      /* Do not start an interrupt server on a processor without a scheduler */
      continue;
    }

    sc = rtems_task_set_scheduler(s->server, scheduler, priority);
    _Assert(sc == RTEMS_SUCCESSFUL);

    /* Set the task to processor affinity on a best-effort basis */
    CPU_ZERO(&cpu);
    CPU_SET(cpu_index, &cpu);
    (void) rtems_task_set_affinity(s->server, sizeof(cpu), &cpu);
#endif

    sc = rtems_task_start(
      s->server,
      bsp_interrupt_server_task,
      (rtems_task_argument) s
    );
    _Assert(sc == RTEMS_SUCCESSFUL);
  }

#if defined(RTEMS_SMP)
  bsp_interrupt_server_instances = instances;
#endif
  *server_count = cpu_index;

  return RTEMS_SUCCESSFUL;
}

static void bsp_interrupt_server_entry_initialize(
  rtems_interrupt_server_entry *entry,
  bsp_interrupt_server_context *s
)
{
  rtems_chain_set_off_chain(&entry->node);
  entry->server = s;
  entry->vector = BSP_INTERRUPT_SERVER_MANAGEMENT_VECTOR;
  entry->actions = NULL;
}

static void bsp_interrupt_server_action_prepend(
  rtems_interrupt_server_entry  *entry,
  rtems_interrupt_server_action *action,
  rtems_interrupt_handler        handler,
  void                          *arg
)
{
  action->handler = handler;
  action->arg = arg;
  action->next = entry->actions;
  entry->actions = action;
}

rtems_status_code rtems_interrupt_server_entry_initialize(
  uint32_t                      server_index,
  rtems_interrupt_server_entry *entry
)
{
  rtems_status_code sc;
  bsp_interrupt_server_context *s;

  s = bsp_interrupt_server_get_context(server_index, &sc);
  if (s == NULL) {
    return sc;
  }

  bsp_interrupt_server_entry_initialize(entry, s);
  return RTEMS_SUCCESSFUL;
}

void rtems_interrupt_server_action_prepend(
  rtems_interrupt_server_entry  *entry,
  rtems_interrupt_server_action *action,
  rtems_interrupt_handler        handler,
  void                          *arg
)
{
  bsp_interrupt_server_action_prepend(entry, action, handler, arg);
}

void rtems_interrupt_server_entry_submit(
  rtems_interrupt_server_entry *entry
)
{
  bsp_interrupt_server_trigger(entry);
}

rtems_status_code rtems_interrupt_server_entry_move(
  rtems_interrupt_server_entry *entry,
  uint32_t                      destination_server_index
)
{
  rtems_status_code sc;
  bsp_interrupt_server_context *s;

  s = bsp_interrupt_server_get_context(destination_server_index, &sc);
  if (s == NULL) {
    return sc;
  }

  entry->server = s;
  return RTEMS_SUCCESSFUL;
}

static void bsp_interrupt_server_entry_synchronize_helper(void *arg)
{
  bsp_interrupt_server_helper_data *hd = arg;

  rtems_event_transient_send(hd->task);
}

void rtems_interrupt_server_entry_destroy(
  rtems_interrupt_server_entry *entry
)
{
  bsp_interrupt_server_context *s;
  rtems_interrupt_lock_context lock_context;

  s = entry->server;
  rtems_interrupt_lock_acquire(&s->lock, &lock_context);

  if (!rtems_chain_is_node_off_chain(&entry->node)) {
    rtems_chain_extract_unprotected(&entry->node);
    rtems_chain_set_off_chain(&entry->node);
  }

  rtems_interrupt_lock_release(&s->lock, &lock_context);

  bsp_interrupt_server_call_helper(
    s,
    BSP_INTERRUPT_SERVER_MANAGEMENT_VECTOR,
    0,
    NULL,
    NULL,
    bsp_interrupt_server_entry_synchronize_helper
  );
}

rtems_status_code rtems_interrupt_server_request_initialize(
  uint32_t                        server_index,
  rtems_interrupt_server_request *request,
  rtems_interrupt_handler         handler,
  void                           *arg
)
{
  rtems_status_code sc;
  bsp_interrupt_server_context *s;

  s = bsp_interrupt_server_get_context(server_index, &sc);
  if (s == NULL) {
    return sc;
  }

  bsp_interrupt_server_entry_initialize(&request->entry, s);
  bsp_interrupt_server_action_prepend(
    &request->entry,
    &request->action,
    handler,
    arg
  );
  return RTEMS_SUCCESSFUL;
}

static void bsp_interrupt_server_handler_move_helper(void *arg)
{
  bsp_interrupt_server_helper_data *hd = arg;
  bsp_interrupt_server_handler_iterate_helper_data *hihd = hd->arg;
  rtems_interrupt_server_entry *e;
  rtems_option trigger_options;

  bsp_interrupt_lock();

  e = bsp_interrupt_server_query_entry(hd->vector, &trigger_options);
  if (e != NULL) {
    rtems_interrupt_lock_context lock_context;
    bsp_interrupt_server_context *src = e->server;
    bsp_interrupt_server_context *dst = hihd->arg;
    bool pending;

    /* The source server is only used in SMP configurations for the lock */
    (void) src;

    rtems_interrupt_lock_acquire(&src->lock, &lock_context);

    pending = !rtems_chain_is_node_off_chain(&e->node);
    if (pending) {
      rtems_chain_extract_unprotected(&e->node);
      rtems_chain_set_off_chain(&e->node);
    }

    rtems_interrupt_lock_release(&src->lock, &lock_context);

    e->server = dst;

    if (pending) {
      bsp_interrupt_server_trigger(e);
    }
  }

  bsp_interrupt_unlock();

  rtems_event_transient_send(hd->task);
}

rtems_status_code rtems_interrupt_server_move(
  uint32_t source_server_index,
  rtems_vector_number vector,
  uint32_t destination_server_index
)
{
  rtems_status_code sc;
  bsp_interrupt_server_context *src;
  bsp_interrupt_server_context *dst;
  bsp_interrupt_server_handler_iterate_helper_data hihd;

  src = bsp_interrupt_server_get_context(source_server_index, &sc);
  if (src == NULL) {
    return sc;
  }

  dst = bsp_interrupt_server_get_context(destination_server_index, &sc);
  if (dst == NULL) {
    return sc;
  }

  if (!bsp_interrupt_is_valid_vector(vector)) {
    return RTEMS_INVALID_ID;
  }

  hihd.arg = dst;
  bsp_interrupt_server_call_helper(
    src,
    vector,
    0,
    NULL,
    &hihd,
    bsp_interrupt_server_handler_move_helper
  );
  return RTEMS_SUCCESSFUL;
}

static void bsp_interrupt_server_entry_suspend_helper(void *arg)
{
  bsp_interrupt_server_helper_data *hd = arg;
  rtems_event_set events;

  rtems_event_transient_send(hd->task);
  rtems_event_system_receive(
    RTEMS_EVENT_SYSTEM_SERVER_RESUME,
    RTEMS_WAIT,
    RTEMS_NO_TIMEOUT,
    &events
  );
}

rtems_status_code rtems_interrupt_server_suspend(uint32_t server_index)
{
  rtems_status_code sc;
  bsp_interrupt_server_context *s;

  s = bsp_interrupt_server_get_context(server_index, &sc);
  if (s == NULL) {
    return sc;
  }

  bsp_interrupt_server_call_helper(
    s,
    BSP_INTERRUPT_SERVER_MANAGEMENT_VECTOR,
    0,
    NULL,
    NULL,
    bsp_interrupt_server_entry_suspend_helper
  );
  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_interrupt_server_resume(uint32_t server_index)
{
  rtems_status_code sc;
  bsp_interrupt_server_context *s;

  s = bsp_interrupt_server_get_context(server_index, &sc);
  if (s == NULL) {
    return sc;
  }

  rtems_event_system_send(s->server, RTEMS_EVENT_SYSTEM_SERVER_RESUME);
  bsp_interrupt_server_call_helper(
    s,
    BSP_INTERRUPT_SERVER_MANAGEMENT_VECTOR,
    0,
    NULL,
    NULL,
    bsp_interrupt_server_entry_synchronize_helper
  );
  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_interrupt_server_set_affinity(
  uint32_t            server_index,
  size_t              affinity_size,
  const cpu_set_t    *affinity,
  rtems_task_priority priority
)
{
  rtems_status_code sc;
  bsp_interrupt_server_context *s;
  rtems_id scheduler;

  s = bsp_interrupt_server_get_context(server_index, &sc);
  if (s == NULL) {
    return sc;
  }

  sc = rtems_scheduler_ident_by_processor_set(
    affinity_size,
    affinity,
    &scheduler
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  sc = rtems_task_set_scheduler(s->server, scheduler, priority);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  return rtems_task_set_affinity(s->server, affinity_size, affinity);
}
