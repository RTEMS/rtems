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
 *  _Objects_Allocate_by_index
 *
 *  DESCRIPTION:
 *
 *  This function allocates the object control block 
 *  specified by the index from the inactive chain of 
 *  free object control blocks.
 */

Objects_Control *_Objects_Allocate_by_index(
  Objects_Information *information,
  unsigned32           index,
  unsigned32           sizeof_control
)
{
  Objects_Control *the_object;

  if ( index && information->maximum >= index ) {
    the_object = information->local_table[ index ];
    if ( the_object )
      return NULL;

    /* XXX
     *  This whole section of code needs to be addressed.
     *    +  The 0 should be dealt with more properly so we can autoextend.
     *    +  The pointer arithmetic is probably too expensive.
     *    +  etc.
     */
    
    the_object = (Objects_Control *) _Addresses_Add_offset(
      information->object_blocks[ 0 ],
      (sizeof_control * (index - 1))
    );
    _Chain_Extract( &the_object->Node );
 
    return the_object;   
  }    

  /*
   *  Autoextend will have to be thought out as it applies
   *  to user assigned indices.
   */

  return NULL;
}
