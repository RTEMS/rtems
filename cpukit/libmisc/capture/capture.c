/*
  ------------------------------------------------------------------------

  Copyright Objective Design Systems Pty Ltd, 2002
  All rights reserved Objective Design Systems Pty Ltd, 2002
  Chris Johns (ccj@acm.org)

  COPYRIGHT (c) 1989-2009.
  On-Line Applications Research Corporation (OAR).

  The license and distribution terms for this file may be
  found in the file LICENSE in this distribution.

  This software with is provided ``as is'' and with NO WARRANTY.

  ------------------------------------------------------------------------

  RTEMS Performance Monitoring and Measurement Framework.

  This is the Capture Engine component.

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include "capture.h"
#include <rtems/score/states.inl>
#include <rtems/score/wkspace.h>
#include <rtems/score/wkspace.inl>

/*
 * These events are always recorded and are not part of the
 * watch filters.
 *
 * This feature has been disabled as it becomes confusing when
 * setting up filters and some event leak.
 */
#if defined (RTEMS_CAPTURE_ENGINE_ALLOW_RELATED_EVENTS)
#define RTEMS_CAPTURE_RECORD_EVENTS  (RTEMS_CAPTURE_CREATED_BY_EVENT | \
                                      RTEMS_CAPTURE_CREATED_EVENT | \
                                      RTEMS_CAPTURE_STARTED_BY_EVENT | \
                                      RTEMS_CAPTURE_STARTED_EVENT | \
                                      RTEMS_CAPTURE_RESTARTED_BY_EVENT | \
                                      RTEMS_CAPTURE_RESTARTED_EVENT | \
                                      RTEMS_CAPTURE_DELETED_BY_EVENT | \
                                      RTEMS_CAPTURE_DELETED_EVENT | \
                                      RTEMS_CAPTURE_BEGIN_EVENT | \
                                      RTEMS_CAPTURE_EXITTED_EVENT)
#else
#define RTEMS_CAPTURE_RECORD_EVENTS  (0)
#endif

/*
 * Global capture flags.
 */
#define RTEMS_CAPTURE_ON             (1U << 0)
#define RTEMS_CAPTURE_NO_MEMORY      (1U << 1)
#define RTEMS_CAPTURE_OVERFLOW       (1U << 2)
#define RTEMS_CAPTURE_TRIGGERED      (1U << 3)
#define RTEMS_CAPTURE_READER_ACTIVE  (1U << 4)
#define RTEMS_CAPTURE_READER_WAITING (1U << 5)
#define RTEMS_CAPTURE_GLOBAL_WATCH   (1U << 6)
#define RTEMS_CAPTURE_ONLY_MONITOR   (1U << 7)

/*
 * RTEMS Capture Data.
 */
static rtems_capture_record_t*  capture_records;
static uint32_t                 capture_size;
static uint32_t                 capture_count;
static rtems_capture_record_t*  capture_in;
static uint32_t                 capture_out;
static uint32_t                 capture_flags;
static rtems_capture_task_t*    capture_tasks;
static rtems_capture_control_t* capture_controls;
static int                      capture_extension_index;
static rtems_id                 capture_id;
static rtems_capture_timestamp  capture_timestamp;
static rtems_task_priority      capture_ceiling;
static rtems_task_priority      capture_floor;
static uint32_t                 capture_tick_period;
static rtems_id                 capture_reader;

/*
 * RTEMS Event text.
 */
static const char* capture_event_text[] =
{
  "CREATED_BY",
  "CREATED",
  "STARTED_BY",
  "STARTED",
  "RESTARTED_BY",
  "RESTARTED",
  "DELETED_BY",
  "DELETED",
  "BEGIN",
  "EXITTED",
  "SWITCHED_OUT",
  "SWITCHED_IN",
  "TIMESTAMP"
};

/*
 * rtems_capture_get_time
 *
 *  DESCRIPTION:
 *
 * This function returns the current time. If a handler is provided
 * by the user get the time from that.
 */
static inline void rtems_capture_get_time (uint32_t* ticks,
                                           uint32_t* tick_offset)
{
  if (capture_timestamp)
    capture_timestamp (ticks, tick_offset);
  else
  {
    *ticks       = _Watchdog_Ticks_since_boot;
    *tick_offset = 0;
  }
}

/*
 * rtems_capture_match_names
 *
 *  DESCRIPTION:
 *
 * This function compares rtems_names. It protects the
 * capture engine from a change to the way names are supported
 * in RTEMS.
 *
 */
static inline bool
rtems_capture_match_names (rtems_name lhs, rtems_name rhs)
{
  return lhs == rhs;
}

/*
 * rtems_capture_match_id
 *
 *  DESCRIPTION:
 *
 * This function compares rtems_ids. It protects the
 * capture engine from a change to the way id are supported
 * in RTEMS.
 *
 */
static inline bool
rtems_capture_match_ids (rtems_id lhs, rtems_id rhs)
{
  return lhs == rhs;
}

/*
 * rtems_capture_match_name_id
 *
 *  DESCRIPTION:
 *
 * This function matches a name and/or id.
 */
static inline bool
rtems_capture_match_name_id (rtems_name lhs_name,
                             rtems_id   lhs_id,
                             rtems_name rhs_name,
                             rtems_id   rhs_id)
{
  /*
   * The left hand side name or id could be 0 which means a wildcard.
   */
  if ((lhs_name == 0) && (lhs_id == rhs_id))
    return 1;
  else if ((lhs_id == 0) || (lhs_id == rhs_id))
  {
    if (rtems_capture_match_names (lhs_name, rhs_name))
      return 1;
  }
  return 0;
}

/*
 * rtems_capture_dup_name
 *
 *  DESCRIPTION:
 *
 * This function duplicates an rtems_names. It protects the
 * capture engine from a change to the way names are supported
 * in RTEMS.
 *
 */
static inline void
rtems_capture_dup_name (rtems_name* dst, rtems_name src)
{
  *dst = src;
}

/*
 * rtems_capture_by_in_to
 *
 *  DESCRIPTION:
 *
 * This function sees if a BY control is in the BY names. The use
 * of the valid_mask in this way assumes the number of trigger
 * tasks is the number of bits in uint32_t.
 *
 */
