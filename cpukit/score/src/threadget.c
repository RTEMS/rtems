/*
 *  Thread Handler
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

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
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>

/*PAGE
 *
 *  _Thread_Get
 *
 *  NOTE:  If we are not using static inlines, this must be a real
 *         subroutine call.
 *
 *  NOTE:  XXX... This routine may be able to be optimized.
 */

#ifndef USE_INLINES

Thread_Control *_Thread_Get (
  Objects_Id           id,
  Objects_Locations   *location
)
{
  unsigned32           the_api;
  unsigned32           the_class;
  Objects_Information *information;
  Thread_Control      *tp = (Thread_Control *) 0;
 
  if ( _Objects_Are_ids_equal( id, OBJECTS_ID_OF_SELF ) ) {
    _Thread_Disable_dispatch();
    *location = OBJECTS_LOCAL;
    tp = _Thread_Executing;
    goto done;
  }
 
  the_api = _Objects_Get_API( id );
  if ( the_api && the_api > OBJECTS_APIS_LAST ) {
    *location = OBJECTS_ERROR;
    goto done;
  }
  
  the_class = _Objects_Get_class( id );
  if ( the_class != 1 ) {       /* threads are always first class :) */
    *location = OBJECTS_ERROR;
    goto done;
  }
 
  information = _Objects_Information_table[ the_api ][ the_class ];
 
  if ( !information ) {
    *location = OBJECTS_ERROR;
    goto done;
  }
 
  tp = (Thread_Control *) _Objects_Get( information, id, location );
 
done:
  return tp;
}

#endif
