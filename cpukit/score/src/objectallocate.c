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
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

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

/* #define RTEMS_DEBUG_OBJECT_ALLOCATION */

#if defined(RTEMS_DEBUG_OBJECT_ALLOCATION)
#include <rtems/bspIo.h>
#endif

/*
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
  Objects_Control *the_object;

  /*
   *  If the application is using the optional manager stubs and
   *  still attempts to create the object, the information block
   *  should be all zeroed out because it is in the BSS.  So let's
   *  check that code for this manager is even present.
   */
  if ( information->size == 0 )
    return NULL;

  /*
   *  OK.  The manager should be initialized and configured to have objects.
   *  With any luck, it is safe to attempt to allocate an object.
   */
  the_object = (Objects_Control *) _Chain_Get( &information->Inactive );

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
      uint32_t   block;

      block = (uint32_t) _Objects_Get_index( the_object->id ) -
              _Objects_Get_index( information->minimum_id );
      block /= information->allocation_size;

      information->inactive_per_block[ block ]--;
      information->inactive--;
    }
  }

#if defined(RTEMS_DEBUG_OBJECT_ALLOCATION)
  if ( !the_object ) {
    printk(
      "OBJECT ALLOCATION FAILURE! API/Class %d/%d\n",
      information->the_api,
      information->the_class
    );
  }
#endif

  return the_object;
}
