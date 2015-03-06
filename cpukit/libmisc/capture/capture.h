/**
 * @file rtems/capture.h
 *
 * @brief Capture Engine Component of the RTEMS Measurement and
 * Monitoring System
 *
 * This is the Capture Engine component of the RTEMS Measurement and
 * Monitoring system.
 */

/*
  ------------------------------------------------------------------------

  Copyright Objective Design Systems Pty Ltd, 2002
  All rights reserved Objective Design Systems Pty Ltd, 2002
  Chris Johns (ccj@acm.org)

  COPYRIGHT (c) 1989-2014
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

/**
 *  @defgroup libmisc_capture RTEMS Capture Engine
 *
 *  @ingroup libmisc
 *
 *  Capture Engine Component of the RTEMS Measurement and Monitoring System
 */
/**@{*/
#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <rtems/rtems/tasksimpl.h>

/**
 * The number of tasks in a trigger group.
 */
#define RTEMS_CAPTURE_TRIGGER_TASKS (32)

/**
 * @brief A capture timestamp.
 *
 * This is a nanosecond capture timestamp
 */
typedef uint64_t rtems_capture_time_t;

/**
 * @brief Task id and mask for the from trigger.
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
 * @brief Capture control structure for a group of tasks.
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
#define RTEMS_CAPTURE_TERMINATED    (1 << 7)

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
 * Task flags.
 */
#define RTEMS_CAPTURE_TRACED      (1U << 0)
#define RTEMS_CAPTURE_INIT_TASK   (1U << 1)
#define RTEMS_CAPTURE_RECORD_TASK (1U << 2)

/*
 * @brief Capture record.
 *
 * This is a record that is written into
 * the buffer. The events includes the priority of the task
 * at the time of the context switch.
 */
typedef struct rtems_capture_record_s
{
  uint32_t              events;
  rtems_capture_time_t  time;
  size_t                size;
  rtems_id              task_id;
} rtems_capture_record_t;

/*
 * @brief Capture task record.
 *
 * This is a record that is written into
 * the buffer. The events includes the priority of the task
 * at the time of the context switch.
 */
typedef struct rtems_capture_task_record_s
{
  rtems_capture_record_t rec;
  rtems_name             name;
  rtems_task_priority    start_priority;
  uint32_t               stack_size;
} rtems_capture_task_record_t;

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
#define RTEMS_CAPTURE_TERMINATED_EVENT    UINT32_C (0x01000000)
#define RTEMS_CAPTURE_BEGIN_EVENT         UINT32_C (0x02000000)
#define RTEMS_CAPTURE_EXITTED_EVENT       UINT32_C (0x04000000)
#define RTEMS_CAPTURE_SWITCHED_OUT_EVENT  UINT32_C (0x08000000)
#define RTEMS_CAPTURE_SWITCHED_IN_EVENT   UINT32_C (0x10000000)
#define RTEMS_CAPTURE_TIMESTAMP           UINT32_C (0x20000000)
#define RTEMS_CAPTURE_EVENT_END           (29)

/**
 * @brief Capture trigger modes
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
 * @brief Capture trigger.
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
  rtems_capture_exitted,
  rtems_capture_terminated
} rtems_capture_trigger_t;

/**
 * @brief Capture timestamp callout handler.
 *
 * This defines the callout handler to obtain a time stamp. The
 * value returned is time count since the last read.
 *
 */

typedef void (*rtems_capture_timestamp)(rtems_capture_time_t* time);

