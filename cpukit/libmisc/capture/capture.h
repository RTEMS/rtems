/*
  ------------------------------------------------------------------------
  $Id$
  ------------------------------------------------------------------------
  
  Copyright Objective Design Systems Pty Ltd, 2002
  All rights reserved Objective Design Systems Pty Ltd, 2002
  Chris Johns (ccj@acm.org)

  COPYRIGHT (c) 1989-1998.
  On-Line Applications Research Corporation (OAR).

  The license and distribution terms for this file may be
  found in the file LICENSE in this distribution.

  This software with is provided ``as is'' and with NO WARRANTY.
  
  ------------------------------------------------------------------------

  RTEMS Performance Monitoring and Measurement Framework.

  This is the Capture Engine component.

*/

#ifndef __CAPTURE_H_
#define __CAPTURE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>

/*
 * The number of tasks in a trigger group.
 */
#define RTEMS_CAPTURE_TRIGGER_TASKS (32)

/*
 * rtems_capture_control_t
 *
 *  DESCRIPTION:
 *
 * RTEMS control holds the trigger and watch configuration for a group of 
 * tasks with the same name.
 */
typedef struct rtems_capture_control_s
{
  rtems_name                      name;
  rtems_id                        id;
  rtems_unsigned32                flags;
  rtems_name                      from[RTEMS_CAPTURE_TRIGGER_TASKS];
  rtems_id                        from_id[RTEMS_CAPTURE_TRIGGER_TASKS];
  struct rtems_capture_control_s* next;
} rtems_capture_control_t;

/*
 * Control flags.
 */
#define RTEMS_CAPTURE_WATCH         (1 << 0)
#define RTEMS_CAPTURE_FROM_ANY      (1 << 1)
#define RTEMS_CAPTURE_TO_ANY        (1 << 2)
#define RTEMS_CAPTURE_FROM_TO       (1 << 3)
 
/*
 * rtems_capture_control_t
 *
 *  DESCRIPTION:
 *
 * RTEMS capture control provdes the information about a task, along 
 * with its trigger state. The control is referenced by each
 * capture record. This is* information neeed by the decoder. The 
 * capture record cannot assume the task will exist when the record is
 * dumped via the target interface so task info needed for tracing is
 * copied and held here.
 *
 * The inline heper functions provide more details about the info
 * contained in this structure.
 *
 * Note, the tracer code exploits the fact an rtems_name is a
 * 32bit value.  
 */
typedef struct rtems_capture_task_s
{
  rtems_name                   name;
  rtems_id                     id;
  rtems_unsigned32             flags;
  rtems_tcb*                   tcb;
  rtems_unsigned32             in;
  rtems_unsigned32             out;
  rtems_task_priority          start_priority;
  rtems_unsigned32             stack_size;
  rtems_unsigned32             stack_clean;
  rtems_unsigned32             ticks;
  rtems_unsigned32             tick_offset;
  rtems_unsigned32             ticks_in;
  rtems_unsigned32             tick_offset_in;
  rtems_unsigned32             last_ticks;
  rtems_unsigned32             last_tick_offset;
  rtems_capture_control_t*     control;
  struct rtems_capture_task_s* next;
} rtems_capture_task_t;

/*
 * Task flags.
 */
#define RTEMS_CAPTURE_TRACED  (1 << 0)

/*
 * rtems_capture_record_t
 *
 *  DESCRIPTION:
 *
 * RTEMS capture record. This is a record that is written into
 * the buffer. The events includes the priority of the task
 * at the time of the context switch.
 */
typedef struct rtems_capture_record_s
{
  rtems_capture_task_t* task;
  rtems_unsigned32      events;
  rtems_unsigned32      ticks;
  rtems_unsigned32      tick_offset;
} rtems_capture_record_t;

/*
 * The capture record event flags.
 */
