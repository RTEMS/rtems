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
 */

#define _ITRON_Task_Allocate( _tskid ) \
  (Thread_Control *)_ITRON_Objects_Allocate_by_index( \
    &_ITRON_Task_Information, \
    (_tskid), \
    sizeof(Thread_Control) \
  )

/*
 *  _ITRON_Task_Clarify_allocation_id_error
 */

#define _ITRON_Task_Clarify_allocation_id_error( _id ) \
  _ITRON_Objects_Clarify_allocation_id_error( \
      &_ITRON_Task_Information, (_id) )

/*
 *  _ITRON_Task_Clarify_get_id_error
 */

#define _ITRON_Task_Clarify_get_id_error( _id ) \
 _ITRON_Objects_Clarify_get_id_error( &_ITRON_Task_Information, (_id) )

/*
 *  _ITRON_Task_Free
 */

#define _ITRON_Task_Free( _the_task ) \
  _ITRON_Objects_Free( &_ITRON_Task_Information, &(_the_task)->Object )

/*PAGE
 *
 *  _ITRON_Task_Get
 */

/* XXX fix me */
static Thread_Control *_ITRON_Task_Get (
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
 */

#define _ITRON_Task_Is_null( _the_task ) \
  ( (_the_task) == NULL )

/*PAGE
 *
 *  _ITRON_tasks_Priority_to_Core
 */
 
#define _ITRON_Task_Priority_to_Core( _ITRON_priority ) \
  ((Priority_Control) (_ITRON_priority))

/*PAGE
 *
 *  _ITRON_tasks_Core_to_Priority
 */
 
#define _ITRON_Task_Core_to_Priority( _core_priority ) \
  ((PRI) (_core_priority))

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