/**
 * @brief Capture open
 *
 * This function initialises the realtime trace manager allocating the
 * capture buffer. It is assumed we have a working heap at stage of
 * initialisation.
 *
 * @param[in] size The number of capture records to define.
 * @param[in] timestamp The timestamp callout handler to use. If the
 *            the handler is NULL a default  nano-second timestamp
 *            will be used.
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code
rtems_capture_open (uint32_t                size,
                    rtems_capture_timestamp timestamp);

/**
 * @brief Capture close
 *
 * This function shutdowns the tracer and release any claimed
 * resources.
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code
rtems_capture_close (void);

/**
 * @brief Capture control trace enable/disable.
 *
 * This function allows control of tracing at a global level.
 *
 * @param[in]  enable The trace enable/disable flag.
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code
rtems_capture_control (bool enable);

/**
 * @brief Capture monitor enable/disable.
 *
 * This function enable the monitor mode. When in the monitor mode
 * the tasks are monitored but no data is saved. This can be used
 * to profile the load on a system.
 *
 * @param[in]  enable The monitor enable/disable flag.
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code
rtems_capture_monitor (bool enable);

/*
 * @brief Capture flush trace buffer.
 *
 * This function flushes the trace buffer. The prime parameter allows the
 * capture engine to also be primed again.
 *
 * @param[in]  prime The prime after flush flag.
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code
rtems_capture_flush (bool prime);

/**
 * @brief Capture add watch
 *
 * This function defines a watch for a specific task given a name. A watch
 * causes it to be traced either in or out of context. The watch can be
 * optionally enabled or disabled with the set routine. It is disabled by
 * default.
 *
 * @param[in]  name The name of the @a capture_controls entry
 * @param[in]  id The id of the @a capture_controls entry.
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code
rtems_capture_watch_add (rtems_name name, rtems_id id);

/**
 * @brief Capture delete watch.
 *
 * This function removes a watch for a specific task given a name. The task
 * description will still exist if referenced by a trace record in the trace
 * buffer or a global watch is defined.
 *
 * @param[in]  name The name of the @a capture_controls entry
 * @param[in]  id The id of the @a capture_controls entry.
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code
rtems_capture_watch_del (rtems_name name, rtems_id id);

/**
 * @brief Capture enable/disable watch.
 *
 * This function allows control of a watch. The watch can be enabled or
 * disabled.
 *
 * @param[in]  name The name of the @a capture_controls entry
 * @param[in]  id The id of the @a capture_controls entry.
 * @param[in]  enable The enable/disable flag for the watch.
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code
rtems_capture_watch_ctrl (rtems_name    name,
                          rtems_id      id,
                          bool enable);

/**
 * @brief Capture enable/disable global watch.
 *
 * This function allows control of a global watch. The watch can
 * be enabled or disabled. A global watch configures all tasks below
 * the ceiling and above the floor to be traced.
 *
 * @param[in]  enable The enable/disable flag for the watch.
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code
rtems_capture_watch_global (bool enable);

/**
 * @brief Get global watch state
 *
 * This function returns the global watch state.
 *
 * @retval This method returns true  if the global watch
 *         is on.  Otherwise, it returns false.
 */
bool
rtems_capture_watch_global_on (void);

/**
 * @brief Set watch ceiling.
 *
 * This function sets a watch ceiling. Events from tasks at or greater
 * than the ceiling priority are ignored. This is a simple way to
 * monitor an application and exclude system tasks running at a higher
 * priority level.
 *
 * @param[in] ceiling specifies the priority level immediately above
 *     that at which events from tasks are not captured.
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code
rtems_capture_watch_ceiling (rtems_task_priority ceiling);

/**
 * @brief Get watch ceiling.
 *
 * This function gets the watch ceiling.
 *
 * @retval The priority level immediately above that at which events
 *         from tasks are not captured.
 */
rtems_task_priority
rtems_capture_watch_get_ceiling (void);

/**
 * @brief Capture set watch floor.
 *
 * This function sets a watch floor. Tasks at or less than the
 * floor priority are not watched. This is a simple way to monitor
 * an application and exclude system tasks running at a lower
 * priority level.
 *
 * @param[in] floor specifies the priority level immediately below
 *     that at which events from tasks are not captured.
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code
rtems_capture_watch_floor (rtems_task_priority floor);

/**
 * @brief Capture set watch floor
 *
 * This function gets the watch floor.
 *
 * @retval The priority level immediately below
 *     that at which events from tasks are not captured.
 */
