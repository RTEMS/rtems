/*
 *  Thread Handler
 *
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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
  Objects_Classes      the_class;
  Objects_Information *information;
 
  if ( _Objects_Are_ids_equal( id, OBJECTS_ID_OF_SELF ) ) {
    _Thread_Disable_dispatch();
    *location = OBJECTS_LOCAL;
    return( _Thread_Executing );
  }
 
  the_class = _Objects_Get_class( id );
 
  if ( the_class > OBJECTS_CLASSES_LAST ) {
    *location = OBJECTS_ERROR;
    return (Thread_Control *) 0;
  } 
 
  information = _Objects_Information_table[ the_class ];
 
  if ( !information || !information->is_thread ) { 
    *location = OBJECTS_ERROR;
    return (Thread_Control *) 0;
  }
 
  return (Thread_Control *) _Objects_Get( information, id, location );
}

#endif
