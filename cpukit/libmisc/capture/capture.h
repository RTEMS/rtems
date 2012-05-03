/**
 * @file rtems/capture.h
 *
 * This is the Capture Engine component of the RTEMS Measurement and
 * Monitoring system.
 */

/*
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

/**
 * The number of tasks in a trigger group.
 */
#define RTEMS_CAPTURE_TRIGGER_TASKS (32)

/**
 * rtems_capture_from_t
 *
 *  DESCRIPTION:
 *
 * A from capture is a task id and a mask for the type of
 * from trigger we are interested in. The mask uses the same
 * bit maps as the flags field in the control structure. There
 * will only be a from type trigger if the flags in the control
 * structure has the specific *_BY bit set.
 */
typedef struct rtems_capture_from_s
{
  rtems_name name;
  rtems_id   id;
  uint32_t   trigger;
} rtems_capture_from_t;

/**
 * rtems_capture_control_t
 *
 *  DESCRIPTION:
 *
 * RTEMS control holds the trigger and watch configuration for a group of
 * tasks with the same name. The flags hold global control flags.
 *
 * The to_triggers fields holds triggers TO this task. The from_triggers holds
 * triggers from this task. The by_triggers is an OR or triggers which are
 * caused BY the task listed TO this task. The by_valid flag which entries
 * in by are valid.
 */
typedef struct rtems_capture_control_s
{
  rtems_name                      name;
  rtems_id                        id;
  uint32_t                        flags;
  uint32_t                        to_triggers;
  uint32_t                        from_triggers;
  uint32_t                        by_triggers;
  uint32_t                        by_valid;
  rtems_capture_from_t            by[RTEMS_CAPTURE_TRIGGER_TASKS];
  struct rtems_capture_control_s* next;
} rtems_capture_control_t;

/**
 * The from_valid mask.
 */
#define RTEMS_CAPTURE_CONTROL_FROM_MASK(_s) \
 (UINT32_C(1) << (RTEMS_CAPTURE_TRIGGER_TASKS - ((_s) + 1)))

/**
 * Control flags.
 */
#define RTEMS_CAPTURE_WATCH         (1U << 0)

/**
 * Control triggers.
 */
#define RTEMS_CAPTURE_SWITCH        (1 << 0)
#define RTEMS_CAPTURE_CREATE        (1 << 1)
#define RTEMS_CAPTURE_START         (1 << 2)
#define RTEMS_CAPTURE_RESTART       (1 << 3)
#define RTEMS_CAPTURE_DELETE        (1 << 4)
#define RTEMS_CAPTURE_BEGIN         (1 << 5)
#define RTEMS_CAPTURE_EXITTED       (1 << 6)

#define RTEMS_CAPTURE_FROM_TRIGS    (RTEMS_CAPTURE_SWITCH  | \
                                     RTEMS_CAPTURE_CREATE | \
                                     RTEMS_CAPTURE_START | \
                                     RTEMS_CAPTURE_RESTART | \
                                     RTEMS_CAPTURE_DELETE)

#define RTEMS_CAPTURE_TO_TRIGS      (RTEMS_CAPTURE_SWITCH | \
                                     RTEMS_CAPTURE_CREATE | \
                                     RTEMS_CAPTURE_START | \
                                     RTEMS_CAPTURE_RESTART | \
                                     RTEMS_CAPTURE_DELETE | \
                                     RTEMS_CAPTURE_BEGIN | \
                                     RTEMS_CAPTURE_EXITTED)

/**
 * rtems_capture_task_t
 *
 *  DESCRIPTION:
 *
 * RTEMS capture control provdes the information about a task, along
 * with its trigger state. The control is referenced by each
 * capture record. This is information neeed by the decoder. The
 * capture record cannot assume the task will exist when the record is
 * dumped via the target interface so task info needed for tracing is
 * copied and held here. Once the references in the trace buffer
 * have been removed and the task is deleted this structure is
 * released back to the heap.
 *
 * The inline helper functions provide more details about the info
 * contained in this structure.
 *
 * Note, the tracer code exploits the fact an rtems_name is a
 * 32bit value.
 */
