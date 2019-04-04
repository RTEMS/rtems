/**
 *  @file
 *
 *  @brief Allocate Object
 *  @ingroup RTEMSScoreObject
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/objectimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/sysstate.h>

static Objects_Control *_Objects_Get_inactive(
  Objects_Information *information
)
{
  return (Objects_Control *) _Chain_Get_unprotected( &information->Inactive );
}

Objects_Control *_Objects_Allocate_unprotected(
  Objects_Information *information
)
{
  Objects_Control *the_object;

  _Assert(
    _Objects_Allocator_is_owner()
      || !_System_state_Is_up( _System_state_Get() )
  );

  /*
   *  If the application is using the optional manager stubs and
   *  still attempts to create the object, the information block
   *  should be all zeroed out because it is in the BSS.  So let's
   *  check that code for this manager is even present.
   */
  if ( information->object_size == 0 )
    return NULL;

  /*
   *  OK.  The manager should be initialized and configured to have objects.
   *  With any luck, it is safe to attempt to allocate an object.
   */
  the_object = _Objects_Get_inactive( information );

  if ( _Objects_Is_auto_extend( information ) ) {
    /*
     *  If the list is empty then we are out of objects and need to
     *  extend information base.
     */

    if ( the_object == NULL ) {
      _Objects_Extend_information( information );
      the_object = _Objects_Get_inactive( information );
    }

    if ( the_object != NULL ) {
      Objects_Maximum objects_per_block;
      Objects_Maximum block;

      objects_per_block = information->objects_per_block;
      block = _Objects_Get_index( the_object->id ) - OBJECTS_INDEX_MINIMUM;

      if ( block > objects_per_block ) {
        block /= objects_per_block;

        information->inactive_per_block[ block ]--;
        information->inactive--;
      }
    }
  }

  return the_object;
}

Objects_Control *_Objects_Allocate( Objects_Information *information )
{
  _RTEMS_Lock_allocator();

  return _Objects_Allocate_unprotected( information );
}
