/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicRateMonotonic
 *
 * @brief This source file contains the implementation of
 *   rtems_rate_monotonic_get_status().
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright (c) 2016 embedded brains GmbH & Co. KG
 *  Copyright (c) 2017 Kuan-Hsun Chen.
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

rtems_status_code rtems_rate_monotonic_get_status(
  rtems_id                            id,
  rtems_rate_monotonic_period_status *period_status
)
{
  Rate_monotonic_Control *the_period;
  ISR_lock_Context        lock_context;

  if ( period_status == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  the_period = _Rate_monotonic_Get( id, &lock_context );
  if ( the_period == NULL ) {
    return RTEMS_INVALID_ID;
  }

  _Rate_monotonic_Acquire_critical( the_period, &lock_context );

  period_status->owner = the_period->owner->Object.id;
  period_status->state = the_period->state;
  period_status->postponed_jobs_count = the_period->postponed_jobs;

  if ( the_period->state == RATE_MONOTONIC_INACTIVE ) {
    /*
     *  If the period is inactive, there is no information.
     */
    _Timespec_Set_to_zero( &period_status->since_last_period );
    _Timespec_Set_to_zero( &period_status->executed_since_last_period );
  } else {
    Timestamp_Control wall_since_last_period;
    Timestamp_Control cpu_since_last_period;

    /*
     *  Grab the current status.
     */
    _Rate_monotonic_Get_status(
      the_period,
      &wall_since_last_period,
      &cpu_since_last_period
    );
    _Timestamp_To_timespec(
      &wall_since_last_period,
      &period_status->since_last_period
    );
    _Timestamp_To_timespec(
      &cpu_since_last_period,
      &period_status->executed_since_last_period
    );
  }

  _Rate_monotonic_Release( the_period, &lock_context );
  return RTEMS_SUCCESSFUL;
}
