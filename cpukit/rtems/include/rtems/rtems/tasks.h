/**
 * @file rtems/rtems/tasks.h
 *
 * @defgroup ClassicTasks Tasks
 *
 * @ingroup ClassicRTEMS
 * @brief RTEMS Tasks
 *
 * This include file contains all constants and structures associated
 * with RTEMS tasks. This manager provides a comprehensive set of directives
 * to create, delete, and administer tasks.
 *
 * Directives provided are:
 *
 * - create a task
 * - get an ID of a task
 * - start a task
 * - restart a task
 * - delete a task
 * - suspend a task
 * - resume a task
 * - set a task's priority
 * - change the current task's mode
 * - get a task notepad entry
 * - set a task notepad entry
 * - wake up after interval
 * - wake up when specified
 */

/*
 * COPYRIGHT (c) 1989-2014.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_TASKS_H
#define _RTEMS_RTEMS_TASKS_H

#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/types.h>
#include <rtems/rtems/event.h>
#include <rtems/rtems/asr.h>
#include <rtems/rtems/attr.h>
#include <rtems/rtems/status.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ClassicTasks Tasks
 *
 *  @ingroup ClassicRTEMS
 *
 *  This encapsulates the functionality of the Classic API Task Manager.
 *  This functionality includes task services such as creation, deletion,
 *  delays, suspend/resume, and manipulation of execution mode and priority.
 */
/**@{*/

/**
 *  Constant to be used as the ID of current task
 */
#define RTEMS_SELF                OBJECTS_ID_OF_SELF

/**
 *  This constant is passed to the rtems_task_wake_after directive as the
 *  interval when a task wishes to yield the CPU.
 */
#define RTEMS_YIELD_PROCESSOR WATCHDOG_NO_TIMEOUT

/**
 *  Define the type for an RTEMS API task priority.
 */
typedef Priority_Control rtems_task_priority;

/**
 *  This is the constant used with the rtems_task_set_priority
 *  directive to indicate that the caller wants to obtain its
 *  current priority rather than set it as the name of the
 *  directive indicates.
 */
#define RTEMS_NO_PRIORITY           RTEMS_CURRENT_PRIORITY

/**
 *  This constant is the least valid value for a Classic API
 *  task priority.
 */
#define RTEMS_MINIMUM_PRIORITY      (PRIORITY_MINIMUM + 1)

/**
 *  This constant is the maximum valid value for a Classic API
 *  task priority.
 *
 *  @note This is actually the priority of the IDLE thread so
 *        using this priority will result in having a task
 *        which never executes.  This could be useful if you
 *        want to ensure that a task does not executes during
 *        certain operations such as a system mode change.
 */
#define RTEMS_MAXIMUM_PRIORITY      PRIORITY_MAXIMUM

/**
 *  The following constant is passed to rtems_task_set_priority when the
 *  caller wants to obtain the current priority.
 */
#define RTEMS_CURRENT_PRIORITY      PRIORITY_MINIMUM

/** This is used to indicate the lowest numbered notepad */
#define RTEMS_NOTEPAD_FIRST 0
/** This is used to indicate the notepad location 0. */
#define RTEMS_NOTEPAD_0    0
/** This is used to indicate the notepad location 1. */
#define RTEMS_NOTEPAD_1    1
/** This is used to indicate the notepad location 2. */
#define RTEMS_NOTEPAD_2    2
/** This is used to indicate the notepad location 3. */
#define RTEMS_NOTEPAD_3    3
/** This is used to indicate the notepad location 4. */
#define RTEMS_NOTEPAD_4    4
/** This is used to indicate the notepad location 5. */
#define RTEMS_NOTEPAD_5    5
/** This is used to indicate the notepad location 6. */
#define RTEMS_NOTEPAD_6    6
/** This is used to indicate the notepad location 7. */
#define RTEMS_NOTEPAD_7    7
/** This is used to indicate the notepad location 8. */
#define RTEMS_NOTEPAD_8    8
/** This is used to indicate the notepad location 9. */
#define RTEMS_NOTEPAD_9    9
/** This is used to indicate the notepad location 10. */
#define RTEMS_NOTEPAD_10   10
/** This is used to indicate the notepad location 11. */
#define RTEMS_NOTEPAD_11   11
/** This is used to indicate the notepad location 12. */
#define RTEMS_NOTEPAD_12   12
/** This is used to indicate the notepad location 13. */
#define RTEMS_NOTEPAD_13   13
/** This is used to indicate the notepad location 14. */
#define RTEMS_NOTEPAD_14   14
/** This is used to indicate the notepad location 15. */
#define RTEMS_NOTEPAD_15   15
/** This is used to indicate the highest numbered notepad. */
#define RTEMS_NOTEPAD_LAST RTEMS_NOTEPAD_15