static inline bool
rtems_capture_by_in_to (uint32_t                 events,
                        rtems_capture_task_t*    by,
                        rtems_capture_control_t* to)
{
  uint32_t valid_mask = RTEMS_CAPTURE_CONTROL_FROM_MASK (0);
  uint32_t valid_remainder = 0xffffffff;
  int      i;

  for (i = 0; i < RTEMS_CAPTURE_TRIGGER_TASKS; i++)
  {
    /*
     * If there are no more valid BY entries then
     * we are finished.
     */
    if ((valid_remainder & to->by_valid) == 0)
      break;

    /*
     * Is the froby entry valid and does its name or id match.
     */
    if ((valid_mask & to->by_valid) &&
        (to->by[i].trigger & events))
    {
      /*
       * We have the BY task on the right hand side so we
       * match with id's first then labels if the id's are
       * not set.
       */
      if (rtems_capture_match_name_id (to->by[i].name, to->by[i].id,
                                       by->name, by->id))
        return 1;
    }

    valid_mask >>= 1;
    valid_remainder >>= 1;
  }

  return 0;
}

/*
 * rtems_capture_refcount_up
 *
 *  DESCRIPTION:
 *
 * This function raises the reference count.
 *
 */
static inline void
rtems_capture_refcount_up (rtems_capture_task_t* task)
{
  task->refcount++;
}

/*
 * rtems_capture_refcount_down
 *
 *  DESCRIPTION:
 *
 * This function lowers the reference count and if the count
 * reaches 0 the task control block is returned to the heap.
 *
 */
static inline void
rtems_capture_refcount_down (rtems_capture_task_t* task)
{
  if (task->refcount)
    task->refcount--;
}

/*
 * rtems_capture_init_stack_usage
 *
 *  DESCRIPTION:
 *
 * This function setups a stack so its usage can be monitored.
 */
static inline void
rtems_capture_init_stack_usage (rtems_capture_task_t* task)
{
  if (task->tcb)
  {
    uint32_t* s;
    uint32_t  i;

    task->stack_size  = task->tcb->Start.Initial_stack.size;
    task->stack_clean = task->stack_size;

    s = task->tcb->Start.Initial_stack.area;

    for (i = 0; i < (task->stack_size - 128); i += 4)
      *(s++) = 0xdeaddead;
  }
}

/*
 * rtems_capture_find_control
 *
 *  DESCRIPTION:
 *
 * This function searches for a trigger given a name.
 *
 */
static inline rtems_capture_control_t*
rtems_capture_find_control (rtems_name name, rtems_id id)
{
  rtems_capture_control_t* control;

  for (control = capture_controls; control != NULL; control = control->next)
    if (rtems_capture_match_name_id (name, id, control->name, control->id))
      break;
  return control;
}

/*
 * rtems_capture_create_control
 *
 *  DESCRIPTION:
 *
 * This function creates a capture control for the capture engine.
 *
 */
static inline rtems_capture_control_t*
rtems_capture_create_control (rtems_name name, rtems_id id)
{
  rtems_interrupt_level    level;
  rtems_capture_control_t* control;
  rtems_capture_task_t*    task;

  if ((name == 0) && (id == 0))
    return NULL;

  control = rtems_capture_find_control (name, id);

  if (control == NULL)
  {
    control = _Workspace_Allocate (sizeof (rtems_capture_control_t));

    if (control == NULL)
    {
      capture_flags |= RTEMS_CAPTURE_NO_MEMORY;
      return NULL;
    }

    control->name          = name;
    control->id            = id;
    control->flags         = 0;
    control->to_triggers   = 0;
    control->from_triggers = 0;
    control->by_valid      = 0;

    memset (control->by, 0, sizeof (control->by));

    rtems_interrupt_disable (level);

    control->next    = capture_controls;
    capture_controls = control;

    /*
     * We need to scan the task list as set the control to the
     * tasks.
     */
    for (task = capture_tasks; task != NULL; task = task->forw)
      if (rtems_capture_match_name_id (name, id, task->name, task->id))
        task->control = control;

    rtems_interrupt_enable (level);
  }

  return control;
}

/*
 * rtems_capture_create_capture_task
 *
 *  DESCRIPTION:
 *
 * This function create the task control.
 *
 */
static inline rtems_capture_task_t*
rtems_capture_create_capture_task (rtems_tcb* new_task)
{
  rtems_interrupt_level    level;
  rtems_capture_task_t*    task;
  rtems_capture_control_t* control;
  rtems_name               name;

  task = _Workspace_Allocate (sizeof (rtems_capture_task_t));

  if (task == NULL)
  {
    capture_flags |= RTEMS_CAPTURE_NO_MEMORY;
    return NULL;
  }

  /*
   * Check the type of name the object has.
   */

  rtems_object_get_classic_name( new_task->Object.id, &name );

  rtems_capture_dup_name (&task->name, name);

  task->id               = new_task->Object.id;
  task->flags            = 0;
  task->in               = 0;
  task->refcount         = 0;
  task->out              = 0;
  task->tcb              = new_task;
  task->ticks            = 0;
  task->tick_offset      = 0;
  task->ticks_in         = 0;
  task->tick_offset_in   = 0;
  task->control          = 0;
  task->last_ticks       = 0;
  task->last_tick_offset = 0;

  task->tcb->extensions[capture_extension_index] = task;

  task->start_priority = new_task->Start.initial_priority;
  task->stack_size     = new_task->Start.Initial_stack.size;
  task->stack_clean    = task->stack_size;

  rtems_interrupt_disable (level);

  task->forw    = capture_tasks;
  if (task->forw)
    task->forw->back = task;
  task->back    = NULL;
  capture_tasks = task;

  rtems_interrupt_enable (level);

  /*
   * We need to scan the default control list to initialise
   * this control.
   */

  for (control = capture_controls; control != NULL; control = control->next)
    if (rtems_capture_match_name_id (control->name, control->id,
                                     task->name, task->id))
      task->control = control;

  return task;
}

