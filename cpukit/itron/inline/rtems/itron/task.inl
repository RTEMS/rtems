/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __ITRON_TASK_inl_
#define __ITRON_TASK_inl_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  _ITRON_Task_Allocate
 *
 *  DESCRIPTION:
 *
 *  This routine allocates the task associated with the specified
 *  task ID from the pool of inactive tasks.
 *
 *  Input parameters:
 *    tskid   - id of task to allocate
 *    status  - pointer to status variable
 *
 *  Output parameters:
 *    returns - pointer to the task control block
 *    *status - status
 */

RTEMS_INLINE_ROUTINE Thread_Control *_ITRON_Task_Allocate(
  ID   tskid
)
{
  return (Thread_Control *)_ITRON_Objects_Allocate_by_index(
    &_ITRON_Task_Information,
    tskid,
    sizeof(Thread_Control)
  );
}

/*
 *  _ITRON_Task_Clarify_allocation_id_error
 *
 *  This function is invoked when an object allocation ID error
 *  occurs to determine the specific ITRON error code to return.
 */

#define _ITRON_Task_Clarify_allocation_id_error( _id ) \
  _ITRON_Objects_Clarify_allocation_id_error( \
      &_ITRON_Task_Information, (_id) )

/*
 *  _ITRON_Task_Clarify_get_id_error
 *
 *  This function is invoked when an object get ID error
 *  occurs to determine the specific ITRON error code to return.
 */

#define _ITRON_Task_Clarify_get_id_error( _id ) \
 _ITRON_Objects_Clarify_get_id_error( &_ITRON_Task_Information, (_id) )

/*
 *  _ITRON_Task_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a task control block to the
 *  inactive chain of free task control blocks.
 *
 *  Input parameters:
 *    the_task - pointer to task control block
 *
 *  Output parameters: NONE
 */

RTEMS_INLINE_ROUTINE void _ITRON_Task_Free (
  Thread_Control *the_task
)
{
  _ITRON_Objects_Free( &_ITRON_Task_Information, &the_task->Object );
}

/*PAGE
 *
 *  _ITRON_Task_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps task IDs to task control blocks.
 *  If ID corresponds to a local task, then it returns
 *  the_task control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the task ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_task is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_task is undefined.
 *
 *  Input parameters:
 *    id            - ITRON task ID.
 *    the_location  - pointer to a location variable
 *
 *  Output parameters:
 *    *the_location  - location of the object
 */

RTEMS_INLINE_ROUTINE Thread_Control *_ITRON_Task_Get (
  ID                 id,
  Objects_Locations *location
)
{ 
  if ( id == 0 ) {
    _Thread_Disable_dispatch();
    *location = OBJECTS_LOCAL;
    return _Thread_Executing;
  }

  return (Thread_Control *)
    _ITRON_Objects_Get( &_ITRON_Task_Information, id, location );
}

/*PAGE
 *
 *  _ITRON_Task_Is_null
 *
 *  This function returns TRUE if the_task is NULL and FALSE otherwise.
 *
 *  Input parameters:
 *    the_task - pointer to task control block
 *
 *  Output parameters:
 *    TRUE  - if the_task is NULL
 *    FALSE - otherwise
 */

RTEMS_INLINE_ROUTINE boolean _ITRON_Task_Is_null (
  Thread_Control *the_task
)
{
  return ( the_task == NULL );
}

/*  
 *  XXX insert inline routines here
 */ 

/*PAGE
 *
 *  _ITRON_tasks_Priority_to_Core
 */
 
RTEMS_INLINE_ROUTINE _ITRON_Task_Priority_to_Core( 
  PRI   ITRON_priority 
) 
{  
  return (Priority_Control) ITRON_priority;
}

/*PAGE
 *
 *  _ITRON_tasks_Core_to_Priority
 */
 
RTEMS_INLINE_ROUTINE _ITRON_Task_Core_to_Priority( 
  Priority_Control  core_priority 
) 
{  
  return (PRI) core_priority;
}


#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
