/**
 *  @file
 *
 *  @brief Free Object
 *  @ingroup ScoreObject
 */

/*
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

#include <rtems/score/objectimpl.h>
#include <rtems/score/chainimpl.h>

void _Objects_Free(
  Objects_Information *information,
  Objects_Control     *the_object
)
{
  uint32_t    allocation_size = information->allocation_size;

  _Chain_Append( &information->Inactive, &the_object->Node );

  if ( information->auto_extend ) {
    uint32_t    block;

    block = (uint32_t) (_Objects_Get_index( the_object->id ) -
                        _Objects_Get_index( information->minimum_id ));
    block /= information->allocation_size;

    information->inactive_per_block[ block ]++;
    information->inactive++;

    /*
     *  Check if the threshold level has been met of
     *  1.5 x allocation_size are free.
     */

    if ( information->inactive > ( allocation_size + ( allocation_size >> 1 ) ) ) {
      _Objects_Shrink_information( information );
    }
  }
}
