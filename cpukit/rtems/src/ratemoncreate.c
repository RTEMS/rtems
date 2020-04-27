/**
 * @file
 *
 * @ingroup ClassicRateMon Rate Monotonic Scheduler
 *
 * @brief Create a Period
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/ratemonimpl.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/sysinit.h>

rtems_status_code rtems_rate_monotonic_create(
  rtems_name  name,
  rtems_id   *id
)
{
  Rate_monotonic_Control *the_period;

  if ( !rtems_is_name_valid( name ) )
    return RTEMS_INVALID_NAME;

  if ( !id )
    return RTEMS_INVALID_ADDRESS;

  the_period = _Rate_monotonic_Allocate();

  if ( !the_period ) {
    _Objects_Allocator_unlock();
    return RTEMS_TOO_MANY;
  }

  _ISR_lock_Initialize( &the_period->Lock, "Rate Monotonic Period" );
  _Priority_Node_initialize( &the_period->Priority, 0 );
  _Priority_Node_set_inactive( &the_period->Priority );

  the_period->owner = _Thread_Get_executing();
  the_period->state = RATE_MONOTONIC_INACTIVE;

  _Watchdog_Preinitialize( &the_period->Timer, _Per_CPU_Get_by_index( 0 ) );
  _Watchdog_Initialize( &the_period->Timer, _Rate_monotonic_Timeout );

  _Rate_monotonic_Reset_statistics( the_period );

  _Objects_Open(
    &_Rate_monotonic_Information,
    &the_period->Object,
    (Objects_Name) name
  );

  *id = the_period->Object.id;
  _Objects_Allocator_unlock();
  return RTEMS_SUCCESSFUL;
}

static void _Rate_monotonic_Manager_initialization( void )
{
  _Objects_Initialize_information( &_Rate_monotonic_Information );
}

RTEMS_SYSINIT_ITEM(
  _Rate_monotonic_Manager_initialization,
  RTEMS_SYSINIT_CLASSIC_RATE_MONOTONIC,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
