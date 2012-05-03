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

/*
 * _Objects_Get_next
 *
 * Like _Objects_Get, but considers the 'id' as a "hint" and
 * finds next valid one after that point.
 * Mostly used for monitor and debug traversal of an object.
 *
 * Input parameters:
 *   information - pointer to entry in table for this class
 *   id          - object id to search for
 *   location    - address of where to store the location
 *   next_id     - address to store next id to try
 *
 * Output parameters:
 *   returns     - address of object if local
 *   location    - one of the following:
 *                  OBJECTS_ERROR  - invalid object ID
 *                  OBJECTS_REMOTE - remote object
 *                  OBJECTS_LOCAL  - local object
 *   next_id     - will contain a reasonable "next" id to continue traversal
 *
 * NOTE:
 *      assumes can add '1' to an id to get to next index.
 */

Objects_Control *
_Objects_Get_next(
    Objects_Information *information,
    Objects_Id           id,
    Objects_Locations   *location_p,
    Objects_Id          *next_id_p
)
{
    Objects_Control *object;
    Objects_Id       next_id;

    if ( !information )
      return NULL;

    if ( !location_p )
      return NULL;

    if ( !next_id_p )
      return NULL;

    if (_Objects_Get_index(id) == OBJECTS_ID_INITIAL_INDEX)
        next_id = information->minimum_id;
    else
        next_id = id;

    do {
        /* walked off end of list? */
        if (_Objects_Get_index(next_id) > information->maximum)
        {
            *location_p = OBJECTS_ERROR;
            goto final;
        }

        /* try to grab one */
        object = _Objects_Get(information, next_id, location_p);

        next_id++;

    } while (*location_p != OBJECTS_LOCAL);

    *next_id_p = next_id;
    return object;

final:
    *next_id_p = OBJECTS_ID_FINAL;
    return 0;
}