/*
 * rtems_capture_destroy_capture_task
 *
 *  DESCRIPTION:
 *
 * This function destroy the task structure if the reference count
 * is 0 and the tcb has been cleared signalling the task has been
 * deleted.
 *
 */
static inline void
rtems_capture_destroy_capture_task (rtems_capture_task_t* task)
{
  if (task)
  {
    rtems_interrupt_level level;

    rtems_interrupt_disable (level);

    if (task->tcb || task->refcount)
      task = 0;

    if (task)
    {
      if (task->forw)
        task->forw->back = task->back;
      if (task->back)
        task->back->forw = task->forw;
      else
        capture_tasks = task->forw;
    }

    rtems_interrupt_enable (level);

    _Workspace_Free (task);
  }
}

/*
 * rtems_capture_record
 *
 *  DESCRIPTION:
 *
 * This function records a capture record into the capture buffer.
 *
 */
static inline void
rtems_capture_record (rtems_capture_task_t* task,
                      uint32_t              events)
{
  /*
   * Check the watch state if we have a task control, and
   * the task's real priority is lower or equal to the ceiling.
   */
  if (task &&
      ((capture_flags &
        (RTEMS_CAPTURE_TRIGGERED | RTEMS_CAPTURE_ONLY_MONITOR)) ==
       RTEMS_CAPTURE_TRIGGERED))
  {
    rtems_capture_control_t* control;

    control = task->control;

    /*
     * Capure the record if we have an event that is always
     * captured, or the task's real priority is greater than the
     * watch ceiling, and the global watch or task watch is enabled.
     */

    if ((events & RTEMS_CAPTURE_RECORD_EVENTS) ||
        ((task->tcb->real_priority >= capture_ceiling) &&
         (task->tcb->real_priority <= capture_floor) &&
         ((capture_flags & RTEMS_CAPTURE_GLOBAL_WATCH) ||
          (control && (control->flags & RTEMS_CAPTURE_WATCH)))))
    {
      rtems_interrupt_level level;

      rtems_interrupt_disable (level);

      if (capture_count < capture_size)
      {
        capture_count++;
        capture_in->task   = task;
        capture_in->events = (events |
                              (task->tcb->real_priority) |
                              (task->tcb->current_priority << 8));

        if ((events & RTEMS_CAPTURE_RECORD_EVENTS) == 0)
          task->flags |= RTEMS_CAPTURE_TRACED;

        rtems_capture_get_time (&capture_in->ticks, &capture_in->tick_offset);

        if (capture_in == &capture_records[capture_size - 1])
          capture_in = capture_records;
        else
          capture_in++;

        rtems_capture_refcount_up (task);
      }
      else
        capture_flags |= RTEMS_CAPTURE_OVERFLOW;
      rtems_interrupt_enable (level);
    }
  }
}

/*
 * rtems_capture_trigger
 *
 *  DESCRIPTION:
 *
 * See if we have triggered and if not see if this event is a
 * cause of a trigger.
 */
static bool
rtems_capture_trigger (rtems_capture_task_t* ft,
                       rtems_capture_task_t* tt,
                       uint32_t              events)
{
  /*
   * If we have not triggered then see if this is a trigger condition.
   */
  if (!(capture_flags & RTEMS_CAPTURE_TRIGGERED))
  {
    rtems_capture_control_t* fc = NULL;
    rtems_capture_control_t* tc = NULL;
    uint32_t                 from_events = 0;
    uint32_t                 to_events = 0;
    uint32_t                 from_to_events = 0;

    if (ft)
    {
      fc = ft->control;
      if (fc)
        from_events = fc->from_triggers & events;
    }

    if (tt)
    {
      tc = tt->control;
      if (tc)
      {
        to_events = tc->to_triggers & events;
        if (ft && tc->by_valid)
          from_to_events = tc->by_triggers & events;
      }
    }

    /*
     * Check if we have any from or to events. These are the
     * from any or to any type triggers. All from/to triggers are
     * listed in the to's control with the from in the from list.
     *
     * The masking above means any flag set is a trigger.
     */
    if (from_events || to_events)
    {
      capture_flags |= RTEMS_CAPTURE_TRIGGERED;
      return 1;
    }

    /*
     * Check the from->to events.
     */
    if (from_to_events)
    {
      if (rtems_capture_by_in_to (events, ft, tc))
      {
        capture_flags |= RTEMS_CAPTURE_TRIGGERED;
        return 1;
      }
    }

    return 0;
  }

  return 1;
}

/*
 * rtems_capture_create_task
 *
 *  DESCRIPTION:
 *
 * This function is called when a task is created.
 *
 */
static bool
rtems_capture_create_task (rtems_tcb* current_task,
                           rtems_tcb* new_task)
{
  rtems_capture_task_t* ct;
  rtems_capture_task_t* nt;

  ct = current_task->extensions[capture_extension_index];

  /*
   * The task pointers may not be known as the task may have
   * been created before the capture engine was open. Add them.
   */

  if (ct == NULL)
    ct = rtems_capture_create_capture_task (current_task);

  /*
   * Create the new task's capture control block.
   */
  nt = rtems_capture_create_capture_task (new_task);

  if (rtems_capture_trigger (ct, nt, RTEMS_CAPTURE_CREATE))
  {
    rtems_capture_record (ct, RTEMS_CAPTURE_CREATED_BY_EVENT);
    rtems_capture_record (nt, RTEMS_CAPTURE_CREATED_EVENT);
  }

  return 1 == 1;
}

/*
 * rtems_capture_start_task
 *
 *  DESCRIPTION:
 *
 * This function is called when a task is started.
 *
 */
static void
rtems_capture_start_task (rtems_tcb* current_task,
                          rtems_tcb* started_task)
{
  /*
   * Get the capture task control block so we can trace this
   * event.
   */
  rtems_capture_task_t* ct;
  rtems_capture_task_t* st;

  ct = current_task->extensions[capture_extension_index];
  st = started_task->extensions[capture_extension_index];

  /*
   * The task pointers may not be known as the task may have
   * been created before the capture engine was open. Add them.
   */

  if (ct == NULL)
    ct = rtems_capture_create_capture_task (current_task);

  if (st == NULL)
    st = rtems_capture_create_capture_task (started_task);

  if (rtems_capture_trigger (ct, st, RTEMS_CAPTURE_START))
  {
    rtems_capture_record (ct, RTEMS_CAPTURE_STARTED_BY_EVENT);
    rtems_capture_record (st, RTEMS_CAPTURE_STARTED_EVENT);
  }

  rtems_capture_init_stack_usage (st);
}