#define RTEMS_CAPTURE_REAL_PRI_EVENT_MASK (0x000000ff)
#define RTEMS_CAPTURE_CURR_PRI_EVENT_MASK (0x0000ff00)
#define RTEMS_CAPTURE_REAL_PRIORITY_EVENT (0)
#define RTEMS_CAPTURE_CURR_PRIORITY_EVENT (8)
#define RTEMS_CAPTURE_EVENT_START         (16)
#define RTEMS_CAPTURE_CREATED_BY_EVENT    (1 << 16)
#define RTEMS_CAPTURE_CREATED_EVENT       (1 << 17)
#define RTEMS_CAPTURE_STARTED_BY_EVENT    (1 << 18)
#define RTEMS_CAPTURE_STARTED_EVENT       (1 << 19)
#define RTEMS_CAPTURE_RESTARTED_BY_EVENT  (1 << 20)
#define RTEMS_CAPTURE_RESTARTED_EVENT     (1 << 21)
#define RTEMS_CAPTURE_DELETED_BY_EVENT    (1 << 22)
#define RTEMS_CAPTURE_DELETED_EVENT       (1 << 23)
#define RTEMS_CAPTURE_BEGIN_EVENT         (1 << 24)
#define RTEMS_CAPTURE_EXITTED_EVENT       (1 << 25)
#define RTEMS_CAPTURE_SWITCHED_OUT_EVENT  (1 << 26)
#define RTEMS_CAPTURE_SWITCHED_IN_EVENT   (1 << 27)
#define RTEMS_CAPTURE_TIMESTAMP           (1 << 28)
#define RTEMS_CAPTURE_EVENT_END           (28)

/*
 * rtems_capture_trigger_t
 *
 *  DESCRIPTION:
 *
 * The types of triggers that exist. FIXME: add more here.
 */
typedef enum rtems_capture_trigger_t
{
  rtems_capture_to_any,
  rtems_capture_from_any,
  rtems_capture_from_to
} rtems_capture_trigger_t;

/*
 * rtems_capture_timestamp
 *
 *  DESCRIPTION:
 *
 * This defines the callout handler to obtain a time stamp. The
 * value returned is time count since the last read.
 *
 */

typedef void (*rtems_capture_timestamp)
                (rtems_unsigned32* ticks, rtems_unsigned32* micro);

/*
 * rtems_capture_open
 *
 *  DESCRIPTION:
 *
 * This function initialises the realtime trace manager allocating the capture
 * buffer. It is assumed we have a working heap at stage of initialisation.
 *
 */
rtems_status_code
rtems_capture_open (rtems_unsigned32        size,
                    rtems_capture_timestamp timestamp);

/*
 * rtems_capture_close
 *
 *  DESCRIPTION:
 *
 * This function shutdowns the tracer and release any claimed
 * resources.
 */
rtems_status_code
rtems_capture_close ();

/*
 * rtems_capture_control
 *
 *  DESCRIPTION:
 *
 * This function allows control of tracing at a global level.
 */
rtems_status_code
rtems_capture_control (rtems_boolean enable);

/*
 * rtems_capture_flush
 *
 *  DESCRIPTION:
 *
 * This function flushes the trace buffer. The prime parameter allows the
 * capture engine to also be primed again.
 */
rtems_status_code
rtems_capture_flush (rtems_boolean prime);

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
rtems_capture_watch_add (rtems_name name, rtems_id id);

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
rtems_capture_watch_del (rtems_name name, rtems_id id);

/*
 * rtems_capture_watch_set
 *
 *  DESCRIPTION:
 *
 * This function allows control of a watch. The watch can be enabled or
 * disabled.
 */
rtems_status_code
rtems_capture_watch_ctrl (rtems_name name, rtems_id id, rtems_boolean enable);

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
rtems_capture_watch_global (rtems_boolean enable);

/*
 * rtems_capture_watch_global_on
 *
 *  DESCRIPTION:
 *
 * This function returns the global watch state.
 */