rtems_task_priority
rtems_capture_watch_get_floor (void);

/**
 * @brief Capture set trigger
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
 *
 * Some events captured such as context switch include two
 * tasks. These are referred to as being "from" and "to"
 * Some events may only have one task specified.
 *
 * @param[in] from_name specifies the name of the from task.
 * @param[in] from_id specifies the id of the from task.
 * @param[in] to_name specifies the name of the to task.
 * @param[in] to_id specifies the id of the to task.
 * @param[in] mode specifies the trigger mode.
 * @param[in] trigger specifies the type of trigger.
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code
rtems_capture_set_trigger (rtems_name                   from_name,
                           rtems_id                     from_id,
                           rtems_name                   to_name,
                           rtems_id                     to_id,
                           rtems_capture_trigger_mode_t mode,
                           rtems_capture_trigger_t      trigger);

/**
 * @brief Capture clear trigger.
 *
 * This function clears a trigger.
 *
 * This clear trigger routine will not clear a watch.
 *
 * @param[in] from_name specifies the name of the from task.
 * @param[in] from_id specifies the id of the from task.
 * @param[in] to_name specifies the name of the to task.
 * @param[in] to_id specifies the id of the to task.
 * @param[in] mode specifies the trigger mode.
 * @param[in] trigger specifies the type of trigger.
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code
rtems_capture_clear_trigger (rtems_name                   from_name,
                             rtems_id                     from_id,
                             rtems_name                   to_name,
                             rtems_id                     to_id,
                             rtems_capture_trigger_mode_t mode,
                             rtems_capture_trigger_t      trigger);

/**
 * @brief Capture read records from capture buffer
 *
 * This function reads a number of records from the capture buffer.
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
 * @param[in]  cpu The cpu number that the records were recorded on
 * @param[out] read will contain the number of records read
 * @param[out] recs The capture records that are read.
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code
rtems_capture_read (uint32_t                 cpu,
                    uint32_t*                read,
                    rtems_capture_record_t** recs);

/**
 * @brief Capture release records.
 *
 * This function releases the requested number of record slots back
 * to the capture engine. The count must match the number read.
 *
 * @param[in] count The number of record slots to release
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code
rtems_capture_release (uint32_t cpu, uint32_t count);

/*
 * @brief Capture nano-second time period.
 *
 * This function returns the time period in nano-seconds.
 *
 * @param[out] uptime The nano-second time period.
 */
void
rtems_capture_time (rtems_capture_time_t* uptime);

/**
 * @brief Capture get event text.
 *
 * This function returns a string for an event based on the bit in the
 * event. The functions takes the bit offset as a number not the bit
 * set in a bit map.
 *
 * @param[in] event specifies the event to describe
 *
 * @retval This method returns a string description of the given event.
 */
const char*
rtems_capture_event_text (int event);

/**
 * @brief Capture initialize task
 *
 * This function initializes capture control in the tcb.
 *
 * @param[in] tcb is the task control block for the task
 */
void rtems_capture_initialize_task( rtems_tcb* tcb );

/**
 * @brief Capture record task.
 *
 * This function records a new capture task record.
 *
 * @param[in] tcb is the task control block for the task
 */
void rtems_capture_record_task( rtems_tcb* tcb );

/**
 * @brief Capture task recorded
 *
 * This function returns true if this task information has been
 * recorded.
 *
 * @param[in] tcb is the task control block for the task
 */
static inline bool rtems_capture_task_recorded( rtems_tcb* tcb ) {
  return ( (tcb->Capture.flags & RTEMS_CAPTURE_RECORD_TASK) != 0 );
}