/*
 * rtems_capture_restart_task
 *
 *  DESCRIPTION:
 *
 * This function is called when a task is restarted.
 *
 */
static void
rtems_capture_restart_task (rtems_tcb* current_task,
                            rtems_tcb* restarted_task)
{
  /*
   * Get the capture task control block so we can trace this
   * event.
   */
  rtems_capture_task_t* ct;
  rtems_capture_task_t* rt;

  ct = current_task->extensions[capture_extension_index];
  rt = restarted_task->extensions[capture_extension_index];

  /*
   * The task pointers may not be known as the task may have
   * been created before the capture engine was open. Add them.
   */

  if (ct == NULL)
    ct = rtems_capture_create_capture_task (current_task);

  if (rt == NULL)
    rt = rtems_capture_create_capture_task (restarted_task);

  if (rtems_capture_trigger (ct, rt, RTEMS_CAPTURE_RESTART))
  {
    rtems_capture_record (ct, RTEMS_CAPTURE_RESTARTED_BY_EVENT);
    rtems_capture_record (rt, RTEMS_CAPTURE_RESTARTED_EVENT);
  }

  rtems_capture_task_stack_usage (rt);
  rtems_capture_init_stack_usage (rt);
}

/*
 * rtems_capture_delete_task
 *
 *  DESCRIPTION:
 *
 * This function is called when a task is deleted.
 *
 */
static void
rtems_capture_delete_task (rtems_tcb* current_task,
                           rtems_tcb* deleted_task)
{
  /*
   * Get the capture task control block so we can trace this
   * event.
   */
  rtems_capture_task_t* ct;
  rtems_capture_task_t* dt;

  /*
   * The task pointers may not be known as the task may have
   * been created before the capture engine was open. Add them.
   */

  ct = current_task->extensions[capture_extension_index];
  dt = deleted_task->extensions[capture_extension_index];

  if (ct == NULL)
    ct = rtems_capture_create_capture_task (current_task);

  if (dt == NULL)
    dt = rtems_capture_create_capture_task (deleted_task);

  if (rtems_capture_trigger (ct, dt, RTEMS_CAPTURE_DELETE))
  {
    rtems_capture_record (ct, RTEMS_CAPTURE_DELETED_BY_EVENT);
    rtems_capture_record (dt, RTEMS_CAPTURE_DELETED_EVENT);
  }

  rtems_capture_task_stack_usage (dt);

  /*
   * This task's tcb will be invalid. This signals the
   * task has been deleted.
   */
  dt->tcb = 0;

  rtems_capture_destroy_capture_task (dt);
}

/*
 * rtems_capture_begin_task
 *
 *  DESCRIPTION:
 *
 * This function is called when a task is begun.
 *
 */
static void
rtems_capture_begin_task (rtems_tcb* begin_task)
{
  /*
   * Get the capture task control block so we can trace this
   * event.
   */
  rtems_capture_task_t* bt;

  bt = begin_task->extensions[capture_extension_index];

  /*
   * The task pointers may not be known as the task may have
   * been created before the capture engine was open. Add them.
   */

  if (bt == NULL)
    bt = rtems_capture_create_capture_task (begin_task);

  if (rtems_capture_trigger (NULL, bt, RTEMS_CAPTURE_BEGIN))
    rtems_capture_record (bt, RTEMS_CAPTURE_BEGIN_EVENT);
}

/*
 * rtems_capture_exitted_task
 *
 *  DESCRIPTION:
 *
 * This function is called when a task is exitted. That is
 * returned rather than was deleted.
 *
 */
static void
rtems_capture_exitted_task (rtems_tcb* exitted_task)
{
  /*
   * Get the capture task control block so we can trace this
   * event.
   */
  rtems_capture_task_t* et;

  et = exitted_task->extensions[capture_extension_index];

  /*
   * The task pointers may not be known as the task may have
   * been created before the capture engine was open. Add them.
   */

  if (et == NULL)
    et = rtems_capture_create_capture_task (exitted_task);

  if (rtems_capture_trigger (NULL, et, RTEMS_CAPTURE_EXITTED))
    rtems_capture_record (et, RTEMS_CAPTURE_EXITTED_EVENT);

  rtems_capture_task_stack_usage (et);
}

/*
 * rtems_capture_switch_task
 *
 *  DESCRIPTION:
 *
 * This function is called when a context is switched.
 *
 */
static void
rtems_capture_switch_task (rtems_tcb* current_task,
                           rtems_tcb* heir_task)
{
  /*
   * Only perform context switch trace processing if tracing is
   * enabled.
   */
  if (capture_flags & RTEMS_CAPTURE_ON)
  {
    uint32_t ticks;
    uint32_t tick_offset;

    /*
     * Get the cpature task control block so we can update the
     * reference and perform any watch or trigger functions.
     * The task pointers may not be known as the task may have
     * been created before the capture engine was open. Add them.
     */
    rtems_capture_task_t* ct;
    rtems_capture_task_t* ht;


    if (_States_Is_transient (current_task->current_state)
     || _States_Is_dormant (current_task->current_state))
    {
      rtems_id ct_id = current_task->Object.id;

      for (ct = capture_tasks; ct; ct = ct->forw)
        if (ct->id == ct_id)
          break;
    }
    else
    {
      ct = current_task->extensions[capture_extension_index];

      if (ct == NULL)
        ct = rtems_capture_create_capture_task (current_task);
    }

    ht = heir_task->extensions[capture_extension_index];

    if (ht == NULL)
      ht = rtems_capture_create_capture_task (heir_task);

    /*
     * Update the execution time. Assume the tick will not overflow
     * for now. This may need to change.
     */
    rtems_capture_get_time (&ticks, &tick_offset);

    /*
     * We could end up with null pointers for both the current task
     * and the heir task.
     */

    if (ht)
    {
      ht->in++;
      ht->ticks_in       = ticks;
      ht->tick_offset_in = tick_offset;
    }

    if (ct)
    {
      ct->out++;
      ct->ticks += ticks - ct->ticks_in;

      if (capture_timestamp)
      {
        tick_offset += capture_tick_period - ct->tick_offset_in;

        if (tick_offset < capture_tick_period)
          ct->tick_offset = tick_offset;
        else
        {
          ct->ticks++;
          ct->tick_offset = tick_offset - capture_tick_period;
        }
      }
      else
      {
        ct->tick_offset += 100;
      }
    }

    if (rtems_capture_trigger (ct, ht, RTEMS_CAPTURE_SWITCH))
    {
      rtems_capture_record (ct, RTEMS_CAPTURE_SWITCHED_OUT_EVENT);
      rtems_capture_record (ht, RTEMS_CAPTURE_SWITCHED_IN_EVENT);
    }
  }
}

