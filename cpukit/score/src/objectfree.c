/**
 *  @file
 *
 *  @brief Free Object
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

void _Objects_Free(
  Objects_Information *information,
  Objects_Control     *the_object
)
{
  _Assert( _Objects_Allocator_is_owner() );

  _Chain_Append_unprotected( &information->Inactive, &the_object->Node );

  if ( _Objects_Is_auto_extend( information ) ) {
    Objects_Maximum objects_per_block;
    Objects_Maximum block;
    Objects_Maximum inactive;

    objects_per_block = information->objects_per_block;
    block = _Objects_Get_index( the_object->id ) - OBJECTS_INDEX_MINIMUM;

    if ( block > objects_per_block ) {
      block /= objects_per_block;

      ++information->inactive_per_block[ block ];

      inactive = information->inactive;
      ++inactive;
      information->inactive = inactive;

      /*
       *  Check if the threshold level has been met of
       *  1.5 x objects_per_block are free.
       */

      if ( inactive > ( objects_per_block + ( objects_per_block >> 1 ) ) ) {
        _Objects_Shrink_information( information );
      }
    }
  }
}