/**
 * @brief Capture task initialized
 *
 * This function returns true if this task information has been
 * initialized.
 *
 * @param[in] tcb is the task control block for the task
 */
static inline bool rtems_capture_task_initialized( rtems_tcb* tcb ) {
  return ( (tcb->Capture.flags & RTEMS_CAPTURE_INIT_TASK) != 0 );
}

/**
 * @brief Capture get task id.
 *
 * This function returns the task id.
 *
 * @param[in] task The capture task.
 *
 * @retval This function returns the task id.
 */
static inline rtems_id
rtems_capture_task_id (rtems_tcb* tcb)
{
  return tcb->Object.id;
}

/**
 * @brief Capture get task state.
 *
 * This function returns the task state.
 *
 * @param[in] task The capture task.
 *
 * @retval This function returns the task state.
 */
static inline States_Control
rtems_capture_task_state (rtems_tcb* tcb)
{
  if (tcb)
    return tcb->current_state;
  return 0;
}

/**
 * @brief Capture get task name.
 *
 * This function returns the task name.
 *
 * @param[in] task The capture task.
 *
 * @retval This function returns the task name.
 */
static inline rtems_name
rtems_capture_task_name (rtems_tcb* tcb)
{
  rtems_name  name;
  rtems_object_get_classic_name( tcb->Object.id, &name );
  return name;
}

/**
 * @brief Capture get task flags.
 *
 * This function returns the task flags.
 *
 * @param[in] task The capture task.
 *
 * @retval This function returns the task flags.
 */
static inline uint32_t
rtems_capture_task_flags (rtems_tcb* tcb)
{
  return tcb->Capture.flags;
}

/**
 * @brief Capture get task control
 *
 * This function returns the task control if present.
 *
 * @param[in] task The capture task.
 *
 * @retval This function returns the task control if present.
 */
static inline rtems_capture_control_t*
rtems_capture_task_control (rtems_tcb* tcb)
{
  return tcb->Capture.control;
}

/**
 * @brief Capture get task control flags.
 *
 * This function returns the task control flags if a control is present.
 *
 * @param[in] task The capture task.
 *
 * @retval This function returns the task control flags if a control is present.
 */
static inline uint32_t
rtems_capture_task_control_flags (rtems_tcb* tcb)
{
  rtems_capture_control_t*  control = tcb->Capture.control;
  if (!control)
    return 0;
  return control->flags;
}

/**
 * @brief Capture get task start priority.
 *
 * This function returns the tasks start priority. The tracer needs this
 * to track where the task's priority goes.
 *
 * @param[in] task The capture task.
 *
 * @retval This function returns the tasks start priority. The tracer needs this
 * to track where the task's priority goes.
 */
static inline rtems_task_priority
rtems_capture_task_start_priority (rtems_tcb* tcb)
{
  return _RTEMS_tasks_Priority_from_Core(
    tcb->Start.initial_priority
  );
}

/**
 * @brief Capture get task real priority.
 *
 * This function returns the tasks real priority.
 *
 * @param[in] task The capture task.
 *
 * @retval This function returns the tasks real priority.
 */
static inline rtems_task_priority
rtems_capture_task_real_priority (rtems_tcb* tcb)
{
  return tcb->real_priority;
}

/**
 * @brief Capture get task current priority.
 *
 * This function returns the tasks current priority.
 *
 * @param[in] task The capture task.
 *
 * @retval This function returns the tasks current priority.
 */
static inline rtems_task_priority
rtems_capture_task_curr_priority (rtems_tcb* tcb)
{
  return tcb->current_priority;
}

/**
 * @brief Capture get control list.
 *
 * This function returns the head of the list of controls in the
 * capture engine.
 *
 * @retval This function returns the head of the list of controls in the
 * capture engine.
 */
rtems_capture_control_t*
rtems_capture_get_control_list (void);