typedef struct rtems_capture_task_s
{
  rtems_name                   name;
  rtems_id                     id;
  uint32_t                     flags;
  uint32_t                     refcount;
  rtems_tcb*                   tcb;
  uint32_t                     in;
  uint32_t                     out;
  rtems_task_priority          start_priority;
  uint32_t                     stack_size;
  uint32_t                     stack_clean;
  uint32_t                     ticks;
  uint32_t                     tick_offset;
  uint32_t                     ticks_in;
  uint32_t                     tick_offset_in;
  uint32_t                     last_ticks;
  uint32_t                     last_tick_offset;
  rtems_capture_control_t*     control;
  struct rtems_capture_task_s* forw;
  struct rtems_capture_task_s* back;
} rtems_capture_task_t;

/**
 * Task flags.
 */
#define RTEMS_CAPTURE_TRACED  (1U << 0)

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
  uint32_t              events;
  uint32_t              ticks;
  uint32_t              tick_offset;
} rtems_capture_record_t;

/**
 * The capture record event flags.
 */
#define RTEMS_CAPTURE_REAL_PRI_EVENT_MASK UINT32_C (0x000000ff)
#define RTEMS_CAPTURE_CURR_PRI_EVENT_MASK UINT32_C (0x0000ff00)
#define RTEMS_CAPTURE_REAL_PRIORITY_EVENT (0)
#define RTEMS_CAPTURE_CURR_PRIORITY_EVENT (8)
#define RTEMS_CAPTURE_EVENT_START         (16)
#define RTEMS_CAPTURE_CREATED_BY_EVENT    UINT32_C (0x00010000)
#define RTEMS_CAPTURE_CREATED_EVENT       UINT32_C (0x00020000)
#define RTEMS_CAPTURE_STARTED_BY_EVENT    UINT32_C (0x00040000)
#define RTEMS_CAPTURE_STARTED_EVENT       UINT32_C (0x00080000)
#define RTEMS_CAPTURE_RESTARTED_BY_EVENT  UINT32_C (0x00100000)
#define RTEMS_CAPTURE_RESTARTED_EVENT     UINT32_C (0x00200000)
#define RTEMS_CAPTURE_DELETED_BY_EVENT    UINT32_C (0x00400000)
#define RTEMS_CAPTURE_DELETED_EVENT       UINT32_C (0x00800000)
#define RTEMS_CAPTURE_BEGIN_EVENT         UINT32_C (0x01000000)
#define RTEMS_CAPTURE_EXITTED_EVENT       UINT32_C (0x02000000)
#define RTEMS_CAPTURE_SWITCHED_OUT_EVENT  UINT32_C (0x04000000)
#define RTEMS_CAPTURE_SWITCHED_IN_EVENT   UINT32_C (0x08000000)
#define RTEMS_CAPTURE_TIMESTAMP           UINT32_C (0x10000000)
#define RTEMS_CAPTURE_EVENT_END           (28)

/**
 * rtems_capture_trigger_mode_t
 *
 *  DESCRIPTION:
 *
 * The types of trigger modes that exist.
 */
typedef enum rtems_capture_trigger_mode_e
{
  rtems_capture_to_any,
  rtems_capture_from_any,
  rtems_capture_from_to
} rtems_capture_trigger_mode_t;

/**
 * rtems_capture_trigger_t
 *
 *  DESCRIPTION:
 *
 * The types of triggers that exist.
 */
typedef enum rtems_capture_trigger_e
{
  rtems_capture_switch,
  rtems_capture_create,
  rtems_capture_start,
  rtems_capture_restart,
  rtems_capture_delete,
  rtems_capture_begin,
  rtems_capture_exitted
} rtems_capture_trigger_t;