/*
 * rtems_capture_open
 *
 *  DESCRIPTION:
 *
 * This function initialises the realtime capture engine allocating the trace
 * buffer. It is assumed we have a working heap at stage of initialisation.
 *
 */
rtems_status_code
rtems_capture_open (uint32_t   size, rtems_capture_timestamp timestamp __attribute__((unused)))
{
  rtems_extensions_table capture_extensions;
  rtems_name             name;
  rtems_status_code      sc;

  /*
   * See if the capture engine is already open.
   */

  if (capture_records)
    return RTEMS_RESOURCE_IN_USE;

  capture_records = malloc (size * sizeof (rtems_capture_record_t));

  if (capture_records == NULL)
    return RTEMS_NO_MEMORY;

  capture_size    = size;
  capture_count   = 0;
  capture_in      = capture_records;
  capture_out     = 0;
  capture_flags   = 0;
  capture_tasks   = NULL;
  capture_ceiling = 0;
  capture_floor   = 255;

  /*
   * Create the extension table. This is copied so we
   * can create it as a local.
   */
  capture_extensions.thread_create  = rtems_capture_create_task;
  capture_extensions.thread_start   = rtems_capture_start_task;
  capture_extensions.thread_restart = rtems_capture_restart_task;
  capture_extensions.thread_delete  = rtems_capture_delete_task;
  capture_extensions.thread_switch  = rtems_capture_switch_task;
  capture_extensions.thread_begin   = rtems_capture_begin_task;
  capture_extensions.thread_exitted = rtems_capture_exitted_task;
  capture_extensions.fatal          = NULL;

  /*
   * Get the tick period from the BSP Configuration Table.
   */
  capture_tick_period = Configuration.microseconds_per_tick;

  /*
   * Register the user extension handlers for the CAPture Engine.
   */
  name = rtems_build_name ('C', 'A', 'P', 'E');
  sc   = rtems_extension_create (name, &capture_extensions, &capture_id);

  if (sc != RTEMS_SUCCESSFUL)
  {
    capture_id = 0;
    free (capture_records);
    capture_records = NULL;
  }
  else
  {
    capture_extension_index = rtems_object_id_get_index (capture_id);
  }

  /*
   * Iterate over the list of existing tasks.
   */

  return sc;
}

/*
 * rtems_capture_close
 *
 *  DESCRIPTION:
 *
 * This function shutdowns the capture engine and release any claimed
 * resources.
 */
rtems_status_code
rtems_capture_close (void)
{
  rtems_interrupt_level    level;
  rtems_capture_task_t*    task;
  rtems_capture_control_t* control;
  rtems_capture_record_t*  records;
  rtems_status_code        sc;

  rtems_interrupt_disable (level);

  if (!capture_records)
  {
    rtems_interrupt_enable (level);
    return RTEMS_SUCCESSFUL;
  }

  capture_flags &= ~(RTEMS_CAPTURE_ON | RTEMS_CAPTURE_ONLY_MONITOR);

  records = capture_records;
  capture_records = NULL;

  rtems_interrupt_enable (level);

  /*
   * Delete the extension first. This means we are now able to
   * release the resources we have without them being used.
   */

  sc = rtems_extension_delete (capture_id);

  if (sc != RTEMS_SUCCESSFUL)
    return sc;

  task = capture_tasks;

  while (task)
  {
    rtems_capture_task_t* delete = task;
    task = task->forw;
    _Workspace_Free (delete);
  }

  capture_tasks = NULL;

  control = capture_controls;

  while (control)
  {
    rtems_capture_control_t* delete = control;
    control = control->next;
    _Workspace_Free (delete);
  }

  capture_controls = NULL;

  if (capture_records)
  {
    free (capture_records);
    capture_records = NULL;
  }

  return RTEMS_SUCCESSFUL;
}

/*
 * rtems_capture_control
 *
 *  DESCRIPTION:
 *
 * This function allows control of tracing at a global level.
 */
rtems_status_code
rtems_capture_control (bool enable)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable (level);

  if (!capture_records)
  {
    rtems_interrupt_enable (level);
    return RTEMS_UNSATISFIED;
  }

  if (enable)
    capture_flags |= RTEMS_CAPTURE_ON;
  else
    capture_flags &= ~RTEMS_CAPTURE_ON;

  rtems_interrupt_enable (level);

  return RTEMS_SUCCESSFUL;
}

/*
 * rtems_capture_monitor
 *
 *  DESCRIPTION:
 *
 * This function enable the monitor mode. When in the monitor mode
 * the tasks are monitored but no data is saved. This can be used
 * to profile the load on a system.
 */
rtems_status_code
rtems_capture_monitor (bool enable)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable (level);

  if (!capture_records)
  {
    rtems_interrupt_enable (level);
    return RTEMS_UNSATISFIED;
  }

  if (enable)
    capture_flags |= RTEMS_CAPTURE_ONLY_MONITOR;
  else
    capture_flags &= ~RTEMS_CAPTURE_ONLY_MONITOR;

  rtems_interrupt_enable (level);

  return RTEMS_SUCCESSFUL;
}

