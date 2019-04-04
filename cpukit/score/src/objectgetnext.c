/**
 * @file
 *
 * @brief Get Pointer to Next Object that is Active
 * @ingroup RTEMSScoreObject
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

Objects_Control *_Objects_Get_next(
  Objects_Id                 id,
  const Objects_Information *information,
  Objects_Id                *next_id_p
)
{
    Objects_Control *the_object;
    Objects_Id       next_id;
    Objects_Maximum  maximum;

    if ( !information )
      return NULL;

    if ( !next_id_p )
      return NULL;

    if (_Objects_Get_index(id) == OBJECTS_ID_INITIAL_INDEX)
        next_id = _Objects_Get_minimum_id( information->maximum_id );
    else
        next_id = id;

    _Objects_Allocator_lock();
    maximum = _Objects_Get_maximum_index( information );

    do {
      /* walked off end of list? */
      if (_Objects_Get_index( next_id ) > maximum) {
        _Objects_Allocator_unlock();
        *next_id_p = OBJECTS_ID_FINAL;
        return NULL;
      }

      /* try to grab one */
      the_object = _Objects_Get_no_protection( next_id, information );

      next_id++;
    } while ( the_object == NULL );

    *next_id_p = next_id;
    return the_object;
}
