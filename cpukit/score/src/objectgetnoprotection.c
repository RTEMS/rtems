/**
 *  @file
 *
 *  @brief  Get Object without Dispatching Protection
 *  @ingroup RTEMSScoreObject
 */

/*
 *  COPYRIGHT (c) 1989-2002.
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

Objects_Control *_Objects_Get_no_protection(
  Objects_Id                 id,
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
    return information->local_table[ end - OBJECTS_INDEX_MINIMUM - delta ];
  }

  /*
   *  This isn't supported or required yet for Global objects so
   *  if it isn't local, we don't find it.
   */
  return NULL;
}
