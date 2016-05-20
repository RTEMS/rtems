/**
 *  @file
 *
 *  @brief Object Get Local
 *  @ingroup ScoreObject
 */

/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

Objects_Control *_Objects_Get(
  Objects_Id                 id,
  ISR_lock_Context          *lock_context,
  const Objects_Information *information
)
{
  uint32_t index;

  index = id - information->minimum_id + 1;

  if ( information->maximum >= index ) {
    Objects_Control *the_object;

    _ISR_lock_ISR_disable( lock_context );

    the_object = information->local_table[ index ];
    if ( the_object != NULL ) {
      /* ISR disabled on behalf of caller */
      return the_object;
    }

    _ISR_lock_ISR_enable( lock_context );
  }

  return NULL;
}
