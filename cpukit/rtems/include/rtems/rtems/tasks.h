/*  tasks.h
 *
 *  This include file contains all constants and structures associated
 *  with RTEMS tasks.  This manager provides a comprehensive set of directives
 *  to create, delete, and administer tasks.
 *
 *  Directives provided are:
 *
 *     + create a task
 *     + get an ID of a task
 *     + start a task
 *     + restart a task
 *     + delete a task
 *     + suspend a task
 *     + resume a task
 *     + set a task's priority
 *     + change the current task's mode
 *     + get a task notepad entry
 *     + set a task notepad entry
 *     + wake up after interval
 *     + wake up when specified
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __RTEMS_RTEMS_TASKS_h
#define __RTEMS_RTEMS_TASKS_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/types.h>
#include <rtems/rtems/eventset.h>
#include <rtems/rtems/asr.h>
#include <rtems/rtems/attr.h>
#include <rtems/rtems/status.h>

/*
 *  Constant to be used as the ID of current task
 */

#define RTEMS_SELF                OBJECTS_ID_OF_SELF

/*
 *  This constant is passed to the rtems_task_wake_after directive as the
 *  interval when a task wishes to yield the CPU.
 */

#define RTEMS_YIELD_PROCESSOR WATCHDOG_NO_TIMEOUT

/*
 *  Define the type for an RTEMS API task priority.
 */

typedef Priority_Control rtems_task_priority;

#define RTEMS_NO_PRIORITY           RTEMS_CURRENT_PRIORITY

#define RTEMS_MINIMUM_PRIORITY      (PRIORITY_MINIMUM + 1)
#define RTEMS_MAXIMUM_PRIORITY      PRIORITY_MAXIMUM

/*
 *  The following constant is passed to rtems_task_set_priority when the
 *  caller wants to obtain the current priority.
 */

#define RTEMS_CURRENT_PRIORITY      PRIORITY_MINIMUM   

/*
 *  Notepads constants (indices into notepad array)
 */
 
#define RTEMS_NOTEPAD_FIRST 0             /* lowest numbered notepad */
#define RTEMS_NOTEPAD_0    0              /* notepad location 0  */
#define RTEMS_NOTEPAD_1    1              /* notepad location 1  */
#define RTEMS_NOTEPAD_2    2              /* notepad location 2  */
#define RTEMS_NOTEPAD_3    3              /* notepad location 3  */
#define RTEMS_NOTEPAD_4    4              /* notepad location 4  */
#define RTEMS_NOTEPAD_5    5              /* notepad location 5  */
#define RTEMS_NOTEPAD_6    6              /* notepad location 6  */
#define RTEMS_NOTEPAD_7    7              /* notepad location 7  */
#define RTEMS_NOTEPAD_8    8              /* notepad location 8  */
#define RTEMS_NOTEPAD_9    9              /* notepad location 9  */
#define RTEMS_NOTEPAD_10   10             /* notepad location 10 */
#define RTEMS_NOTEPAD_11   11             /* notepad location 11 */
#define RTEMS_NOTEPAD_12   12             /* notepad location 12 */
#define RTEMS_NOTEPAD_13   13             /* notepad location 13 */
#define RTEMS_NOTEPAD_14   14             /* notepad location 14 */
#define RTEMS_NOTEPAD_15   15             /* notepad location 15 */
#define RTEMS_NOTEPAD_LAST RTEMS_NOTEPAD_15     /* highest numbered notepad */
 
#define RTEMS_NUMBER_NOTEPADS  (RTEMS_NOTEPAD_LAST+1)

/*
 *  External API name for Thread_Control
 */

typedef Thread_Control rtems_tcb;

/*
 *  The following defines the "return type" of an RTEMS task.
 */
 
typedef void rtems_task;

/*
 *  The following defines the argument to an RTEMS task.
 */
 
typedef unsigned32 rtems_task_argument;

/*
 *  The following defines the type for the entry point of an RTEMS task.
 */
 
typedef rtems_task ( *rtems_task_entry )(
                      rtems_task_argument
                   );
 
/*
 *  The following records define the Initialization Tasks Table.
 *  Each entry contains the information required by RTEMS to
 *  create and start a user task automatically at executive
 *  initialization time.
 */
 
typedef struct {
  rtems_name            name;              /* task name */
  unsigned32            stack_size;        /* task stack size */
  rtems_task_priority   initial_priority;  /* task priority */
  rtems_attribute       attribute_set;     /* task attributes */
  rtems_task_entry      entry_point;       /* task entry point */
  rtems_mode            mode_set;          /* task initial mode */
  unsigned32            argument;          /* task argument */
} rtems_initialization_tasks_table;

/*
 *  This is the API specific information required by each thread for
 *  the RTEMS API to function correctly.
 */

 
typedef struct {
  unsigned32               Notepads[ RTEMS_NUMBER_NOTEPADS ];
  rtems_event_set          pending_events;
  rtems_event_set          event_condition;
  ASR_Information          Signal;
}  RTEMS_API_Control;

