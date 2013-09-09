/*
 *  @file
 *
 *  @brief Get Object
 *  @ingroup Score
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

Objects_Control *_Objects_Get(
  Objects_Information *information,
  Objects_Id           id,
  Objects_Locations   *location
)
{
  Objects_Control *the_object;
  uint32_t         index;

  /*
   *  Extract the index portion of an Id in a way that produces a valid
   *  index for objects within this class and an invalid value for objects
   *  outside this class.
   *
   *  If the Id matches the api, class, and node but index portion is 0,
   *  then the subtraction will underflow and the addition of 1 will
   *  result in a 0 index.  The zeroth element in the local_table is
   *  always NULL.
   *
   *  If the Id is valid but the object has not been created yet, then
   *  the local_table entry will be NULL.
   */
  index = id - information->minimum_id + 1;

  /*
   *  If the index is less than maximum, then it is OK to use it to
   *  index into the local_table array.
   */
  if ( index <= information->maximum ) {
    _Thread_Disable_dispatch();
    if ( (the_object = information->local_table[ index ]) != NULL ) {
      *location = OBJECTS_LOCAL;
      return the_object;
    }

    /*
     *  Valid Id for this API, Class and Node but the object has not
     *  been allocated yet.
     */
    _Thread_Enable_dispatch();
    *location = OBJECTS_ERROR;
    return NULL;
  }

  /*
   *  Object Id is not within this API and Class on this node.  So
   *  it may be global in a multiprocessing system.  But it is clearly
   *  invalid on a single processor system.
   */
  *location = OBJECTS_ERROR;

#if defined(RTEMS_MULTIPROCESSING)
  _Objects_MP_Is_remote( information, id, location, &the_object );
  return the_object;
#else
  return NULL;
#endif
}