/** This is used to indicate the number of notepads available. */
#define RTEMS_NUMBER_NOTEPADS  (RTEMS_NOTEPAD_LAST+1)

/**
 *  External API name for Thread_Control
 */
typedef Thread_Control rtems_tcb;

/**
 *  The following defines the "return type" of an RTEMS task.
 */
typedef void rtems_task;

/**
 *  The following defines the argument to an RTEMS task.
 */
typedef Thread_Entry_numeric_type rtems_task_argument;

/**
 *  The following defines the type for the entry point of an RTEMS task.
 */
typedef rtems_task ( *rtems_task_entry )(
                      rtems_task_argument
                   );

/**
 *  The following records define the Initialization Tasks Table.
 *  Each entry contains the information required by RTEMS to
 *  create and start a user task automatically at executive
 *  initialization time.
 */
typedef struct {
  /** This is the Initialization Task's name. */
  rtems_name            name;
  /** This is the Initialization Task's stack size. */
  size_t                stack_size;
  /** This is the Initialization Task's priority. */
  rtems_task_priority   initial_priority;
  /** This is the Initialization Task's attributes. */
  rtems_attribute       attribute_set;
  /** This is the Initialization Task's entry point. */
  rtems_task_entry      entry_point;
  /** This is the Initialization Task's initial mode. */
  rtems_mode            mode_set;
  /** This is the Initialization Task's argument. */
  rtems_task_argument	argument;
} rtems_initialization_tasks_table;

/**
 * @brief RTEMS Task Create
 *
 * This routine implements the rtems_task_create directive. The task
 * will have the name name. The attribute_set can be used to indicate
 * that the task will be globally accessible or utilize floating point.
 * The task's stack will be stack_size bytes. The task will begin
 * execution with initial_priority and initial_modes. It returns the
 * id of the created task in ID.
 *
 * @param[in] name is the user defined thread name
 * @param[in] initial_priority is the thread priority
 * @param[in] stack_size is the stack size in bytes
 * @param[in] initial_modes is the initial thread mode
 * @param[in] attribute_set is the thread attributes
 * @param[in] id is the pointer to thread id
 *
 * @retval RTEMS_SUCCESSFUL if successful or error code if unsuccessful
 *             	and *id thread id filled in
 */
rtems_status_code rtems_task_create(
  rtems_name           name,
  rtems_task_priority  initial_priority,
  size_t               stack_size,
  rtems_mode           initial_modes,
  rtems_attribute      attribute_set,
  rtems_id            *id
);

/**
 * @brief RTEMS Task Name to Id
 *
 * This routine implements the rtems_task_ident directive.
 * This directive returns the task ID associated with name.
 * If more than one task is named name, then the task to
 * which the ID belongs is arbitrary. node indicates the
 * extent of the search for the ID of the task named name.
 * The search can be limited to a particular node or allowed to
 * encompass all nodes.
 *
 * @param[in] name is the user defined thread name
 * @param[in] node is(are) the node(s) to be searched
 * @param[in] id is the pointer to thread id
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error. If successful, the id will
 *         be filled in with the thread id.
 */
rtems_status_code rtems_task_ident(
  rtems_name    name,
  uint32_t      node,
  rtems_id     *id
);

/**
 * @brief RTEMS Delete Task
 *
 * This routine implements the rtems_task_delete directive. The
 * task indicated by ID is deleted. The executive halts execution
 * of the thread and frees the thread control block.
 *
 * @param[in] id is the thread id
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error and id is not the requesting thread. Status code is
 *         returned indicating the source of the error. Nothing
 *         is returned if id is the requesting thread (always succeeds).
 */
rtems_status_code rtems_task_delete(
  rtems_id   id
);

