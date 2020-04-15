/**
 * @file
 *
 * @brief Maps Thread IDs to TCB Pointer
 *
 * @ingroup RTEMSScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>

Thread_Control *_Thread_Get(
  Objects_Id         id,
  ISR_lock_Context  *lock_context
)
{
  Objects_Information *information;

  if ( _Objects_Are_ids_equal( id, OBJECTS_ID_OF_SELF ) ) {
    _ISR_lock_ISR_disable( lock_context );
    return _Thread_Executing;
  }

  information = _Thread_Get_objects_information( id );
  if ( information == NULL ) {
    return NULL;
  }

  return (Thread_Control *)
    _Objects_Get( id, lock_context, information );
}