rtems_boolean
rtems_capture_watch_global_on ();

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
rtems_capture_watch_ceiling (rtems_task_priority ceiling);

/*
 * rtems_capture_watch_get_ceiling
 *
 *  DESCRIPTION:
 *
 * This function gets the watch ceiling.
 */
rtems_task_priority
rtems_capture_watch_get_ceiling ();

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
rtems_capture_watch_floor (rtems_task_priority floor);

/*
 * rtems_capture_watch_get_floor
 *
 *  DESCRIPTION:
 *
 * This function gets the watch floor.
 */
rtems_task_priority
rtems_capture_watch_get_floor ();

/*
 * rtems_capture_set_trigger
 *
 *  DESCRIPTION:
 *
 * This function sets an edge trigger. Left is the left side of
 * the edge and right is right side of the edge. The trigger type
 * can be -
 *
 *  FROM_ANY : a switch from any task to the right side of the edge.
 *  TO_ANY   : a switch from the left side of the edge to any task.
 *  FROM_TO  : a switch from the left side of the edge to the right
 *             side of the edge.
 *
 * This set trigger routine will create a trace control for the
 * target task. The task list is searched and any existing tasks
 * are linked to the new control.
 *
 * We can have a number of tasks that have the same name so we
 * search using names. This means a number of tasks can be
 * linked to single control.
 */
rtems_status_code
rtems_capture_set_trigger (rtems_name              from,
                           rtems_id                from_id,
                           rtems_name              to,
                           rtems_id                to_id,
                           rtems_capture_trigger_t trigger);

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
rtems_capture_read (rtems_unsigned32         threshold,
                    rtems_unsigned32         timeout,
                    rtems_unsigned32*        read,
                    rtems_capture_record_t** recs);

/*
 * rtems_capture_release
 *
 *  DESCRIPTION:
 *
 * This function releases the requested number of record slots back
 * to the capture engine. The count must match the number read.
 */
rtems_status_code
rtems_capture_release (rtems_unsigned32 count);

/*
 * rtems_capture_tick_time
 *
 *  DESCRIPTION:
 *
 * This function returns the tick period in micro-seconds.
 */
rtems_unsigned32
rtems_capture_tick_time ();

/*
 * rtems_capture_tick_time
 *
 *  DESCRIPTION:
 *
 * This function returns the tick period in micro-seconds.
 */
rtems_unsigned32
rtems_capture_tick_time ();

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
rtems_capture_event_text (int event);

/*
 * rtems_capture_get_task_list
 *
 *  DESCRIPTION:
 *
 * This function returns the head of the list of tasks that the
 * capture engine has detected.
 */
rtems_capture_task_t*
rtems_capture_get_task_list ();

/*
 * rtems_capture_next_task
 *
 *  DESCRIPTION:
 *
 * This function returns the pointer to the next task in the list. The
 * pointer NULL terminates the list.
 */
static inline rtems_capture_task_t*
rtems_capture_next_task (rtems_capture_task_t* task)
{
  return task->next;
}

/*
 * rtems_capture_task_valid
 *
 *  DESCRIPTION:
 *
 * This function returns true if the task control block points to
 * a valid task.
 */
static inline rtems_boolean
rtems_capture_task_valid (rtems_capture_task_t* task)
{
  return task->tcb != NULL;
}

/*
 * rtems_capture_task_id
 *
 *  DESCRIPTION:
 *
 * This function returns the task id.
 */
static inline rtems_id
rtems_capture_task_id (rtems_capture_task_t* task)
{
  return task->id;
}

/*
 * rtems_capture_task_state
 *
 *  DESCRIPTION:
 *
 * This function returns the task state.
 */
static inline States_Control
rtems_capture_task_state (rtems_capture_task_t* task)
{
  if (rtems_capture_task_valid (task))
    return task->tcb->current_state;
  return 0;
}

/*
 * rtems_capture_task_name
 *
 *  DESCRIPTION:
 *
 * This function returns the task name.
 */
