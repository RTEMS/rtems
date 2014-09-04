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

#include "captureimpl.h"
#include "capture_buffer.h"

#include <rtems/score/statesimpl.h>
#include <rtems/score/todimpl.h>

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
                                      RTEMS_CAPTURE_EXITTED_EVENT | \
                                      RTEMS_CAPTURE_TERMINATED_EVENT | \
                                      RTEMS_CAPTURE_AUTOGEN_ENTRY_EVENT | \
                                      RTEMS_CAPTURE_AUTOGEN_EXIT_EVENT)
#else
#define RTEMS_CAPTURE_RECORD_EVENTS  (0)
#endif


/*
 * RTEMS Capture Data.
 */
static rtems_capture_buffer_t   capture_records = {NULL, 0, 0, 0, 0, 0};
static uint32_t                 capture_count;
static uint32_t                 capture_flags;
static rtems_capture_control_t* capture_controls;
static int                      capture_extension_index;
static rtems_capture_timestamp  capture_timestamp;
static rtems_task_priority      capture_ceiling;
static rtems_task_priority      capture_floor;
static rtems_id                 capture_reader;
static rtems_interrupt_lock     capture_lock =
  RTEMS_INTERRUPT_LOCK_INITIALIZER("capture");

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
  "TERMINATED",
  "BEGIN",
  "EXITTED",
  "SWITCHED_OUT",
  "SWITCHED_IN",
  "TIMESTAMP"
};

void rtems_capture_set_extension_index(int index)
{
  capture_extension_index = index;
}

int  rtems_capture_get_extension_index(void)
{
  return capture_extension_index;
}

uint32_t rtems_capture_get_flags(void)
{
  return capture_flags;
}

void rtems_capture_set_flags(uint32_t mask)
{
  capture_flags |= mask;
}

/*
 * This function returns the current time. If a handler is provided
 * by the user get the time from that.
 */
void
rtems_capture_get_time (rtems_capture_time_t* time)
{
  if (capture_timestamp)
    capture_timestamp (time);
  else
  {
    *time = rtems_clock_get_uptime_nanoseconds ();
  }
}

/*
 * This function compares rtems_names. It protects the
 * capture engine from a change to the way names are supported
 * in RTEMS.
 */
static inline bool
rtems_capture_match_names (rtems_name lhs, rtems_name rhs)
{
  return lhs == rhs;
}

/*
 * This function compares rtems_ids. It protects the
 * capture engine from a change to the way id are supported
 * in RTEMS.
 */
static inline bool
rtems_capture_match_ids (rtems_id lhs, rtems_id rhs)
{
  return lhs == rhs;
}

/*
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
 * This function duplicates an rtems_names. It protects the
 * capture engine from a change to the way names are supported
 * in RTEMS.
 */
static inline void
rtems_capture_dup_name (rtems_name* dst, rtems_name src)
{
  *dst = src;
}

/*
 * This function sees if a BY control is in the BY names. The use
 * of the valid_mask in this way assumes the number of trigger
 * tasks is the number of bits in uint32_t.
 */
static inline bool
rtems_capture_by_in_to (uint32_t                 events,
                        rtems_tcb*               by,
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
                                       rtems_capture_task_name( by ),
                                       by->Object.id))
        return 1;
    }

    valid_mask >>= 1;
    valid_remainder >>= 1;
  }

  return 0;
}

/*
 * This function searches for a trigger given a name.
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
 * This function checks if a new control structure matches
 * the given task and sets the control if it does.
 */
static void
rtems_capture_initialize_control (rtems_tcb *tcb)
{
  rtems_name                   name;
  rtems_capture_control_t*     control;

  /*
   * We need to scan the default control list to initialise
   * this control.
   */
  rtems_object_get_classic_name( tcb->Object.id, &name );
  control = capture_controls;
  if (rtems_capture_match_name_id (control->name, control->id,
                                   name, tcb->Object.id))
    tcb->Capture.control = control;
}

/*
 * This function creates a capture control for the capture engine.
 */