/**
 * @brief RTEMS Get Task Node
 *
 * @deprecated Notepads are deprecated and will be removed.
 *
 * This routine implements the rtems_task_get_note directive. The
 * value of the indicated notepad for the task associated with ID
 * is returned in note.
 *
 * @param[in] id is the thread id
 * @param[in] notepad is the notepad number
 * @param[out] note is the pointer to note
 *
 * @retval RTEMS_SUCCESSFUL if successful or error code if unsuccessful
 */
rtems_status_code rtems_task_get_note(
  rtems_id    id,
  uint32_t    notepad,
  uint32_t   *note
) RTEMS_COMPILER_DEPRECATED_ATTRIBUTE;

/**
 * @brief RTEMS Set Task Note
 *
 * @deprecated Notepads are deprecated and will be removed.
 *
 * This routine implements the rtems_task_set_note directive. The
 * value of the indicated notepad for the task associated with ID
 * is returned in note.
 *
 * @param[in] id is the thread id
 * @param[in] notepad is the notepad number
 * @param[in] note is the note value
 *
 * @return This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code rtems_task_set_note(
  rtems_id   id,
  uint32_t   notepad,
  uint32_t   note
) RTEMS_COMPILER_DEPRECATED_ATTRIBUTE;

/**
 * @brief RTEMS Task Mode
 *
 * This routine implements the rtems_task_mode directive. The current
 * values of the modes indicated by mask of the calling task are changed
 * to that indicated in mode_set. The former mode of the task is
 * returned in mode_set.
 *
 * @param[in] mode_set is the new mode
 * @param[in] mask is the mask
 * @param[in] previous_mode_set is the address of previous mode set
 *
 * @retval RTEMS_SUCCESSFUL and previous_mode_set filled in with the
 * previous mode set
 */
rtems_status_code rtems_task_mode(
  rtems_mode  mode_set,
  rtems_mode  mask,
  rtems_mode *previous_mode_set
);

/**
 * @brief RTEMS Task Restart
 *
 * This routine implements the rtems_task_restart directive. The
 * task associated with ID is restarted at its initial entry
 * point with the new argument.
 *
 * @param[in] id is the thread id
 * @param[in] arg is the thread argument
 *
 * @retval RTEMS_SUCCESSFUL if successful or error code if unsuccessful
 */
rtems_status_code rtems_task_restart(
  rtems_id   id,
  uint32_t   arg
);

/**
 * @brief RTEMS Suspend Task
 *
 * This routine implements the rtems_task_suspend directive. The
 * SUSPENDED state is set for task associated with ID. Note that the
 * suspended state can be in addition to other waiting states.
 *
 * @param[in] id is the thread id
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code rtems_task_suspend(
  rtems_id   id
);

/**
 * @brief RTEMS Resume Task
 *
 * This routine implements the rtems_task_resume Directive. The
 * SUSPENDED state is cleared for task associated with ID.
 *
 * @param[in] id is the thread id
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code rtems_task_resume(
  rtems_id   id
);

/**
 * @brief RTEMS Set Task Priority
 *
 * This routine implements the rtems_task_set_priority directive. The
 * current priority of the task associated with ID is set to
 * new_priority. The former priority of that task is returned
 * in old_priority.
 *
 * @param[in] id is the thread to extract
 * @param[in] new_priority is the thread to extract
 * @param[in] old_priority is the thread to extract
 *
 * @retval RTEMS_SUCCESSFUL if successful or error code if unsuccessful and
 * and *old_priority filled in with the previous previous priority
 */
rtems_status_code rtems_task_set_priority(
  rtems_id             id,
  rtems_task_priority  new_priority,
  rtems_task_priority *old_priority
);

/**
 *  @brief RTEMS Start Task
 *
 *  RTEMS Task Manager
 *
 *  This routine implements the rtems_task_start directive.  The
 *  starting execution point of the task associated with ID is
 *  set to entry_point with the initial argument.
 */
rtems_status_code rtems_task_start(
  rtems_id             id,
  rtems_task_entry     entry_point,
  rtems_task_argument  argument
);

/**
 * @brief RTEMS Task Wake When
 *
 * This routine implements the rtems_task_wake_when directive. The
 * calling task is blocked until the current time of day is
 * equal to that indicated by time_buffer.
 *
 * @param[in] time_buffer is the pointer to the time and date structure
 *
 * @retval RTEMS_SUCCESSFUL if successful or error code if unsuccessful
 */
