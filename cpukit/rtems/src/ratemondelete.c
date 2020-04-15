/**
 *  @file
 *
 *  @brief RTEMS Delete Rate Monotonic
 *  @ingroup ClassicRateMon
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright (c) 2016 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/ratemonimpl.h>

rtems_status_code rtems_rate_monotonic_delete(
  rtems_id id
)
{
  Rate_monotonic_Control *the_period;
  ISR_lock_Context        lock_context;
  rtems_status_code       status;

  _Objects_Allocator_lock();

  the_period = _Rate_monotonic_Get( id, &lock_context );
  if ( the_period != NULL ) {
    _Objects_Close( &_Rate_monotonic_Information, &the_period->Object );
    _Rate_monotonic_Cancel( the_period, the_period->owner, &lock_context );
    _Objects_Free( &_Rate_monotonic_Information, &the_period->Object );
    status = RTEMS_SUCCESSFUL;
  } else {
    status = RTEMS_INVALID_ID;
  }

  _Objects_Allocator_unlock();

  return status;
}
