/**
 * @file
 *
 * @brief Maps Thread IDs to TCB Pointer
 *
 * @ingroup ScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>

static Objects_Information *_Thread_Get_objects_information(
  Objects_Id id
)
{
  uint32_t             the_api;
  uint32_t             the_class;
  Objects_Information **api_information;

  the_api = _Objects_Get_API( id );
  if ( !_Objects_Is_api_valid( the_api ) ) {
    return NULL;
  }

  the_class = _Objects_Get_class( id );
  if ( the_class != 1 ) {       /* threads are always first class :) */
    return NULL;
  }

  api_information = _Objects_Information_table[ the_api ];
  /*
   *  There is no way for this to happen if POSIX is enabled.  But there
   *  is actually a test case in sp43 for this which trips it whether or
   *  not POSIX is enabled.  So in the interest of safety, this is left
   *  on in all configurations.
   */
  if ( !api_information ) {
    return NULL;
  }

  return api_information[ the_class ];
}

Thread_Control *_Thread_Get(
  Objects_Id         id,
  Objects_Locations *location
)
{
  Objects_Information *information;

  if ( _Objects_Are_ids_equal( id, OBJECTS_ID_OF_SELF ) ) {
    _Thread_Disable_dispatch();
    *location = OBJECTS_LOCAL;
    return _Thread_Executing;
  }

  information = _Thread_Get_objects_information( id );
  if ( information == NULL ) {
    *location = OBJECTS_ERROR;
    return NULL;
  }

  return (Thread_Control *) _Objects_Get( information, id, location );
}

Thread_Control *_Thread_Get_interrupt_disable(
  Objects_Id         id,
  Objects_Locations *location,
  ISR_lock_Context  *lock_context
)
{
  Objects_Information *information;

  if ( _Objects_Are_ids_equal( id, OBJECTS_ID_OF_SELF ) ) {
    *location = OBJECTS_LOCAL;
    _ISR_lock_ISR_disable( lock_context );
    return _Thread_Executing;
  }

  information = _Thread_Get_objects_information( id );
  if ( information == NULL ) {
    *location = OBJECTS_ERROR;
    return NULL;
  }

  return (Thread_Control *)
    _Objects_Get_isr_disable( information, id, location, lock_context );
}
