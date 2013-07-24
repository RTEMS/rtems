/**
 *  @file
 *
 *  @brief  Get Object without Dispatching Protection
 *  @ingroup ScoreObject
 */

/*
 *  COPYRIGHT (c) 1989-2002.
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

Objects_Control *_Objects_Get_no_protection(
  Objects_Information *information,
  Objects_Id           id,
  Objects_Locations   *location
)
{
  Objects_Control *the_object;
  uint32_t         index;

  /*
   * You can't just extract the index portion or you can get tricked
   * by a value between 1 and maximum.
   */
  index = id - information->minimum_id + 1;

  if ( information->maximum >= index ) {
    if ( (the_object = information->local_table[ index ]) != NULL ) {
      *location = OBJECTS_LOCAL;
      return the_object;
    }
  }

  /*
   *  This isn't supported or required yet for Global objects so
   *  if it isn't local, we don't find it.
   */
  *location = OBJECTS_ERROR;
  return NULL;
}
