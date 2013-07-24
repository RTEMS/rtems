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
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>

Thread_Control *_Thread_Get (
  Objects_Id         id,
  Objects_Locations *location
)
{
  uint32_t             the_api;
  uint32_t             the_class;
  Objects_Information **api_information;
  Objects_Information *information;
  Thread_Control      *tp = (Thread_Control *) 0;

  if ( _Objects_Are_ids_equal( id, OBJECTS_ID_OF_SELF ) ) {
    _Thread_Disable_dispatch();
    *location = OBJECTS_LOCAL;
    tp = _Thread_Executing;
    goto done;
  }

  the_api = _Objects_Get_API( id );
  if ( !_Objects_Is_api_valid( the_api ) ) {
    *location = OBJECTS_ERROR;
    goto done;
  }

  the_class = _Objects_Get_class( id );
  if ( the_class != 1 ) {       /* threads are always first class :) */
    *location = OBJECTS_ERROR;
    goto done;
  }

  api_information = _Objects_Information_table[ the_api ];
  /*
   *  There is no way for this to happen if POSIX is enabled.  But there
   *  is actually a test case in sp43 for this which trips it whether or
   *  not POSIX is enabled.  So in the interest of safety, this is left
   *  on in all configurations.
   */
  if ( !api_information ) {
    *location = OBJECTS_ERROR;
    goto done;
  }

  information = api_information[ the_class ];
  if ( !information ) {
    *location = OBJECTS_ERROR;
    goto done;
  }

  tp = (Thread_Control *) _Objects_Get( information, id, location );

done:
  return tp;
}

