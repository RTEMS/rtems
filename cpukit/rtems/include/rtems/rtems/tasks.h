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
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __RTEMS_RTEMS_TASKS_h
#define __RTEMS_RTEMS_TASKS_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <rtems/message.h>
#include <rtems/object.h>
#include <rtems/part.h>
#include <rtems/region.h>
#include <rtems/sem.h>
#include <rtems/states.h>
#include <rtems/thread.h>
#include <rtems/threadq.h>
#include <rtems/types.h>

/*
 *  Constant to be used as the ID of current task
 */

#define RTEMS_SELF                OBJECTS_ID_OF_SELF

/*
 *  This constant is passed to the rtems_task_wake_after directive as the
 *  interval when a task wishes to yield the CPU.
 */

#define RTEMS_YIELD_PROCESSOR RTEMS_NO_TIMEOUT

/*
 *  The following defines the information control block used to
 *  manage this class of objects.
 */

EXTERN Objects_Information _RTEMS_tasks_Information;

/*
 *  _RTEMS_tasks_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine initializes all Task Manager related data structures.
 */
 
void _RTEMS_tasks_Manager_initialization(
  unsigned32   maximum_tasks
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
  rtems_interval ticks
);

/*
 *  _RTEMS_tasks_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a task control block from
 *  the inactive chain of free task control blocks.
 */

STATIC INLINE Thread_Control *_RTEMS_tasks_Allocate( void );

/*
 *  _RTEMS_tasks_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a task control block to the
 *  inactive chain of free task control blocks.

 */

STATIC INLINE void _RTEMS_tasks_Free (
  Thread_Control *the_task
);

/*
 *  _RTEMS_tasks_Cancel_wait
 *
 *  DESCRIPTION:
 *
 *  This routine unblocks the_thread and cancels any timers
 *  which the_thread has active.
 */

STATIC INLINE void _RTEMS_tasks_Cancel_wait(
  Thread_Control *the_thread
);

/*
 *  _RTEMS_Tasks_Priority_to_Core
 *
 *  DESCRIPTION:
 *
 *  This function converts an RTEMS API priority into a core priority.
 */
 
STATIC INLINE Priority_Control _RTEMS_Tasks_Priority_to_Core(
  rtems_task_priority   priority
);

#include <rtems/tasks.inl>
#include <rtems/taskmp.h>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