/**
 * @brief Capture get next capture control.
 *
 * This function returns the pointer to the next control in the list. The
 * pointer NULL terminates the list.
 *
 * @param[in] control the current capture control.
 *
 * @retval This function returns the pointer to the next control in the list. The
 * pointer NULL terminates the list.
 */
static inline rtems_capture_control_t*
rtems_capture_next_control (rtems_capture_control_t* control)
{
  return control->next;
}

/**
 * @brief Capture get capture control id.
 *
 * This function returns the control id.
 *
 * @param[in] control the capture control.
 *
 * @retval This function returns the control id.
 */
static inline rtems_id
rtems_capture_control_id (rtems_capture_control_t* control)
{
  return control->id;
}

/**
 * @brief Capture get capture control name.
 *
 * This function returns the control name.
 *
 * @param[in] control the capture control.
 *
 * @retval This function returns the control name.
 */
static inline rtems_name
rtems_capture_control_name (rtems_capture_control_t* control)
{
  return control->name;
}

/**
 * @brief Capture get capture control flags.
 *
 * This function returns the control flags.
 *
 * @param[in] control the capture control.
 *
 * @retval This function returns the control flags.
 */
static inline uint32_t
rtems_capture_control_flags (rtems_capture_control_t* control)
{
  return control->flags;
}

/**
 * @brief Capture get capture control to triggers.
 *
 * This function returns the task control to triggers.
 *
 * @param[in] control the capture control.
 *
 * @retval This function returns the task control to triggers.
 */
static inline uint32_t
rtems_capture_control_to_triggers (rtems_capture_control_t* control)
{
  return control->to_triggers;
}

/**
 * @brief Capture get capture control from triggers.
 *
 * This function returns the task control from triggers.
 *
 * @param[in] control the capture control.
 *
 * @retval This function returns the task control from triggers.
 */
static inline uint32_t
rtems_capture_control_from_triggers (rtems_capture_control_t* control)
{
  return control->from_triggers;
}

/**
 * @brief Capture get capture control by triggers.
 *
 * This function returns the task control by triggers.
 *
 * @param[in] control the capture control.
 *
 * @retval This function returns the task control by triggers.
 */
static inline uint32_t
rtems_capture_control_all_by_triggers (rtems_capture_control_t* control)
{
  return control->by_triggers;
}

/**
 * @brief Capture get capture control valid by flags.
 *
 * This function returns the control valid BY flags.
 *
 * @param[in] control The capture control.
 * @param[in] slot The slot.
 *
 * @retval This function returns the control valid BY flags.
 */
static inline int
rtems_capture_control_by_valid (rtems_capture_control_t* control, int slot)
{
  return control->by_valid & RTEMS_CAPTURE_CONTROL_FROM_MASK (slot);
}

/**
 * @brief Capture get capture control by task name.
 *
 * This function returns the control @a by task name.
 *
 * @param[in] control The capture control.
 * @param[in] by The by index.
 *
 * @retval This function returns the control @a by task name.
 */
static inline rtems_name
rtems_capture_control_by_name (rtems_capture_control_t* control, int by)
{
  if (by < RTEMS_CAPTURE_TRIGGER_TASKS)
    return control->by[by].name;
  return control->by[0].name;
}

/**
 * @brief Capture get capture control by task id.
 *
 * This function returns the control @a by task id
 *
 * @retval This function returns the control @a by task id.
 */
static inline rtems_id
rtems_capture_control_by_id (rtems_capture_control_t* control, int by)
{
  if (by < RTEMS_CAPTURE_TRIGGER_TASKS)
    return control->by[by].id;
  return control->by[0].id;
}

/**
 * @brief Capture get capture control by task triggers.
 *
 * This function returns the control @a by task triggers.
 *
 * @retval This function returns the control @a by task triggers.
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
 * @brief Capture get capture control count.
 *
 * This function returns the number of controls the capture
 * engine has.
 *
 * @retval This function returns the number of controls the capture
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
/**@}*/

#endif
