/**
 *  @file
 *
 *  @brief Object Get Isr Disable
 *  @ingroup ScoreObject
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

Objects_Control *_Objects_Get_isr_disable(
  Objects_Information *information,
  Objects_Id           id,
  Objects_Locations   *location,
  ISR_lock_Context    *lock_context
)
{
  Objects_Control *the_object;
  uint32_t         index;

  index = id - information->minimum_id + 1;

  if ( information->maximum >= index ) {
    _ISR_lock_ISR_disable( lock_context );
    if ( (the_object = information->local_table[ index ]) != NULL ) {
      *location = OBJECTS_LOCAL;
      return the_object;
    }
    _ISR_lock_ISR_enable( lock_context );
    *location = OBJECTS_ERROR;
    return NULL;
  }
  *location = OBJECTS_ERROR;

#if defined(RTEMS_MULTIPROCESSING)
  _Objects_MP_Is_remote( information, id, location, &the_object );
  return the_object;
#else
  return NULL;
#endif
}