rtems_status_code rtems_task_wake_when(
  rtems_time_of_day *time_buffer
);

/**
 * @brief RTEMS Task Wake After
 *
 * This routine implements the rtems_task_wake_after directive. The
 * calling task is blocked until the indicated number of clock
 * ticks have occurred.
 *
 * @param[in] ticks is the number of ticks to wait
 * @retval RTEMS_SUCCESSFUL
 */
rtems_status_code rtems_task_wake_after(
  rtems_interval  ticks
);

/**
 *  @brief rtems_task_is_suspended
 *
 *  This directive returns a status indicating whether or not
 *  the specified task is suspended.
 */
rtems_status_code rtems_task_is_suspended(
  rtems_id   id
);

#if !defined(RTEMS_SMP)
/**
 *  @brief RTEMS Add Task Variable
 *
 *  @deprecated Task variables are deprecated.
 *
 *  This directive adds a per task variable.
 *
 *  @note This service is not available in SMP configurations.
 */
rtems_status_code rtems_task_variable_add(
  rtems_id  tid,
  void    **ptr,
  void    (*dtor)(void *)
) RTEMS_COMPILER_DEPRECATED_ATTRIBUTE;

/**
 *  @brief Get a per-task variable
 *
 *  @deprecated Task variables are deprecated.
 *
 *  This directive gets the value of a task variable.
 *
 *  @note This service is not available in SMP configurations.
 */
rtems_status_code rtems_task_variable_get(
  rtems_id tid,
  void **ptr,
  void **result
) RTEMS_COMPILER_DEPRECATED_ATTRIBUTE;

/**
 *  @brief RTEMS Delete Task Variable
 *
 *  @deprecated Task variables are deprecated.
 *
 *  This directive removes a per task variable.
 *
 *  @note This service is not available in SMP configurations.
 */
rtems_status_code rtems_task_variable_delete(
  rtems_id  tid,
  void    **ptr
) RTEMS_COMPILER_DEPRECATED_ATTRIBUTE;
#endif

#if defined(__RTEMS_HAVE_SYS_CPUSET_H__)
/**
 * @brief Gets the processor affinity set of a task.
 *
 * @param[in] id Identifier of the task.  Use @ref RTEMS_SELF to select the
 * executing task.
 * @param[in] cpusetsize Size of the specified affinity set buffer in
 * bytes.  This value must be positive.
 * @param[out] cpuset The current processor affinity set of the task.  A set
 * bit in the affinity set means that the task can execute on this processor
 * and a cleared bit means the opposite.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ADDRESS The @a cpuset parameter is @c NULL.
 * @retval RTEMS_INVALID_ID Invalid task identifier.
 * @retval RTEMS_INVALID_NUMBER The affinity set buffer is too small for the
 * current processor affinity set of the task.
 */
rtems_status_code rtems_task_get_affinity(
  rtems_id             id,
  size_t               cpusetsize,
  cpu_set_t           *cpuset
);

/**
 * @brief Sets the processor affinity set of a task.
 *
 * This function will not change the scheduler of the task.  The intersection
 * of the processor affinity set and the set of processors owned by the
 * scheduler of the task must be non-empty.  It is not an error if the
 * processor affinity set contains processors that are not part of the set of
 * processors owned by the scheduler instance of the task.  A task will simply
 * not run under normal circumstances on these processors since the scheduler
 * ignores them.  Some locking protocols may temporarily use processors that
 * are not included in the processor affinity set of the task.  It is also not
 * an error if the processor affinity set contains processors that are not part
 * of the system.
 *
 * @param[in] id Identifier of the task.  Use @ref RTEMS_SELF to select the
 * executing task.
 * @param[in] cpusetsize Size of the specified affinity set buffer in
 * bytes.  This value must be positive.
 * @param[in] cpuset The new processor affinity set for the task.  A set bit in
 * the affinity set means that the task can execute on this processor and a
 * cleared bit means the opposite.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ADDRESS The @a cpuset parameter is @c NULL.
 * @retval RTEMS_INVALID_ID Invalid task identifier.
 * @retval RTEMS_INVALID_NUMBER Invalid processor affinity set.
 */
rtems_status_code rtems_task_set_affinity(
  rtems_id         id,
  size_t           cpusetsize,
  const cpu_set_t *cpuset
);
#endif

