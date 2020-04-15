/**
 *  @file
 *
 *  @brief Object Get Local
 *  @ingroup RTEMSScoreObject
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/objectimpl.h>

Objects_Control *_Objects_Get(
  Objects_Id                 id,
  ISR_lock_Context          *lock_context,
  const Objects_Information *information
)
{
  Objects_Id delta;
  Objects_Id maximum_id;
  Objects_Id end;

  maximum_id = information->maximum_id;
  delta = maximum_id - id;
  end = _Objects_Get_index( maximum_id );

  if ( RTEMS_PREDICT_TRUE( delta < end ) ) {
    ISR_Level        level;
    Objects_Control *the_object;

    _ISR_Local_disable( level );
    _ISR_lock_Context_set_level( lock_context, level );

    the_object =
      information->local_table[ end - OBJECTS_INDEX_MINIMUM - delta ];
    if ( RTEMS_PREDICT_TRUE( the_object != NULL ) ) {
      /* ISR disabled on behalf of caller */
      return the_object;
    }

    _ISR_Local_enable( level );
  }

  return NULL;
}
