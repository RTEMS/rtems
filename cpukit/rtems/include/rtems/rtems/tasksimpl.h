/**
 * @file
 *
 * @ingroup ClassicTasksImpl
 *
 * @brief Classic Tasks Manager Implementation
 */

/*  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_TASKSIMPL_H
#define _RTEMS_RTEMS_TASKSIMPL_H

#include <rtems/rtems/tasks.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/threadimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicTasksImpl Classic Tasks Manager Implementation
 *
 * @ingroup ClassicTasks
 *
 * @{
 */

/**
 *  @brief Instantiate RTEMS Classic API Tasks Data
 *
 *  This constant is defined to extern most of the time when using
 *  this header file.  However by defining it to nothing, the data
 *  declared in this header file can be instantiated.  This is done
 *  in a single per manager file.
 */
#ifndef RTEMS_TASKS_EXTERN
#define RTEMS_TASKS_EXTERN extern
#endif

/**
 *  The following instantiates the information control block used to
 *  manage this class of objects.
 */
RTEMS_TASKS_EXTERN Objects_Information _RTEMS_tasks_Information;

/**
 *  @brief RTEMS Task Manager Initialization
 *
 *  This routine initializes all Task Manager related data structures.
 */
void _RTEMS_tasks_Manager_initialization(void);

/**
 *  @brief RTEMS User Task Initialization
 *
 *  This routine creates and starts all configured user
 *  initialization threads.
 */
void _RTEMS_tasks_Initialize_user_tasks( void );

#if !defined(RTEMS_SMP)
/**
 *  @brief RTEMS Tasks Invoke Task Variable Destructor
 *
 *  @deprecated Task variables are deprecated.
 *
 *  This routine invokes the optional user provided destructor on the
 *  task variable and frees the memory for the task variable.
 */
void _RTEMS_Tasks_Invoke_task_variable_dtor(
  Thread_Control        *the_thread,
  rtems_task_variable_t *tvp
) RTEMS_COMPILER_DEPRECATED_ATTRIBUTE;
#endif

RTEMS_INLINE_ROUTINE Thread_Control *_RTEMS_tasks_Allocate(void)
{
  _Objects_Allocator_lock();

  _Thread_Kill_zombies();

  return (Thread_Control *)
    _Objects_Allocate_unprotected( &_RTEMS_tasks_Information );
}

/**
 *  @brief Frees a task control block.
 *
 *  This routine frees a task control block to the
 *  inactive chain of free task control blocks.
 */
RTEMS_INLINE_ROUTINE void _RTEMS_tasks_Free (
  Thread_Control *the_task
)
{
  _Objects_Free(
    _Objects_Get_information_id( the_task->Object.id ),
    &the_task->Object
  );
}

/**
 *  @brief Converts an RTEMS API priority into a core priority.
 *
 *  This function converts an RTEMS API priority into a core priority.
 */
RTEMS_INLINE_ROUTINE Priority_Control _RTEMS_tasks_Priority_to_Core(
  rtems_task_priority   priority
)
{
  return (Priority_Control) priority;
}

/**
 *  @brief Converts a core priority into an RTEMS API priority.
 *
 *  This function converts a core priority into an RTEMS API priority.
 */
RTEMS_INLINE_ROUTINE rtems_task_priority _RTEMS_tasks_Priority_from_Core (
  Priority_Control priority
)
{
  return (rtems_task_priority) priority;
}

/**
 *  @brief Checks whether the priority is a valid user task.
 *
 *  This function returns TRUE if the_priority is a valid user task priority
 *  and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _RTEMS_tasks_Priority_is_valid (
  rtems_task_priority the_priority
)
{
  return (  ( the_priority >= RTEMS_MINIMUM_PRIORITY ) &&
            ( the_priority <= RTEMS_MAXIMUM_PRIORITY ) );
}

/**@}*/

#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/taskmp.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
