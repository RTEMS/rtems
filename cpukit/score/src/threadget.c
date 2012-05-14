/*
 *  Thread Handler - Object Id to Thread Pointer
 *
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

#include <rtems/system.h>
#include <rtems/score/apiext.h>
#include <rtems/score/context.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/states.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>


/**
 *  This function maps thread IDs to thread control
 *  blocks.  If ID corresponds to a local thread, then it
 *  returns the_thread control pointer which maps to ID
 *  and location is set to OBJECTS_LOCAL.  If the thread ID is
 *  global and resides on a remote node, then location is set
 *  to OBJECTS_REMOTE, and the_thread is undefined.
 *  Otherwise, location is set to OBJECTS_ERROR and
 *  the_thread is undefined.
 *
 *  @note  The performance of many RTEMS services depends upon
 *         the quick execution of the "good object" path in this
 *         routine.  If there is a possibility of saving a few
 *         cycles off the execution time, this routine is worth
 *         further optimization attention.
 */
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

