/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicRateMonotonic
 *
 * @brief This source file contains the implementation of
 *   rtems_rate_monotonic_create().
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
  the_period->postponed_jobs = 0;

  _Watchdog_Preinitialize( &the_period->Timer, _Per_CPU_Get_by_index( 0 ) );
  _Watchdog_Initialize( &the_period->Timer, _Rate_monotonic_Timeout );

  _Rate_monotonic_Reset_statistics( the_period );

  *id = _Objects_Open_u32(
    &_Rate_monotonic_Information,
    &the_period->Object,
    name
  );
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
