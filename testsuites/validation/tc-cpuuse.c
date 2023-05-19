/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsCpuuseValCpuuse
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/cpuuse.h>
#include <rtems/score/threadimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsCpuuseValCpuuse spec:/rtems/cpuuse/val/cpuuse
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests the CPU usage reporting and reset.
 *
 * This test case performs the following actions:
 *
 * - Create a worker thread.  Generate some CPU usage.
 *
 *   - Check that we have a non-zero CPU usage.  Reset the CPU usage.  Check
 *     that it was cleared to zero.
 *
 *   - Clean up all used resources.
 *
 * @{
 */

static void Worker( rtems_task_argument arg )
{
  (void) arg;

  while ( true ) {
    Yield();
  }
}

/**
 * @brief Create a worker thread.  Generate some CPU usage.
 */
static void RtemsCpuuseValCpuuse_Action_0( void )
{
  rtems_id          id;
  Thread_Control   *self;
  Thread_Control   *other;
  Timestamp_Control cpu_usage_self;
  Timestamp_Control cpu_usage_other;
  uint32_t          idle_tasks;
  uint32_t          cpu_index;

  idle_tasks = 0;

  for ( cpu_index = 0; cpu_index < rtems_scheduler_get_processor_maximum(); ++cpu_index ) {
    rtems_status_code sc;
    rtems_id          unused;

    sc = rtems_scheduler_ident_by_processor( cpu_index, &unused );

    if ( sc == RTEMS_SUCCESSFUL ) {
      ++idle_tasks;
    }
  }

  id = CreateTask( "WORK", GetSelfPriority() );
  StartTask( id, Worker, NULL );

  self = GetThread( RTEMS_SELF );
  other = GetThread( id );

  Yield();
  Yield();

  /*
   * Check that we have a non-zero CPU usage.  Reset the CPU usage.  Check that
   * it was cleared to zero.
   */
  TimecounterTick();

  cpu_usage_self = _Thread_Get_CPU_time_used( self );
  cpu_usage_other = _Thread_Get_CPU_time_used( other );

  T_gt_i64( _Thread_Get_CPU_time_used_after_last_reset( self ), 0 );
  T_gt_i64( _Thread_Get_CPU_time_used_after_last_reset( other ), 0 );

  rtems_cpu_usage_reset();

  /*
   * Our CPU usage after the last reset is now exactly one tick of the
   * software timecounter.
   */
  T_eq_i64( _Thread_Get_CPU_time_used_after_last_reset( self ), 4295 );
  T_eq_i64(
    _Thread_Get_CPU_time_used( self ),
    cpu_usage_self + 12885 + 4295 * idle_tasks
  );

  T_eq_i64( _Thread_Get_CPU_time_used_after_last_reset( other ), 0 );
  T_eq_i64( _Thread_Get_CPU_time_used( other ), cpu_usage_other );

  /*
   * Clean up all used resources.
   */
  DeleteTask( id );
}

/**
 * @fn void T_case_body_RtemsCpuuseValCpuuse( void )
 */
T_TEST_CASE( RtemsCpuuseValCpuuse )
{
  RtemsCpuuseValCpuuse_Action_0();
}

/** @} */