/*
 * rtems_capture_flush
 *
 *  DESCRIPTION:
 *
 * This function flushes the capture buffer. The prime parameter allows the
 * capture engine to also be primed again.
 */
rtems_status_code
rtems_capture_flush (bool prime)
{
  rtems_interrupt_level level;
  rtems_capture_task_t* task;

  rtems_interrupt_disable (level);

  for (task = capture_tasks; task != NULL; task = task->forw)
  {
    task->flags &= ~RTEMS_CAPTURE_TRACED;
    task->refcount = 0;
  }

  if (prime)
    capture_flags &= ~(RTEMS_CAPTURE_TRIGGERED | RTEMS_CAPTURE_OVERFLOW);
  else
    capture_flags &= ~RTEMS_CAPTURE_OVERFLOW;

  capture_count = 0;
  capture_in    = capture_records;
  capture_out   = 0;

  rtems_interrupt_enable (level);

  task = capture_tasks;

  while (task)
  {
    rtems_capture_task_t* check = task;
    task = task->forw;
    rtems_capture_destroy_capture_task (check);
  }

  return RTEMS_SUCCESSFUL;
}

/*
 * rtems_capture_watch_add
 *
 *  DESCRIPTION:
 *
 * This function defines a watch for a specific task given a name. A watch
 * causes it to be traced either in or out of context. The watch can be
 * optionally enabled or disabled with the set routine. It is disabled by
 * default.
 */
rtems_status_code
rtems_capture_watch_add (rtems_name name, rtems_id id)
{
  rtems_capture_control_t* control;

  if ((name == 0) && (id == 0))
    return RTEMS_UNSATISFIED;

  control = rtems_capture_find_control (name, id);

  if (control && !id)
    return RTEMS_TOO_MANY;

  if (!control)
    control = rtems_capture_create_control (name, id);

  if (!control)
    return RTEMS_NO_MEMORY;

  return RTEMS_SUCCESSFUL;
}

/*
 * rtems_capture_watch_del
 *
 *  DESCRIPTION:
 *
 * This function removes a watch for a specific task given a name. The task
 * description will still exist if referenced by a trace record in the trace
 * buffer or a global watch is defined.
 */
rtems_status_code
rtems_capture_watch_del (rtems_name name, rtems_id id)
{
  rtems_interrupt_level     level;
  rtems_capture_control_t*  control;
  rtems_capture_control_t** prev_control;
  rtems_capture_task_t*     task;
  bool                      found = false;

  /*
   * Should this test be for wildcards ?
   */

  for (prev_control = &capture_controls, control = capture_controls;
       control != NULL; )
  {
    if (rtems_capture_match_name_id (control->name, control->id, name, id))
    {
      rtems_interrupt_disable (level);

      for (task = capture_tasks; task != NULL; task = task->forw)
        if (task->control == control)
          task->control = 0;

      *prev_control = control->next;

      rtems_interrupt_enable (level);

      _Workspace_Free (control);

      control = *prev_control;

      found = true;
    }
    else
    {
      prev_control = &control->next;
      control      = control->next;
    }
  }

  if (found)
    return RTEMS_SUCCESSFUL;

  return RTEMS_INVALID_NAME;
}

/*
 * rtems_capture_watch_set
 *
 *  DESCRIPTION:
 *
 * This function allows control of a watch. The watch can be enabled or
 * disabled.
 */
rtems_status_code
rtems_capture_watch_ctrl (rtems_name name, rtems_id id, bool enable)
{
  rtems_interrupt_level    level;
  rtems_capture_control_t* control;
  bool                     found = false;

  /*
   * Find the control and then set the watch. It must exist before it can
   * be controlled.
   */
  for (control = capture_controls; control != NULL; control = control->next)
  {
    if (rtems_capture_match_name_id (control->name, control->id, name, id))
    {
      rtems_interrupt_disable (level);

      if (enable)
        control->flags |= RTEMS_CAPTURE_WATCH;
      else
        control->flags &= ~RTEMS_CAPTURE_WATCH;

      rtems_interrupt_enable (level);

      found = true;
    }
  }

  if (found)
    return RTEMS_SUCCESSFUL;

  return RTEMS_INVALID_NAME;
}

/*
 * rtems_capture_watch_global
 *
 *  DESCRIPTION:
 *
 * This function allows control of a global watch. The watch can be enabled or
 * disabled. A global watch configures all tasks below the ceiling and above
 * the floor to be traced.
 */
rtems_status_code
rtems_capture_watch_global (bool enable)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable (level);

  /*
   * We need to keep specific and global watches separate so
   * a global enable/disable does not lose a specific watch.
   */
  if (enable)
    capture_flags |= RTEMS_CAPTURE_GLOBAL_WATCH;
  else
    capture_flags &= ~RTEMS_CAPTURE_GLOBAL_WATCH;

  rtems_interrupt_enable (level);

  return RTEMS_SUCCESSFUL;
}

/*
 * rtems_capture_watch_global_on
 *
 *  DESCRIPTION:
 *
 * This function returns the global watch state.
 */
bool
rtems_capture_watch_global_on (void)
{
  return capture_flags & RTEMS_CAPTURE_GLOBAL_WATCH ? 1 : 0;
}

/*
 * rtems_capture_watch_ceiling
 *
 *  DESCRIPTION:
 *
 * This function sets a watch ceiling. Tasks at or greating that the
 * ceiling priority are not watched. This is a simple way to monitor
 * an application and exclude system tasks running at a higher
 * priority level.
 */
rtems_status_code
rtems_capture_watch_ceiling (rtems_task_priority ceiling)
{
  capture_ceiling = ceiling;
  return RTEMS_SUCCESSFUL;
}

/*
 * rtems_capture_watch_get_ceiling
 *
 *  DESCRIPTION:
 *
 * This function gets the watch ceiling.
 */
rtems_task_priority
rtems_capture_watch_get_ceiling (void)
{
  return capture_ceiling;
}

/*
 * rtems_capture_watch_floor
 *
 *  DESCRIPTION:
 *
 * This function sets a watch floor. Tasks at or less that the
 * floor priority are not watched. This is a simple way to monitor
 * an application and exclude system tasks running at a lower
 * priority level.
 */
