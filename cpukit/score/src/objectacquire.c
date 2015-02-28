/**
 * @file
 *
 * @brief Object Acquire
 *
 * @ingroup ScoreObject
 */

/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/objectimpl.h>

Objects_Control *_Objects_Acquire(
  const Objects_Information *information,
  Objects_Id                 id,
  Objects_Locations         *location,
  ISR_lock_Context          *lock_context
)
{
  Objects_Control *the_object;
  uint32_t         index;

  index = id - information->minimum_id + 1;

  if ( information->maximum >= index ) {
    /*
     * On uni-processor configurations we disable interrupts before we use the
     * local table.  This prevents use of freed memory in case the object
     * information is extended in between.  On SMP configurations bad things
     * can happen, see https://devel.rtems.org/ticket/2280.
     */
#if !defined(RTEMS_SMP)
    ISR_Level level;

    _ISR_Disable( level );
#endif
    if ( ( the_object = information->local_table[ index ] ) != NULL ) {
      *location = OBJECTS_LOCAL;
#if defined(RTEMS_SMP)
      _ISR_lock_ISR_disable_and_acquire( &the_object->Lock, lock_context );
#else
      lock_context->isr_level = level;
#endif

      return the_object;
    }
#if !defined(RTEMS_SMP)
    _ISR_Enable( level );
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