static inline rtems_capture_control_t*
rtems_capture_create_control (rtems_name name, rtems_id id)
{
  rtems_interrupt_lock_context lock_context;
  rtems_capture_control_t*     control;

  if ((name == 0) && (id == 0))
    return NULL;

  control = rtems_capture_find_control (name, id);

  if (control == NULL)
  {
    bool ok = rtems_workspace_allocate (sizeof (*control), (void **) &control);

    if (!ok)
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

    rtems_interrupt_lock_acquire (&capture_lock, &lock_context);

    control->next    = capture_controls;
    capture_controls = control;
    rtems_iterate_over_all_threads (rtems_capture_initialize_control);

    rtems_interrupt_lock_release (&capture_lock, &lock_context);
  }

  return control;
}

void rtems_capture_record_task( rtems_tcb* tcb )
{
  rtems_capture_control_t*    control;
  rtems_capture_task_record_t rec;
  void*                       ptr;

  rtems_object_get_classic_name( tcb->Object.id, &rec.name );

  /*
   * We need to scan the default control list to initialise
   * this control if it is a new task.
   */

  if (tcb->Capture.control == NULL) {
    for (control = capture_controls; control != NULL; control = control->next)
      if (rtems_capture_match_name_id (control->name, control->id,
                                       rec.name, tcb->Object.id))
        tcb->Capture.control = control;
  }

  rec.stack_size = tcb->Start.Initial_stack.size;
  rec.start_priority = _RTEMS_tasks_Priority_from_Core(
    tcb->Start.initial_priority
  );

  tcb->Capture.flags |= RTEMS_CAPTURE_RECORD_TASK;

  /*
   *  Log the task information. The first time a task is
   *  seen a record is logged.  This record can be identified
   *  by a 0 in the event identifier.
   */

  rtems_capture_begin_add_record (tcb, 0, sizeof(rec), &ptr);
  ptr = rtems_capture_append_to_record(
    ptr,
    &rec.name,
    sizeof( rec.name )
  );
  ptr = rtems_capture_append_to_record(
    ptr,
    &rec.start_priority,
    sizeof( rec.start_priority)
  );
  ptr = rtems_capture_append_to_record(
    ptr,
    &rec.stack_size,
    sizeof( rec.stack_size)
  );
  rtems_capture_end_add_record ( ptr );
}

/*
 * This function indicates if data should be filtered from the 
 * log.
 */
bool rtems_capture_filter( rtems_tcb*            tcb,
                           uint32_t              events)
{
  if (tcb &&
      ((capture_flags &
        (RTEMS_CAPTURE_TRIGGERED | RTEMS_CAPTURE_ONLY_MONITOR)) ==
       RTEMS_CAPTURE_TRIGGERED))
  {
    rtems_capture_control_t* control;

    control = tcb->Capture.control;

    /*
     * Capture the record if we have an event that is always
     * captured, or the task's real priority is greater than the
     * watch ceiling, and the global watch or task watch is enabled.
     */
    if ((events & RTEMS_CAPTURE_RECORD_EVENTS) ||
        ((tcb->real_priority >= capture_ceiling) &&
         (tcb->real_priority <= capture_floor) &&
         ((capture_flags & RTEMS_CAPTURE_GLOBAL_WATCH) ||
          (control && (control->flags & RTEMS_CAPTURE_WATCH)))))
    {
      return false;
    }
  }

  return true;
}

/*
 * This function records a capture record into the capture buffer.
 */
void *
rtems_capture_record_open (rtems_tcb*                    tcb,
                           uint32_t                      events,
                           size_t                        size,
                           rtems_interrupt_lock_context* lock_context)
{
  uint8_t*                     ptr;
  rtems_capture_record_t*      capture_in;

  rtems_interrupt_lock_acquire (&capture_lock, lock_context);

  ptr = rtems_capture_buffer_allocate(&capture_records, size);
  capture_in = (rtems_capture_record_t *) ptr;
  if ( capture_in )
  {
    capture_count++;
    capture_in->size    = size;
    capture_in->task_id = tcb->Object.id;
    capture_in->events  = (events |
                          (tcb->real_priority) |
                          (tcb->current_priority << 8));

    if ((events & RTEMS_CAPTURE_RECORD_EVENTS) == 0)
      tcb->Capture.flags |= RTEMS_CAPTURE_TRACED;

    rtems_capture_get_time (&capture_in->time);

    ptr = ptr + sizeof(*capture_in);
  }
  else
    capture_flags |= RTEMS_CAPTURE_OVERFLOW;

  return ptr;
}

void rtems_capture_record_close( void *rec, rtems_interrupt_lock_context* lock_context)
{
  rtems_interrupt_lock_release (&capture_lock, lock_context);
}