/*
 *  The following defines the information control block used to
 *  manage this class of objects.
 */

RTEMS_EXTERN Objects_Information _RTEMS_tasks_Information;

/*
 *  These are used to manage the user initialization tasks.
 */

RTEMS_EXTERN rtems_initialization_tasks_table 
               *_RTEMS_tasks_User_initialization_tasks;
RTEMS_EXTERN unsigned32   _RTEMS_tasks_Number_of_initialization_tasks;

/*
 *  _RTEMS_tasks_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine initializes all Task Manager related data structures.
 */
 
void _RTEMS_tasks_Manager_initialization(
  unsigned32                        maximum_tasks,
  unsigned32                        number_of_initialization_tasks,
  rtems_initialization_tasks_table *user_tasks
);

/*
 *  rtems_task_create
 *
 *  DESCRIPTION:
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
  unsigned32           stack_size,
  rtems_mode           initial_modes,
  rtems_attribute      attribute_set,
  Objects_Id          *id
);

/*
 *  rtems_task_ident
 *
 *  DESCRIPTION:
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
  unsigned32    node,
  Objects_Id   *id
);

/*
 *  rtems_task_delete
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_task_delete directive.  The
 *  task indicated by ID is deleted.
 */

rtems_status_code rtems_task_delete(
  Objects_Id id
);

/*
 *  rtems_task_get_note
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_task_get_note directive.  The
 *  value of the indicated notepad for the task associated with ID
 *  is returned in note.
 */

rtems_status_code rtems_task_get_note(
  Objects_Id  id,
  unsigned32  notepad,
  unsigned32 *note
);

/*
 *  rtems_task_set_note
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_task_set_note directive.  The
 *  value of the indicated notepad for the task associated with ID
 *  is returned in note.
 */

rtems_status_code rtems_task_set_note(
  Objects_Id id,
  unsigned32 notepad,
  unsigned32 note
);

/*
 *  rtems_task_mode
 *
 *  DESCRIPTION:
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

/*
 *  rtems_task_restart
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_task_restart directive.  The
 *  task associated with ID is restarted at its initial entry
 *  point with the new argument.
 */

rtems_status_code rtems_task_restart(
  Objects_Id id,
  unsigned32 arg
);

/*
 *  rtems_task_suspend
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_task_suspend directive.  The
 *  SUSPENDED state is set for task associated with ID.
 */

rtems_status_code rtems_task_suspend(
  Objects_Id id
);

/*
 *  rtems_task_resume
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_task_resume Directive.  The
 *  SUSPENDED state is cleared for task associated with ID.
 */

rtems_status_code rtems_task_resume(
  Objects_Id id
);

/*
 *  rtems_task_set_priority
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_task_set_priority directive.  The
 *  current priority of the task associated with ID is set to
 *  new_priority.  The former priority of that task is returned
 *  in old_priority.
 */

rtems_status_code rtems_task_set_priority(
  Objects_Id           id,
  rtems_task_priority  new_priority,
  rtems_task_priority *old_priority
);

/*
 *  rtems_task_start
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_task_start directive.  The
 *  starting execution point of the task associated with ID is
 *  set to entry_point with the initial argument.
 */

rtems_status_code rtems_task_start(
  Objects_Id   id,
  rtems_task_entry entry_point,
  unsigned32   argument
);

/*
 *  rtems_task_wake_when
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_task_wake_when directive.  The
 *  calling task is blocked until the current time of day is
 *  equal to that indicated by time_buffer.
 */

rtems_status_code rtems_task_wake_when(
  rtems_time_of_day *time_buffer
);

/*
 *  rtems_task_wake_after
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_task_wake_after directive.  The
 *  calling task is blocked until the indicated number of clock
 *  ticks have occurred.
 */

rtems_status_code rtems_task_wake_after(
  rtems_interval  ticks
);

/*
 *  rtems_task_is_suspended
 *
 *  This directive returns a status indicating whether or not
 *  the specified task is suspended.
 */

rtems_status_code rtems_task_is_suspended(
  Objects_Id id
);

/*
 *  rtems_task_variable_add
 *
 *  This directive adds a per task variable.
 */

rtems_status_code rtems_task_variable_add(
  rtems_id  tid,
  void    **ptr,
  void    (*dtor)(void *)
);

/*
 *  rtems_task_variable_get
 *
 *  This directive gets the value of a task variable.
 */

rtems_status_code rtems_task_variable_get(
  rtems_id tid,
  void **ptr,
  void **result
);

/*
 *  rtems_task_variable_delete
 *
 *  This directive removes a per task variable.
 */

rtems_status_code rtems_task_variable_delete(
  rtems_id  tid,
  void    **ptr
);

/*
 *  _RTEMS_tasks_Initialize_user_tasks
 *
 *  This routine creates and starts all configured user
 *  initialzation threads.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 */
 
void _RTEMS_tasks_Initialize_user_tasks( void );

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/tasks.inl>
#endif
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/taskmp.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
