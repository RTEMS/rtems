/*
 *  Object Handler
 *
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

#include <rtems/system.h>
#include <rtems/score/address.h>
#include <rtems/score/chain.h>
#include <rtems/score/object.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/objectmp.h>
#endif
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/isr.h>

/*PAGE
 *
 * _Objects_Get_by_index
 *
 * This routine sets the object pointer for the given
 * object id based on the given object information structure.
 *
 * Input parameters:
 *   information - pointer to entry in table for this class
 *   index       - object index to check for
 *   location    - address of where to store the location
 *
 * Output parameters:
 *   returns  - address of object if local
 *   location - one of the following:
 *                  OBJECTS_ERROR  - invalid object ID
 *                  OBJECTS_REMOTE - remote object
 *                  OBJECTS_LOCAL  - local object
 */

Objects_Control *_Objects_Get_by_index(
  Objects_Information *information,
  unsigned32           index,
  Objects_Locations   *location
)
{
  Objects_Control *the_object;

  if ( information->maximum >= index ) {
    _Thread_Disable_dispatch();
    the_object = information->local_table[ index ];
    if ( the_object ) {
      *location = OBJECTS_LOCAL;
      return( the_object );
    }
    _Thread_Enable_dispatch();
    *location = OBJECTS_ERROR;
    return( NULL );
  }

  /*
   *  With just an index, you can't access a remote object.
   */

  *location = OBJECTS_ERROR;
  return NULL;
}