/**
 * rtems_capture_timestamp
 *
 *  DESCRIPTION:
 *
 * This defines the callout handler to obtain a time stamp. The
 * value returned is time count since the last read.
 *
 */

typedef void (*rtems_capture_timestamp)
                (uint32_t* ticks, uint32_t* micro);

/**
 * rtems_capture_open
 *
 *  DESCRIPTION:
 *
 * This function initialises the realtime trace manager allocating the
 * capture buffer. It is assumed we have a working heap at stage of
 * initialisation.
 *
 */
rtems_status_code
rtems_capture_open (uint32_t                size,
                    rtems_capture_timestamp timestamp);

/**
 * rtems_capture_close
 *
 *  DESCRIPTION:
 *
 * This function shutdowns the tracer and release any claimed
 * resources.
 */
rtems_status_code
rtems_capture_close (void);

/**
 * rtems_capture_control
 *
 *  DESCRIPTION:
 *
 * This function allows control of tracing at a global level.
 */
rtems_status_code
rtems_capture_control (bool enable);

/**
 * rtems_capture_monitor
 *
 *  DESCRIPTION:
 *
 * This function enable the monitor mode. When in the monitor mode
 * the tasks are monitored but no data is saved. This can be used
 * to profile the load on a system.
 */
rtems_status_code
rtems_capture_monitor (bool enable);

/*
 * rtems_capture_flush
 *
 *  DESCRIPTION:
 *
 * This function flushes the trace buffer. The prime parameter allows the
 * capture engine to also be primed again.
 */
rtems_status_code
rtems_capture_flush (bool prime);

/**
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

/**
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

/**
 * rtems_capture_watch_set
 *
 *  DESCRIPTION:
 *
 * This function allows control of a watch. The watch can be enabled or
 * disabled.
 */
rtems_status_code
rtems_capture_watch_ctrl (rtems_name    name,
                          rtems_id      id,
                          bool enable);

/**
 * rtems_capture_watch_global
 *
 *  DESCRIPTION:
 *
 * This function allows control of a global watch. The watch can
 * be enabled or disabled. A global watch configures all tasks below
 * the ceiling and above the floor to be traced.
 */
rtems_status_code
rtems_capture_watch_global (bool enable);

/**
 * rtems_capture_watch_global_on
 *
 *  DESCRIPTION:
 *
 * This function returns the global watch state.
 */
bool
rtems_capture_watch_global_on (void);

/**
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

/**
 * rtems_capture_watch_get_ceiling
 *
 *  DESCRIPTION:
 *
 * This function gets the watch ceiling.
 */
rtems_task_priority
rtems_capture_watch_get_ceiling (void);

/**
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

/**
 * rtems_capture_watch_get_floor
 *
 *  DESCRIPTION:
 *
 * This function gets the watch floor.
 */
rtems_task_priority
rtems_capture_watch_get_floor (void);

/**
 * rtems_capture_set_trigger
 *
 *  DESCRIPTION:
 *
 * This function sets a trigger.
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
rtems_capture_set_trigger (rtems_name                   from_name,
                           rtems_id                     from_id,
                           rtems_name                   to_name,
                           rtems_id                     to_id,
                           rtems_capture_trigger_mode_t mode,
                           rtems_capture_trigger_t      trigger);

/**
 * rtems_capture_clear_trigger
 *
 *  DESCRIPTION:
 *
 * This function clears a trigger.
 *
 * This clear trigger routine will not clear a watch.
 */
rtems_status_code
rtems_capture_clear_trigger (rtems_name                   from_name,
                             rtems_id                     from_id,
                             rtems_name                   to_name,
                             rtems_id                     to_id,
                             rtems_capture_trigger_mode_t mode,
                             rtems_capture_trigger_t      trigger);

/**
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
 * The 'timeout' parameter is in micro-seconds. A value of 0 will
 * disable the timeout.
 *
 */
