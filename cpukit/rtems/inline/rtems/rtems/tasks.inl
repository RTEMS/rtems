/*  tasks.inl
 *
 *  This file contains the static inline implementation of all inlined
 *  routines in the with RTEMS Tasks Manager.
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

#ifndef __RTEMS_TASKS_inl
#define __RTEMS_TASKS_inl

/*PAGE
 *
 *  _RTEMS_tasks_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a task control block from
 *  the inactive chain of free task control blocks.
 */

RTEMS_INLINE_ROUTINE Thread_Control *_RTEMS_tasks_Allocate( void )
{
  return (Thread_Control *) _Objects_Allocate( &_RTEMS_tasks_Information );
}

/*PAGE
 *
 *  _RTEMS_tasks_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a task control block to the
 *  inactive chain of free task control blocks.
 */

RTEMS_INLINE_ROUTINE void _RTEMS_tasks_Free (
  Thread_Control *the_task
)
{
  _Objects_Free( 
    _Objects_Get_information( the_task->Object.id ),
    &the_task->Object
  );
}

/*PAGE
 *
 *  _RTEMS_tasks_Priority_to_Core
 *
 *  DESCRIPTION:
 *
 *  This function converts an RTEMS API priority into a core priority.
 */
 
RTEMS_INLINE_ROUTINE Priority_Control _RTEMS_tasks_Priority_to_Core(
  rtems_task_priority   priority
)
{
  return (Priority_Control) priority;
}

/*PAGE
 *
 *  _RTEMS_tasks_Priority_is_valid
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_priority is a valid user task priority
 *  and FALSE otherwise.
 */
 
RTEMS_INLINE_ROUTINE boolean _RTEMS_tasks_Priority_is_valid (
  rtems_task_priority the_priority
)
{
  return (  ( the_priority >= RTEMS_MINIMUM_PRIORITY ) &&
            ( the_priority <= RTEMS_MAXIMUM_PRIORITY ) );
}

#endif
/* end of include file */
