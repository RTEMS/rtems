/*  tasks.inl
 *
 *  This file contains the static inline implementation of all inlined
 *  routines in the with RTEMS Tasks Manager.
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

STATIC INLINE Thread_Control *_RTEMS_tasks_Allocate( void )
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

STATIC INLINE void _RTEMS_tasks_Free (
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
 
STATIC INLINE Priority_Control _RTEMS_tasks_Priority_to_Core(
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
 
STATIC INLINE boolean _RTEMS_tasks_Priority_is_valid (
  rtems_task_priority the_priority
)
{
  return (  ( the_priority >= RTEMS_MINIMUM_PRIORITY ) &&
            ( the_priority <= RTEMS_MAXIMUM_PRIORITY ) );
}

#endif
/* end of include file */