static inline rtems_name
rtems_capture_task_name (rtems_capture_task_t* task)
{
  return task->name;
}

/*
 * rtems_capture_task_flags
 *
 *  DESCRIPTION:
 *
 * This function returns the task flags.
 */
static inline rtems_unsigned32
rtems_capture_task_flags (rtems_capture_task_t* task)
{
  return task->flags;
}

/*
 * rtems_capture_task_control
 *
 *  DESCRIPTION:
 *
 * This function returns the task control if present.
 */
static inline rtems_capture_control_t*
rtems_capture_task_control (rtems_capture_task_t* task)
{
  return task->control;
}

/*
 * rtems_capture_task_control_flags
 *
 *  DESCRIPTION:
 *
 * This function returns the task control flags if a control is present.
 */
static inline rtems_unsigned32
rtems_capture_task_control_flags (rtems_capture_task_t* task)
{
  if (!task->control)
    return 0;
  return task->control->flags;
}

/*
 * rtems_capture_task_switched_in
 *
 *  DESCRIPTION:
 *
 * This function returns the number of times the task has
 * been switched into context.
 */
static inline rtems_unsigned32
rtems_capture_task_switched_in (rtems_capture_task_t* task)
{
  return task->in;
}

/*
 * rtems_capture_task_switched_out
 *
 *  DESCRIPTION:
 *
 * This function returns the number of times the task has
 * been switched out of context.
 */
static inline rtems_unsigned32
rtems_capture_task_switched_out (rtems_capture_task_t* task)
{
  return task->out;
}

/*
 * rtems_capture_task_curr_priority
 *
 *  DESCRIPTION:
 *
 * This function returns the tasks start priority. The tracer needs this
 * to track where the task's priority goes.
 */
static inline rtems_task_priority
rtems_capture_task_start_priority (rtems_capture_task_t* task)
{
  return task->start_priority;
}

/*
 * rtems_capture_task_real_priority
 *
 *  DESCRIPTION:
 *
 * This function returns the tasks real priority.
 */
static inline rtems_task_priority
rtems_capture_task_real_priority (rtems_capture_task_t* task)
{
  if (rtems_capture_task_valid (task))
    return task->tcb->real_priority;
  return 0;
}

/*
 * rtems_capture_task_curr_priority
 *
 *  DESCRIPTION:
 *
 * This function returns the tasks current priority.
 */
static inline rtems_task_priority
rtems_capture_task_curr_priority (rtems_capture_task_t* task)
{
  if (rtems_capture_task_valid (task))
    return task->tcb->current_priority;
  return 0;
}

/*
 * rtems_capture_task_stack_usage
 *
 *  DESCRIPTION:
 *
 * This function updates the stack usage. The task control block
 * is updated.
 */
rtems_unsigned32
rtems_capture_task_stack_usage (rtems_capture_task_t* task);

/*
 * rtems_capture_task_stack_size
 *
 *  DESCRIPTION:
 *
 * This function returns the task's stack size.
 */
static inline rtems_unsigned32
rtems_capture_task_stack_size (rtems_capture_task_t* task)
{
  return task->stack_size;
}

/*
 * rtems_capture_task_stack_used
 *
 *  DESCRIPTION:
 *
 * This function returns the amount of stack used.
 */
static inline rtems_unsigned32
rtems_capture_task_stack_used (rtems_capture_task_t* task)
{
  return task->stack_size - task->stack_clean;
}

/*
 * rtems_capture_task_ticks
 *
 *  DESCRIPTION:
 *
 * This function returns the current execution time as ticks.
 */
static inline rtems_unsigned32
rtems_capture_task_ticks (rtems_capture_task_t* task)
{
  return task->ticks;
}

/*
 * rtems_capture_task_tick_offset
 *
 *  DESCRIPTION:
 *
 * This function returns the current execution time tick offset.
 */
static inline rtems_unsigned32
rtems_capture_task_tick_offset (rtems_capture_task_t* task)
{
  return task->tick_offset;
}