rtems_status_code
rtems_capture_watch_floor (rtems_task_priority floor)
{
  capture_floor = floor;
  return RTEMS_SUCCESSFUL;
}

/*
 * rtems_capture_watch_get_floor
 *
 *  DESCRIPTION:
 *
 * This function gets the watch floor.
 */
rtems_task_priority
rtems_capture_watch_get_floor (void)
{
  return capture_floor;
}

/*
 * rtems_capture_map_trigger
 *
 *  DESCRIPTION:
 *
 * Map the trigger to a bit mask.
 *
 */
static uint32_t
rtems_capture_map_trigger (rtems_capture_trigger_t trigger)
{
  /*
   * Transform the mode and trigger to a bit map.
   */
  switch (trigger)
  {
    case rtems_capture_switch:
      return RTEMS_CAPTURE_SWITCH;
    case rtems_capture_create:
      return RTEMS_CAPTURE_CREATE;
    case rtems_capture_start:
      return RTEMS_CAPTURE_START;
    case rtems_capture_restart:
      return RTEMS_CAPTURE_RESTART;
    case rtems_capture_delete:
      return RTEMS_CAPTURE_DELETE;
    case rtems_capture_begin:
      return RTEMS_CAPTURE_BEGIN;
    case rtems_capture_exitted:
      return RTEMS_CAPTURE_EXITTED;
    default:
      break;
  }
  return 0;
}

/*
 * rtems_capture_set_trigger
 *
 *  DESCRIPTION:
 *
 * This function sets a trigger.
 *
 * This set trigger routine will create a capture control for the
 * target task. The task list is searched and any existing tasks
 * are linked to the new control.
 *
 * We can have a number of tasks that have the same name so we
 * search using names. This means a number of tasks can be
 * linked to single control.
 */
rtems_status_code
rtems_capture_set_trigger (rtems_name                   from_name,
                           rtems_id                     from_id,
                           rtems_name                   to_name,
                           rtems_id                     to_id,
                           rtems_capture_trigger_mode_t mode,
                           rtems_capture_trigger_t      trigger)
{
  rtems_capture_control_t* control;
  uint32_t                 flags;

  flags = rtems_capture_map_trigger (trigger);

  /*
   * The mode sets the opposite type of trigger. For example
   * FROM ANY means trigger when the event happens TO this
   * task. TO ANY means FROM this task.
   */

  if (mode == rtems_capture_to_any)
  {
    control = rtems_capture_create_control (from_name, from_id);
    if (control == NULL)
      return RTEMS_NO_MEMORY;
    control->from_triggers |= flags & RTEMS_CAPTURE_FROM_TRIGS;
  }
  else
  {
    control = rtems_capture_create_control (to_name, to_id);
    if (control == NULL)
      return RTEMS_NO_MEMORY;
    if (mode == rtems_capture_from_any)
      control->to_triggers |= flags;
    else
    {
      bool done = false;
      int  i;

      control->by_triggers |= flags;

      for (i = 0; i < RTEMS_CAPTURE_TRIGGER_TASKS; i++)
      {
        if (rtems_capture_control_by_valid (control, i) &&
            ((control->by[i].name == from_name) ||
             (from_id && (control->by[i].id == from_id))))
        {
          control->by[i].trigger |= flags;
          done = true;
          break;
        }
      }

      if (!done)
      {
        for (i = 0; i < RTEMS_CAPTURE_TRIGGER_TASKS; i++)
        {
          if (!rtems_capture_control_by_valid (control, i))
          {
            control->by_valid |= RTEMS_CAPTURE_CONTROL_FROM_MASK (i);
            control->by[i].name = from_name;
            control->by[i].id = from_id;
            control->by[i].trigger = flags;
            done = true;
            break;
          }
        }
      }

      if (!done)
        return RTEMS_TOO_MANY;
    }
  }
  return RTEMS_SUCCESSFUL;
}

/*
 * rtems_capture_clear_trigger
 *
 *  DESCRIPTION:
 *
 * This function clear a trigger.
 */
rtems_status_code
rtems_capture_clear_trigger (rtems_name                   from_name,
                             rtems_id                     from_id,
                             rtems_name                   to_name,
                             rtems_id                     to_id,
                             rtems_capture_trigger_mode_t mode,
                             rtems_capture_trigger_t      trigger)
{
  rtems_capture_control_t* control;
  uint32_t                 flags;

  flags = rtems_capture_map_trigger (trigger);

  if (mode == rtems_capture_to_any)
  {
    control = rtems_capture_find_control (from_name, from_id);
    if (control == NULL)
    {
      if (from_id)
        return RTEMS_INVALID_ID;
      return RTEMS_INVALID_NAME;
    }
    control->from_triggers &= ~flags;
  }
  else
  {
    control = rtems_capture_find_control (to_name, to_id);
    if (control == NULL)
    {
      if (to_id)
        return RTEMS_INVALID_ID;
      return RTEMS_INVALID_NAME;
    }
    if (mode == rtems_capture_from_any)
      control->to_triggers &= ~flags;
    else
    {
      bool done = false;
      int  i;

      control->by_triggers &= ~flags;

      for (i = 0; i < RTEMS_CAPTURE_TRIGGER_TASKS; i++)
      {
        if (rtems_capture_control_by_valid (control, i) &&
            ((control->by[i].name == from_name) ||
             (control->by[i].id == from_id)))
        {
          control->by[i].trigger &= ~trigger;
          if (control->by[i].trigger == 0)
            control->by_valid &= ~RTEMS_CAPTURE_CONTROL_FROM_MASK (i);
          done = true;
          break;
        }
      }

      if (!done)
      {
        if (from_id)
          return RTEMS_INVALID_ID;
        return RTEMS_INVALID_NAME;
      }
    }
  }
  return RTEMS_SUCCESSFUL;
}