/*
 * See if we have triggered and if not see if this event is a
 * cause of a trigger.
 */
bool
rtems_capture_trigger (rtems_tcb* ft,
                       rtems_tcb* tt,
                       uint32_t   events)
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
      fc = ft->Capture.control;
      if (fc)
        from_events = fc->from_triggers & events;
    }

    if (tt)
    {
      tc = tt->Capture.control;
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
 * This function initialises the realtime capture engine allocating the trace
 * buffer. It is assumed we have a working heap at stage of initialisation.
 */
rtems_status_code
rtems_capture_open (uint32_t   size, rtems_capture_timestamp timestamp __attribute__((unused)))
{
  rtems_status_code      sc;

  /*
   * See if the capture engine is already open.
   */

  if (capture_records.buffer)
    return RTEMS_RESOURCE_IN_USE;

  rtems_capture_buffer_create( &capture_records, size );

  if (capture_records.buffer == NULL)
    return RTEMS_NO_MEMORY;

  capture_count   = 0;
  capture_flags   = 0;
  capture_ceiling = 0;
  capture_floor   = 255;

  sc = rtems_capture_user_extension_open();

  if (sc != RTEMS_SUCCESSFUL)
  {
    rtems_capture_buffer_destroy( &capture_records);
  }

  /*
   * Iterate over the list of existing tasks.
   */

  return sc;
}

/*
 * This function shutdowns the capture engine and release any claimed
 * resources.
 */
rtems_status_code
rtems_capture_close (void)
{
  rtems_interrupt_lock_context lock_context;
  rtems_capture_control_t*     control;
  rtems_status_code            sc;

  rtems_interrupt_lock_acquire (&capture_lock, &lock_context);

  if (!capture_records.buffer)
  {
    rtems_interrupt_lock_release (&capture_lock, &lock_context);
    return RTEMS_SUCCESSFUL;
  }

  capture_flags &= ~(RTEMS_CAPTURE_ON | RTEMS_CAPTURE_ONLY_MONITOR);

  rtems_interrupt_lock_release (&capture_lock, &lock_context);

  /*
   * Delete the extension first. This means we are now able to
   * release the resources we have without them being used.
   */

  sc = rtems_capture_user_extension_close();

  if (sc != RTEMS_SUCCESSFUL)
    return sc;

  control = capture_controls;

  while (control)
  {
    rtems_capture_control_t* delete = control;
    control = control->next;
    rtems_workspace_free (delete);
  }

  capture_controls = NULL;

  if (capture_records.buffer)
  {
    rtems_capture_buffer_destroy( &capture_records);
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code
rtems_capture_control (bool enable)
{
  rtems_interrupt_lock_context lock_context;

  rtems_interrupt_lock_acquire (&capture_lock, &lock_context);

  if (!capture_records.buffer)
  {
    rtems_interrupt_lock_release (&capture_lock, &lock_context);
    return RTEMS_UNSATISFIED;
  }

  if (enable)
    capture_flags |= RTEMS_CAPTURE_ON;
  else
    capture_flags &= ~RTEMS_CAPTURE_ON;

  rtems_interrupt_lock_release (&capture_lock, &lock_context);

  return RTEMS_SUCCESSFUL;
}

/*
 * This function enable the monitor mode. When in the monitor mode
 * the tasks are monitored but no data is saved. This can be used
 * to profile the load on a system.
 */
rtems_status_code
rtems_capture_monitor (bool enable)
{
  rtems_interrupt_lock_context lock_context;

  rtems_interrupt_lock_acquire (&capture_lock, &lock_context);

  if (!capture_records.buffer)
  {
    rtems_interrupt_lock_release (&capture_lock, &lock_context);
    return RTEMS_UNSATISFIED;
  }

  if (enable)
    capture_flags |= RTEMS_CAPTURE_ONLY_MONITOR;
  else
    capture_flags &= ~RTEMS_CAPTURE_ONLY_MONITOR;

  rtems_interrupt_lock_release (&capture_lock, &lock_context);

  return RTEMS_SUCCESSFUL;
}

/*
 * This function clears the capture trace flag in the tcb.
 */
static void
rtems_capture_flush_tcb (rtems_tcb *tcb)
{
  tcb->Capture.flags &= ~RTEMS_CAPTURE_TRACED;
}


/*
 * This function flushes the capture buffer. The prime parameter allows the
 * capture engine to also be primed again.
 */
rtems_status_code
rtems_capture_flush (bool prime)
{
  rtems_interrupt_lock_context lock_context;

  rtems_interrupt_lock_acquire (&capture_lock, &lock_context);

  rtems_iterate_over_all_threads (rtems_capture_flush_tcb);

  if (prime)
    capture_flags &= ~(RTEMS_CAPTURE_TRIGGERED | RTEMS_CAPTURE_OVERFLOW);
  else
    capture_flags &= ~RTEMS_CAPTURE_OVERFLOW;

  rtems_capture_buffer_flush( &capture_records );
  capture_count = 0;

  rtems_interrupt_lock_release (&capture_lock, &lock_context);

  return RTEMS_SUCCESSFUL;
}

/*
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
 * This function removes a watch for a specific task given a name. The task
 * description will still exist if referenced by a trace record in the trace
 * buffer or a global watch is defined.
 */
rtems_status_code
rtems_capture_watch_del (rtems_name name, rtems_id id)
{
  rtems_interrupt_lock_context lock_context;
  rtems_capture_control_t*     control;
  rtems_capture_control_t**    prev_control;
  bool                         found = false;

  /*
   * Should this test be for wildcards ?
   */

  for (prev_control = &capture_controls, control = capture_controls;
       control != NULL; )
  {
    if (rtems_capture_match_name_id (control->name, control->id, name, id))
    {
      rtems_interrupt_lock_acquire (&capture_lock, &lock_context);

      *prev_control = control->next;

      rtems_interrupt_lock_release (&capture_lock, &lock_context);

      rtems_workspace_free (control);

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
 * This function allows control of a watch. The watch can be enabled or
 * disabled.
 */
rtems_status_code
rtems_capture_watch_ctrl (rtems_name name, rtems_id id, bool enable)
{
  rtems_interrupt_lock_context lock_context;
  rtems_capture_control_t*     control;
  bool                         found = false;

  /*
   * Find the control and then set the watch. It must exist before it can
   * be controlled.
   */
  for (control = capture_controls; control != NULL; control = control->next)
  {
    if (rtems_capture_match_name_id (control->name, control->id, name, id))
    {
      rtems_interrupt_lock_acquire (&capture_lock, &lock_context);

      if (enable)
        control->flags |= RTEMS_CAPTURE_WATCH;
      else
        control->flags &= ~RTEMS_CAPTURE_WATCH;

      rtems_interrupt_lock_release (&capture_lock, &lock_context);

      found = true;
    }
  }

  if (found)
    return RTEMS_SUCCESSFUL;

  return RTEMS_INVALID_NAME;
}

/*
 * This function allows control of a global watch. The watch can be enabled or
 * disabled. A global watch configures all tasks below the ceiling and above
 * the floor to be traced.
 */
rtems_status_code
rtems_capture_watch_global (bool enable)
{
  rtems_interrupt_lock_context lock_context;

  rtems_interrupt_lock_acquire (&capture_lock, &lock_context);

  /*
   * We need to keep specific and global watches separate so
   * a global enable/disable does not lose a specific watch.
   */
  if (enable)
    capture_flags |= RTEMS_CAPTURE_GLOBAL_WATCH;
  else
    capture_flags &= ~RTEMS_CAPTURE_GLOBAL_WATCH;

  rtems_interrupt_lock_release (&capture_lock, &lock_context);

  return RTEMS_SUCCESSFUL;
}

/*
 * This function returns the global watch state.
 */
bool
rtems_capture_watch_global_on (void)
{
  return capture_flags & RTEMS_CAPTURE_GLOBAL_WATCH ? 1 : 0;
}

/*
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
 * This function gets the watch ceiling.
 */
rtems_task_priority
rtems_capture_watch_get_ceiling (void)
{
  return capture_ceiling;
}

/*
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
 * This function gets the watch floor.
 */
rtems_task_priority
rtems_capture_watch_get_floor (void)
{
  return capture_floor;
}

/*
 * Map the trigger to a bit mask.
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
    case rtems_capture_terminated:
      return RTEMS_CAPTURE_TERMINATED;
    default:
      break;
  }
  return 0;
}

/*
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

static inline uint32_t rtems_capture_count_records( void* recs, size_t size )
{ 
  rtems_capture_record_t* rec;
  uint8_t*                ptr = recs;
  uint32_t                rec_count = 0;
  size_t                  byte_count = 0;
  
 
 while (byte_count < size) {
    rec = (rtems_capture_record_t*) ptr;
    rec_count++;
    _Assert( rec->size >= sizeof(*rec) );
    ptr += rec->size;
    byte_count += rec->size;
    _Assert( rec_count <= capture_count ); 
 };
   
 return rec_count;
}

/*
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
 */
rtems_status_code
rtems_capture_read (uint32_t                 threshold,
                    uint32_t                 timeout,
                    uint32_t*                read,
                    rtems_capture_record_t** recs)
{
  rtems_interrupt_lock_context lock_context;
  rtems_status_code            sc = RTEMS_SUCCESSFUL;
  size_t                       recs_size = 0;
  bool                         wrapped;

  *read = 0;
  *recs = NULL;

  rtems_interrupt_lock_acquire (&capture_lock, &lock_context);

  /*
   * Only one reader is allowed.
   */

  if (capture_flags & RTEMS_CAPTURE_READER_ACTIVE)
  {
    rtems_interrupt_lock_release (&capture_lock, &lock_context);
    return RTEMS_RESOURCE_IN_USE;
  }

  capture_flags |= RTEMS_CAPTURE_READER_ACTIVE;

  *recs = rtems_capture_buffer_peek( &capture_records, &recs_size );
  *read = rtems_capture_count_records( *recs, recs_size );

  rtems_interrupt_lock_release (&capture_lock, &lock_context);

  for (;;)
  {
    /*
     * See if the data wraps the end of the record buffer.
     */
    wrapped = rtems_capture_buffer_has_wrapped( &capture_records);

    /*
     * Do we have a threshold and have not wrapped
     * around the end of the capture record buffer ?
     */
    if ((!wrapped) && threshold)
    {
      /*
       * Do we have enough records ?
       */
      if (*read < threshold)
      {
        rtems_event_set event_out;

        rtems_task_ident (RTEMS_SELF, RTEMS_LOCAL, &capture_reader);

        rtems_interrupt_lock_acquire (&capture_lock, &lock_context);

        capture_flags |= RTEMS_CAPTURE_READER_WAITING;

        rtems_interrupt_lock_release (&capture_lock, &lock_context);

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

        rtems_interrupt_lock_acquire (&capture_lock, &lock_context);

        *recs = rtems_capture_buffer_peek( &capture_records, &recs_size );
        *read = rtems_capture_count_records( *recs, recs_size );

        rtems_interrupt_lock_release (&capture_lock, &lock_context);

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
 * This function releases the requested number of record slots back
 * to the capture engine. The count must match the number read.
 */
rtems_status_code
rtems_capture_release (uint32_t count)
{
  rtems_interrupt_lock_context lock_context;
  uint8_t*                     ptr;
  rtems_capture_record_t*      rec;
  uint32_t                     counted;
  size_t                       ptr_size = 0;

  rtems_interrupt_lock_acquire (&capture_lock, &lock_context);

  if (count > capture_count)
    count = capture_count;

  rtems_interrupt_lock_release (&capture_lock, &lock_context);

  counted = count;
 
  ptr = rtems_capture_buffer_peek( &capture_records, &ptr_size );
  _Assert(ptr_size >= (count * sizeof(*rec) ));

  ptr_size = 0;
  while (counted--)
  { 
    rec = (rtems_capture_record_t*) ptr;
    ptr_size += rec->size;
    ptr += rec->size;
  }

  rtems_interrupt_lock_acquire (&capture_lock, &lock_context);

  capture_count -= count;

  if (count) 
    rtems_capture_buffer_free( &capture_records, ptr_size );

  capture_flags &= ~RTEMS_CAPTURE_READER_ACTIVE;

  rtems_interrupt_lock_release (&capture_lock, &lock_context);

  return RTEMS_SUCCESSFUL;
}

/*
 * This function returns the current time. If a handler is provided
 * by the user get the time from that.
 */
void
rtems_capture_time (rtems_capture_time_t* uptime)
{
  rtems_capture_get_time(uptime);
}

/*
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
 * This function returns the head of the list of control in the
 * capture engine.
 */
rtems_capture_control_t*
rtems_capture_get_control_list (void)
{
  return capture_controls;
}


