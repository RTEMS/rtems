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
 *  _Objects_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a object control block from
 *  the inactive chain of free object control blocks.
 */

Objects_Control *_Objects_Allocate(
  Objects_Information *information
)
{
  Objects_Control *the_object =  
    (Objects_Control *) _Chain_Get( &information->Inactive );

  if ( information->auto_extend ) {
    /*
     *  If the list is empty then we are out of objects and need to
     *  extend information base.
     */
  
    if ( !the_object ) {
      _Objects_Extend_information( information );
      the_object =  (Objects_Control *) _Chain_Get( &information->Inactive );
    }
  
    if ( the_object ) {
      unsigned32 block;
    
      block = _Objects_Get_index( the_object->id ) -
              _Objects_Get_index( information->minimum_id );
      block /= information->allocation_size;
      
      information->inactive_per_block[ block ]--;
      information->inactive--;
    }
  }
  
  return the_object;
}
