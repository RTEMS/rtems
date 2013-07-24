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
#include <rtems/score/isr.h>

Objects_Control *_Objects_Get_isr_disable(
  Objects_Information *information,
  Objects_Id           id,
  Objects_Locations   *location,
  ISR_Level           *level_p
)
{
  Objects_Control *the_object;
  uint32_t         index;
  ISR_Level        level;

  index = id - information->minimum_id + 1;

  if ( information->maximum >= index ) {
#if defined(RTEMS_SMP)
    _Thread_Disable_dispatch();
#endif
    _ISR_Disable( level );
    if ( (the_object = information->local_table[ index ]) != NULL ) {
      *location = OBJECTS_LOCAL;
      *level_p = level;
      return the_object;
    }
    _ISR_Enable( level );
#if defined(RTEMS_SMP)
    _Thread_Enable_dispatch();
#endif
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