rtems_status_code
rtems_capture_read (uint32_t                 threshold,
                    uint32_t                 timeout,
                    uint32_t*                read,
                    rtems_capture_record_t** recs);

/**
 * rtems_capture_release
 *
 *  DESCRIPTION:
 *
 * This function releases the requested number of record slots back
 * to the capture engine. The count must match the number read.
 */
rtems_status_code
rtems_capture_release (uint32_t count);

/**
 * rtems_capture_tick_time
 *
 *  DESCRIPTION:
 *
 * This function returns the tick period in micro-seconds.
 */
uint32_t
rtems_capture_tick_time (void);

/*
 * rtems_capture_tick_time
 *
 *  DESCRIPTION:
 *
 * This function returns the tick period in micro-seconds.
 */
uint32_t
rtems_capture_tick_time (void);

/**
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

/**
 * rtems_capture_get_task_list
 *
 *  DESCRIPTION:
 *
 * This function returns the head of the list of tasks that the
 * capture engine has detected.
 */
rtems_capture_task_t*
rtems_capture_get_task_list (void);

/**
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
  return task->forw;
}

/**
 * rtems_capture_task_valid
 *
 *  DESCRIPTION:
 *
 * This function returns true if the task control block points to
 * a valid task.
 */
static inline bool
rtems_capture_task_valid (rtems_capture_task_t* task)
{
  return task->tcb != NULL;
}

/**
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

/**
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

/**
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

/**
 * rtems_capture_task_flags
 *
 *  DESCRIPTION:
 *
 * This function returns the task flags.
 */
static inline uint32_t
rtems_capture_task_flags (rtems_capture_task_t* task)
{
  return task->flags;
}

/**
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

/**
 * rtems_capture_task_control_flags
 *
 *  DESCRIPTION:
 *
 * This function returns the task control flags if a control is present.
 */
static inline uint32_t
rtems_capture_task_control_flags (rtems_capture_task_t* task)
{
  if (!task->control)
    return 0;
  return task->control->flags;
}

/**
 * rtems_capture_task_switched_in
 *
 *  DESCRIPTION:
 *
 * This function returns the number of times the task has
 * been switched into context.
 */
static inline uint32_t
rtems_capture_task_switched_in (rtems_capture_task_t* task)
{
  return task->in;
}

/**
 * rtems_capture_task_switched_out
 *
 *  DESCRIPTION:
 *
 * This function returns the number of times the task has
 * been switched out of context.
 */
static inline uint32_t
rtems_capture_task_switched_out (rtems_capture_task_t* task)
{
  return task->out;
}

/**
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

/**
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

/**
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

/**
 * rtems_capture_task_stack_usage
 *
 *  DESCRIPTION:
 *
 * This function updates the stack usage. The task control block
 * is updated.
 */
uint32_t
rtems_capture_task_stack_usage (rtems_capture_task_t* task);

/**
 * rtems_capture_task_stack_size
 *
 *  DESCRIPTION:
 *
 * This function returns the task's stack size.
 */
static inline uint32_t
rtems_capture_task_stack_size (rtems_capture_task_t* task)
{
  return task->stack_size;
}

/**
 * rtems_capture_task_stack_used
 *
 *  DESCRIPTION:
 *
 * This function returns the amount of stack used.
 */
static inline uint32_t
rtems_capture_task_stack_used (rtems_capture_task_t* task)
{
  return task->stack_size - task->stack_clean;
}

/**
 * rtems_capture_task_ticks
 *
 *  DESCRIPTION:
 *
 * This function returns the current execution time as ticks.
 */
static inline uint32_t
rtems_capture_task_ticks (rtems_capture_task_t* task)
{
  return task->ticks;
}

/**
 * rtems_capture_task_tick_offset
 *
 *  DESCRIPTION:
 *
 * This function returns the current execution time tick offset.
 */
static inline uint32_t
rtems_capture_task_tick_offset (rtems_capture_task_t* task)
{
  return task->tick_offset;
}