/*
 * rtems_capture_read
 *
 *  DESCRIPTION:
 *
 * This function reads a number of records from the capture buffer.
 * The user can optionally block and wait until the buffer as a
 * specific number of records available or a specific time has
 * elasped.
 *
 * The function returns the number of record that is has that are
 * in a continous block of memory. If the number of available records
 * wrap then only those records are provided. This removes the need for
 * caller to be concerned about buffer wrappings. If the number of
 * requested records cannot be met due to the wrapping of the records
 * less than the specified number will be returned.
 *
 * The user must release the records. This is achieved with a call to
 * rtems_capture_release. Calls this function without a release will
 * result in at least the same number of records being released.
 *
 * The 'threshold' parameter is the number of records that must be
 * captured before returning. If a timeout period is specified (non-0)
 * any captured records will be returned. These parameters stop
 * thrashing occuring for a small number of records, yet allows
 * a user configured latiency to be applied for single events.
 *
 * The 'timeout' parameter is in micro-seconds. A value of 0 will disable
 * the timeout.
 *
 */
rtems_status_code
rtems_capture_read (uint32_t                 threshold,
                    uint32_t                 timeout,
                    uint32_t*                read,
                    rtems_capture_record_t** recs)
{
  rtems_interrupt_level level;
  rtems_status_code     sc = RTEMS_SUCCESSFUL;
  uint32_t              count;

  *read = 0;
  *recs = NULL;

  rtems_interrupt_disable (level);

  /*
   * Only one reader is allowed.
   */

  if (capture_flags & RTEMS_CAPTURE_READER_ACTIVE)
  {
    rtems_interrupt_enable (level);
    return RTEMS_RESOURCE_IN_USE;
  }

  capture_flags |= RTEMS_CAPTURE_READER_ACTIVE;
  *read = count = capture_count;

  rtems_interrupt_enable (level);

  *recs = &capture_records[capture_out];

  for (;;)
  {
    /*
     * See if the count wraps the end of the record buffer.
     */
    if (count && ((capture_out + count) >= capture_size))
      *read = capture_size - capture_out;

    /*
     * Do we have a threshold and the current count has not wrapped
     * around the end of the capture record buffer ?
     */
    if ((*read == count) && threshold)
    {
      /*
       * Do we have enough records ?
       */
      if (*read < threshold)
      {
        rtems_event_set event_out;

        rtems_task_ident (RTEMS_SELF, RTEMS_LOCAL, &capture_reader);

        rtems_interrupt_disable (level);

        capture_flags |= RTEMS_CAPTURE_READER_WAITING;

        rtems_interrupt_enable (level);

        sc = rtems_event_receive (RTEMS_EVENT_0,
                                  RTEMS_WAIT | RTEMS_EVENT_ANY,
                                  RTEMS_MICROSECONDS_TO_TICKS (timeout),
                                  &event_out);

        /*
         * Let the user handle all other sorts of errors. This may
         * not be the best solution, but oh well, it will do for
         * now.
         */
        if ((sc != RTEMS_SUCCESSFUL) && (sc != RTEMS_TIMEOUT))
          break;

        rtems_interrupt_disable (level);

        *read = count = capture_count;

        rtems_interrupt_enable (level);

        continue;
      }
    }

    /*
     * Always out if we reach here. To loop use continue.
     */
    break;
  }

  return sc;
}

/*
 * rtems_capture_release
 *
 *  DESCRIPTION:
 *
 * This function releases the requested number of record slots back
 * to the capture engine. The count must match the number read.
 */
rtems_status_code
rtems_capture_release (uint32_t count)
{
  rtems_capture_record_t* rec;
  uint32_t                counted;

  rtems_interrupt_level level;

  rtems_interrupt_disable (level);

  if (count > capture_count)
    count = capture_count;

  rtems_interrupt_enable (level);

  counted = count;

  rec = &capture_records[capture_out];

  while (counted--)
  {
    rtems_capture_refcount_down (rec->task);
    rtems_capture_destroy_capture_task (rec->task);
    rec++;
  }

  rtems_interrupt_disable (level);

  capture_count -= count;

  capture_out = (capture_out + count) % capture_size;

  capture_flags &= ~RTEMS_CAPTURE_READER_ACTIVE;

  rtems_interrupt_enable (level);

  return RTEMS_SUCCESSFUL;
}

/*
 * rtems_capture_tick_time
 *
 *  DESCRIPTION:
 *
 * This function returns the tick period in nano-seconds.
 */
uint32_t
rtems_capture_tick_time (void)
{
  return capture_tick_period;
}

/*
 * rtems_capture_event_text
 *
 *  DESCRIPTION:
 *
 * This function returns a string for an event based on the bit in the
 * event. The functions takes the bit offset as a number not the bit
 * set in a bit map.
 */
const char*
rtems_capture_event_text (int event)
{
  if ((event < RTEMS_CAPTURE_EVENT_START) || (event > RTEMS_CAPTURE_EVENT_END))
    return "invalid event id";
  return capture_event_text[event - RTEMS_CAPTURE_EVENT_START];
}

/*
 * rtems_capture_get_task_list
 *
 *  DESCRIPTION:
 *
 * This function returns the head of the list of tasks that the
 * capture engine has detected.
 */
rtems_capture_task_t*
rtems_capture_get_task_list (void)
{
  return capture_tasks;
}

/*
 * rtems_capture_task_stack_usage
 *
 *  DESCRIPTION:
 *
 * This function updates the stack usage. The task control block
 * is updated.
 */
uint32_t
rtems_capture_task_stack_usage (rtems_capture_task_t* task)
{
  if (task->tcb)
  {
    uint32_t* st;
    uint32_t* s;

    /*
     * @todo: Assumes all stacks move the same way.
     */
    st = task->tcb->Start.Initial_stack.area + task->stack_size;
    s  = task->tcb->Start.Initial_stack.area;

    while (s < st)
    {
      if (*s != 0xdeaddead)
        break;
      s++;
    }

    task->stack_clean =
      s - (uint32_t*) task->tcb->Start.Initial_stack.area;
  }

  return task->stack_clean;
}

/*
 * rtems_capture_get_control_list
 *
 *  DESCRIPTION:
 *
 * This function returns the head of the list of control in the
 * capture engine.
 */
rtems_capture_control_t*
rtems_capture_get_control_list (void)
{
  return capture_controls;
}