/*
 * rtems_capture_task_time
 *
 *  DESCRIPTION:
 *
 * This function returns the current execution time.
 */
static inline unsigned long long
rtems_capture_task_time (rtems_capture_task_t* task)
{
  unsigned long long t = task->ticks;
  return (t * rtems_capture_tick_time ()) + task->tick_offset;;
}

/*
 * rtems_capture_task_delta_time
 *
 *  DESCRIPTION:
 *
 * This function returns the execution time as a different between the
 * last time the detla time was and now.
 */
static inline unsigned long long
rtems_capture_task_delta_time (rtems_capture_task_t* task)
{
  unsigned long long t = task->ticks - task->last_ticks;
  rtems_unsigned32   o = task->tick_offset - task->last_tick_offset;

  task->last_ticks       = task->ticks;
  task->last_tick_offset = task->tick_offset;

  return (t * rtems_capture_tick_time ()) + o;
}

/*
 * rtems_capture_task_count
 *
 *  DESCRIPTION:
 *
 * This function returns the number of tasks the capture
 * engine knows about.
 */
static inline rtems_unsigned32
rtems_capture_task_count ()
{
  rtems_capture_task_t* task = rtems_capture_get_task_list ();
  rtems_unsigned32      count = 0;
  
  while (task)
  {
    count++;
    task = rtems_capture_next_task (task);
  }

  return count;
}

/*
 * rtems_capture_get_control_list
 *
 *  DESCRIPTION:
 *
 * This function returns the head of the list of controls in the
 * capture engine.
 */
rtems_capture_control_t*
rtems_capture_get_control_list ();

/*
 * rtems_capture_next_control
 *
 *  DESCRIPTION:
 *
 * This function returns the pointer to the next control in the list. The
 * pointer NULL terminates the list.
 */
static inline rtems_capture_control_t*
rtems_capture_next_control (rtems_capture_control_t* control)
{
  return control->next;
}

/*
 * rtems_capture_control_id
 *
 *  DESCRIPTION:
 *
 * This function returns the control id.
 */
static inline rtems_id
rtems_capture_control_id (rtems_capture_control_t* control)
{
  return control->id;
}

/*
 * rtems_capture_control_name
 *
 *  DESCRIPTION:
 *
 * This function returns the control name.
 */
static inline rtems_name
rtems_capture_control_name (rtems_capture_control_t* control)
{
  return control->name;
}

/*
 * rtems_capture_control_flags
 *
 *  DESCRIPTION:
 *
 * This function returns the control flags.
 */
static inline rtems_unsigned32
rtems_capture_control_flags (rtems_capture_control_t* control)
{
  return control->flags;
}

/*
 * rtems_capture_control_from_name
 *
 *  DESCRIPTION:
 *
 * This function returns the control from task name.
 */
static inline rtems_name
rtems_capture_control_from_name (rtems_capture_control_t* control, int from)
{
  if (from < RTEMS_CAPTURE_TRIGGER_TASKS)
    return control->from[from];
  return control->from[0];
}

/*
 * rtems_capture_control_from_id
 *
 *  DESCRIPTION:
 *
 * This function returns the control from task id.
 */
static inline rtems_id
rtems_capture_control_from_id (rtems_capture_control_t* control, int from)
{
  if (from < RTEMS_CAPTURE_TRIGGER_TASKS)
    return control->from_id[from];
  return control->from_id[0];
}

/*
 * rtems_capture_control_count
 *
 *  DESCRIPTION:
 *
 * This function returns the number of controls the capture
 * engine has.
 */
static inline rtems_unsigned32
rtems_capture_control_count ()
{
  rtems_capture_control_t* control = rtems_capture_get_control_list ();
  rtems_unsigned32         count = 0;

  while (control)
  {
    count++;
    control = rtems_capture_next_control (control);
  }

  return count;
}

#ifdef __cplusplus
}
#endif

#endif
