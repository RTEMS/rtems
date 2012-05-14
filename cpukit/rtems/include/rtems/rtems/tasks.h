/**
 * @file rtems/rtems/tasks.h
 *
 *  This include file contains all constants and structures associated
 *  with RTEMS tasks.  This manager provides a comprehensive set of directives
 *  to create, delete, and administer tasks.
 *
 *  Directives provided are:
 *
 *     - create a task
 *     - get an ID of a task
 *     - start a task
 *     - restart a task
 *     - delete a task
 *     - suspend a task
 *     - resume a task
 *     - set a task's priority
 *     - change the current task's mode
 *     - get a task notepad entry
 *     - set a task notepad entry
 *     - wake up after interval
 *     - wake up when specified
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_TASKS_H
#define _RTEMS_RTEMS_TASKS_H

#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/types.h>
#include <rtems/rtems/eventset.h>
#include <rtems/rtems/asr.h>
#include <rtems/rtems/attr.h>
#include <rtems/rtems/status.h>

/**
 *  This constant is defined to extern most of the time when using
 *  this header file.  However by defining it to nothing, the data
 *  declared in this header file can be instantiated.  This is done
 *  in a single per manager file.
 */
#ifndef RTEMS_TASKS_EXTERN
#define RTEMS_TASKS_EXTERN extern
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