/**
 * @brief Gets the scheduler of a task.
 *
 * @param[in] task_id Identifier of the task.  Use @ref RTEMS_SELF to select
 * the executing task.
 * @param[out] scheduler_id Identifier of the scheduler.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ADDRESS The @a scheduler_id parameter is @c NULL.
 * @retval RTEMS_INVALID_ID Invalid task identifier.
 */
rtems_status_code rtems_task_get_scheduler(
  rtems_id  task_id,
  rtems_id *scheduler_id
);

/**
 * @brief Sets the scheduler of a task.
 *
 * The scheduler of a task is initialized to the scheduler of the task that
 * created it.
 *
 * @param[in] task_id Identifier of the task.  Use @ref RTEMS_SELF to select
 * the executing task.
 * @param[in] scheduler_id Identifier of the scheduler.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID Invalid task or scheduler identifier.
 *
 * @see rtems_scheduler_ident().
 */
rtems_status_code rtems_task_set_scheduler(
  rtems_id task_id,
  rtems_id scheduler_id
);

/**
 *  @brief RTEMS Get Self Task Id
 *
 *  This directive returns the ID of the currently executing task.
 */
rtems_id rtems_task_self(void);

/**
 * @brief Identifies a scheduler by its name.
 *
 * The scheduler name is determined by the scheduler configuration.
 *
 * @param[in] name The scheduler name.
 * @param[out] id The scheduler identifier associated with the name.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ADDRESS The @a id parameter is @c NULL.
 * @retval RTEMS_INVALID_NAME Invalid scheduler name.
 * @retval RTEMS_UNSATISFIED A scheduler with this name exists, but the
 * processor set of this scheduler is empty.
 */
rtems_status_code rtems_scheduler_ident(
  rtems_name  name,
  rtems_id   *id
);

#if defined(__RTEMS_HAVE_SYS_CPUSET_H__)
/**
 * @brief Gets the set of processors owned by the scheduler.
 *
 * @param[in] scheduler_id Identifier of the scheduler.
 * @param[in] cpusetsize Size of the specified processor set buffer in
 * bytes.  This value must be positive.
 * @param[out] cpuset The processor set owned by the scheduler.  A set bit in
 * the processor set means that this processor is owned by the scheduler and a
 * cleared bit means the opposite.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ADDRESS The @a cpuset parameter is @c NULL.
 * @retval RTEMS_INVALID_ID Invalid scheduler identifier.
 * @retval RTEMS_INVALID_NUMBER The processor set buffer is too small for the
 * set of processors owned by the scheduler.
 */
rtems_status_code rtems_scheduler_get_processor_set(
  rtems_id   scheduler_id,
  size_t     cpusetsize,
  cpu_set_t *cpuset
);
#endif

/**@}*/

/**
 *  This is the API specific information required by each thread for
 *  the RTEMS API to function correctly.
 *
 *  @note Notepads must be the last entry in the structure and memory
 *        will be taken away from this structure when allocated if
 *        notespads are disabled by the application configuration.
 */
typedef struct {
  /** This field contains the event control for this task. */
  Event_Control            Event;
  /** This field contains the system event control for this task. */
  Event_Control            System_event;
  /** This field contains the Classic API Signal information for this task. */
  ASR_Information          Signal;

  /**
   * @brief Signal post-switch action in case signals are pending.
   */
  Thread_Action            Signal_action;

  /**
   *  This field contains the notepads for this task.
   * 
   *  @deprecated Notepads are deprecated and will be removed.
   *
   *  @note MUST BE LAST ENTRY.
   */
  uint32_t Notepads[ RTEMS_ZERO_LENGTH_ARRAY ] RTEMS_COMPILER_DEPRECATED_ATTRIBUTE;
}  RTEMS_API_Control;

/**
 *  When the user configures a set of Classic API initialization tasks,
 *  This variable will point to the method used to initialize them.
 *
 *  @note It is instantiated and initialized by confdefs.h based upon
 *        application requirements.
 */
extern void (*_RTEMS_tasks_Initialize_user_tasks_p)(void);

/**
 *  @brief _RTEMS_tasks_Initialize_user_tasks_body
 *
 *  This routine creates and starts all configured user
 *  initialization threads.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  RTEMS Task Manager
 */

extern void _RTEMS_tasks_Initialize_user_tasks_body( void );

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
