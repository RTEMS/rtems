/**
 * @file rtems/rtems/tasks.inl
 *
 *  This file contains the static inline implementation of all inlined
 *  routines in the with RTEMS Tasks Manager.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_TASKS_H
# error "Never use <rtems/rtems/tasks.inl> directly; include <rtems/rtems/tasks.h> instead."
#endif

#ifndef _RTEMS_RTEMS_TASKS_INL
#define _RTEMS_RTEMS_TASKS_INL

/**
 *  @addtogroup ClassicTasks
 *  @{
 */

/**
 *  @brief RTEMS_tasks_Allocate
 *
 *  This function allocates a task control block from
 *  the inactive chain of free task control blocks.
 */
RTEMS_INLINE_ROUTINE Thread_Control *_RTEMS_tasks_Allocate( void )
{
  return (Thread_Control *) _Objects_Allocate( &_RTEMS_tasks_Information );
}

/**
 *  @brief RTEMS_tasks_Free
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
 *  @brief RTEMS_tasks_Priority_to_Core
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
 *  @brief RTEMS_tasks_Priority_is_valid
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

#endif
/* end of include file */