#ifdef __cplusplus
extern "C" {
#endif

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
 *  This is the API specific information required by each thread for
 *  the RTEMS API to function correctly.
 *
 *  @note Notepads must be the last entry in the structure and memory
 *        will be taken away from this structure when allocated if
 *        notespads are disabled by the application configuration.
 */
typedef struct {
  /** This field contains the pending events for this task. */
  rtems_event_set          pending_events;
  /** This field contains the event wait condition for this task. */
  rtems_event_set          event_condition;
  /** This field contains the Classic API Signal information for this task. */
  ASR_Information          Signal;
  /**
   *  This field contains the notepads for this task.
   *
   *  @note MUST BE LAST ENTRY.
   */
  uint32_t                 Notepads[ RTEMS_NUMBER_NOTEPADS ];
}  RTEMS_API_Control;

/**
 *  The following instantiates the information control block used to
 *  manage this class of objects.
 */
RTEMS_TASKS_EXTERN Objects_Information _RTEMS_tasks_Information;

/**
 *  When the user configures a set of Classic API initialization tasks,
 *  This variable will point to the method used to initialize them.
 *
 *  @note It is instantiated and initialized by confdefs.h based upon
 *        application requirements.
 */
extern void (*_RTEMS_tasks_Initialize_user_tasks_p)(void);

/**
 *  @brief _RTEMS_tasks_Manager_initialization
 *
 *  This routine initializes all Task Manager related data structures.
 */
void _RTEMS_tasks_Manager_initialization(void);

/**
 *  @brief rtems_task_create
 *
 *  This routine implements the rtems_task_create directive.  The task
 *  will have the name name.  The attribute_set can be used to indicate
 *  that the task will be globally accessible or utilize floating point.
 *  The task's stack will be stack_size bytes.   The task will begin
 *  execution with initial_priority and initial_modes.  It returns the
 *  id of the created task in ID.
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
 *  @brief rtems_task_ident
 *
 *  This routine implements the rtems_task_ident directive.
 *  This directive returns the task ID associated with name.
 *  If more than one task is named name, then the task to
 *  which the ID belongs is arbitrary.  node indicates the
 *  extent of the search for the ID of the task named name.
 *  The search can be limited to a particular node or allowed to
 *  encompass all nodes.
 */
rtems_status_code rtems_task_ident(
  rtems_name    name,
  uint32_t      node,
  rtems_id     *id
);

/**
 *  @brief rtems_task_delete
 *
 *  This routine implements the rtems_task_delete directive.  The
 *  task indicated by ID is deleted.
 */
rtems_status_code rtems_task_delete(
  rtems_id   id
);

/**
 *  @brief rtems_task_get_note
 *
 *  This routine implements the rtems_task_get_note directive.  The
 *  value of the indicated notepad for the task associated with ID
 *  is returned in note.
 */
rtems_status_code rtems_task_get_note(
  rtems_id    id,
  uint32_t    notepad,
  uint32_t   *note
);

/**
 *  @brief rtems_task_set_note
 *
 *  This routine implements the rtems_task_set_note directive.  The
 *  value of the indicated notepad for the task associated with ID
 *  is returned in note.
 */
rtems_status_code rtems_task_set_note(
  rtems_id   id,
  uint32_t   notepad,
  uint32_t   note
);

/**
 *  @brief rtems_task_mode
 *
 *  This routine implements the rtems_task_mode directive.  The current
 *  values of the modes indicated by mask of the calling task are changed
 *  to that indicated in mode_set.  The former mode of the task is
 *  returned in mode_set.
 */
rtems_status_code rtems_task_mode(
  rtems_mode  mode_set,
  rtems_mode  mask,
  rtems_mode *previous_mode_set
);

/**
 *  @brief rtems_task_restart
 *
 *  This routine implements the rtems_task_restart directive.  The
 *  task associated with ID is restarted at its initial entry
 *  point with the new argument.
 */
rtems_status_code rtems_task_restart(
  rtems_id   id,
  uint32_t   arg
);

/**
 *  @brief rtems_task_suspend
 *
 *  This routine implements the rtems_task_suspend directive.  The
 *  SUSPENDED state is set for task associated with ID.
 */
rtems_status_code rtems_task_suspend(
  rtems_id   id
);

/**
 *  @brief rtems_task_resume
 *
 *  This routine implements the rtems_task_resume Directive.  The
 *  SUSPENDED state is cleared for task associated with ID.
 */
rtems_status_code rtems_task_resume(
  rtems_id   id
);

/**
 *  @brief rtems_task_set_priority
 *
 *  This routine implements the rtems_task_set_priority directive.  The
 *  current priority of the task associated with ID is set to
 *  new_priority.  The former priority of that task is returned
 *  in old_priority.
 */
rtems_status_code rtems_task_set_priority(
  rtems_id             id,
  rtems_task_priority  new_priority,
  rtems_task_priority *old_priority
);

/**
 *  @brief rtems_task_start
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
 *  @brief rtems_task_wake_when
 *
 *  This routine implements the rtems_task_wake_when directive.  The
 *  calling task is blocked until the current time of day is
 *  equal to that indicated by time_buffer.
 */
rtems_status_code rtems_task_wake_when(
  rtems_time_of_day *time_buffer
);

/**
 *  @brief rtems_task_wake_after
 *
 *  This routine implements the rtems_task_wake_after directive.  The
 *  calling task is blocked until the indicated number of clock
 *  ticks have occurred.
 */
rtems_status_code rtems_task_wake_after(
  rtems_interval  ticks
);

/**
 *  @brief rtems_task_is_suspended

 *  This directive returns a status indicating whether or not
 *  the specified task is suspended.
 */
rtems_status_code rtems_task_is_suspended(
  rtems_id   id
);

/**
 *  @brief rtems_task_variable_add
 *
 *  This directive adds a per task variable.
 */
rtems_status_code rtems_task_variable_add(
  rtems_id  tid,
  void    **ptr,
  void    (*dtor)(void *)
);

/**
 *  @brief rtems_task_variable_get
 *
 *  This directive gets the value of a task variable.
 */
rtems_status_code rtems_task_variable_get(
  rtems_id tid,
  void **ptr,
  void **result
);

/**
 *  @brief rtems_task_variable_delete
 *
 *  This directive removes a per task variable.
 */
rtems_status_code rtems_task_variable_delete(
  rtems_id  tid,
  void    **ptr
);

/**
 *  @brief rtems_task_self
 *
 *  This directive returns the ID of the currently executing task.
 */
rtems_id rtems_task_self(void);

/**
 *  @brief _RTEMS_tasks_Initialize_user_tasks
 *
 *  This routine creates and starts all configured user
 *  initialization threads.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 */
void _RTEMS_tasks_Initialize_user_tasks( void );

/**
 *  @brief _RTEMS_Tasks_Invoke_task_variable_dtor
 *
 *  This routine invokes the optional user provided destructor on the
 *  task variable and frees the memory for the task variable.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 */
void _RTEMS_Tasks_Invoke_task_variable_dtor(
  Thread_Control        *the_thread,
  rtems_task_variable_t *tvp
);

/**
 *  @brief _RTEMS_tasks_Initialize_user_tasks_body
 *
 *  This routine creates and starts all configured user
 *  initialization threads.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 */

extern void _RTEMS_tasks_Initialize_user_tasks_body( void );

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/tasks.inl>
#endif
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/taskmp.h>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