/**
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

/**
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
  uint32_t     o = task->tick_offset - task->last_tick_offset;

  task->last_ticks       = task->ticks;
  task->last_tick_offset = task->tick_offset;

  return (t * rtems_capture_tick_time ()) + o;
}

/**
 * rtems_capture_task_count
 *
 *  DESCRIPTION:
 *
 * This function returns the number of tasks the capture
 * engine knows about.
 */
static inline uint32_t
rtems_capture_task_count (void)
{
  rtems_capture_task_t* task = rtems_capture_get_task_list ();
  uint32_t        count = 0;

  while (task)
  {
    count++;
    task = rtems_capture_next_task (task);
  }

  return count;
}

/**
 * rtems_capture_get_control_list
 *
 *  DESCRIPTION:
 *
 * This function returns the head of the list of controls in the
 * capture engine.
 */
rtems_capture_control_t*
rtems_capture_get_control_list (void);

/**
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

/**
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

/**
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

/**
 * rtems_capture_control_flags
 *
 *  DESCRIPTION:
 *
 * This function returns the control flags.
 */
static inline uint32_t
rtems_capture_control_flags (rtems_capture_control_t* control)
{
  return control->flags;
}

/**
 * rtems_capture_control_to_triggers
 *
 *  DESCRIPTION:
 *
 * This function returns the task control to triggers.
 */
static inline uint32_t
rtems_capture_control_to_triggers (rtems_capture_control_t* control)
{
  return control->to_triggers;
}

/**
 * rtems_capture_control_from_triggers
 *
 *  DESCRIPTION:
 *
 * This function returns the task control from triggers.
 */
static inline uint32_t
rtems_capture_control_from_triggers (rtems_capture_control_t* control)
{
  return control->from_triggers;
}

/**
 * rtems_capture_control_all_by_triggers
 *
 *  DESCRIPTION:
 *
 * This function returns the task control by triggers.
 */
static inline uint32_t
rtems_capture_control_all_by_triggers (rtems_capture_control_t* control)
{
  return control->by_triggers;
}

/**
 * rtems_capture_control_by_valid
 *
 *  DESCRIPTION:
 *
 * This function returns the control valid BY flags.
 */
static inline int
rtems_capture_control_by_valid (rtems_capture_control_t* control, int slot)
{
  return control->by_valid & RTEMS_CAPTURE_CONTROL_FROM_MASK (slot);
}

/**
 * rtems_capture_control_by_name
 *
 *  DESCRIPTION:
 *
 * This function returns the control BY task name.
 */
static inline rtems_name
rtems_capture_control_by_name (rtems_capture_control_t* control, int by)
{
  if (by < RTEMS_CAPTURE_TRIGGER_TASKS)
    return control->by[by].name;
  return control->by[0].name;
}

/**
 * rtems_capture_control_by_id
 *
 *  DESCRIPTION:
 *
 * This function returns the control BY task id.
 */
static inline rtems_id
rtems_capture_control_by_id (rtems_capture_control_t* control, int by)
{
  if (by < RTEMS_CAPTURE_TRIGGER_TASKS)
    return control->by[by].id;
  return control->by[0].id;
}

/**
 * rtems_capture_control_by_triggers
 *
 *  DESCRIPTION:
 *
 * This function returns the control BY task triggers.
 */
static inline uint32_t
rtems_capture_control_by_triggers (rtems_capture_control_t* control,
                                   int                      by)
{
  if (by < RTEMS_CAPTURE_TRIGGER_TASKS)
    return control->by[by].trigger;
  return control->by[0].trigger;
}

/**
 * rtems_capture_control_count
 *
 *  DESCRIPTION:
 *
 * This function returns the number of controls the capture
 * engine has.
 */
static inline uint32_t
rtems_capture_control_count (void)
{
  rtems_capture_control_t* control = rtems_capture_get_control_list ();
  uint32_t                 count = 0;

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
