/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreTOD
 *
 * @brief This source file contains the implementation of
 *   _TOD_Set().
 */

/*  COPYRIGHT (c) 1989-2007.
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

#include <rtems/score/todimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/watchdogimpl.h>

Status_Control _TOD_Set(
  const struct timespec *tod,
  ISR_lock_Context      *lock_context
)
{
  struct bintime  tod_as_bintime;
  uint64_t        tod_as_ticks;
  uint32_t        cpu_max;
  uint32_t        cpu_index;
  Status_Control  status;

  _Assert( _TOD_Is_owner() );
  _Assert( _TOD_Is_valid_new_time_of_day( tod ) == STATUS_SUCCESSFUL );

  status = _TOD_Hook_Run( TOD_ACTION_SET_CLOCK, tod );
  if ( status != STATUS_SUCCESSFUL ) {
    _TOD_Release( lock_context );
    return status;
  }

  timespec2bintime( tod, &tod_as_bintime );
  _Timecounter_Set_clock( &tod_as_bintime, lock_context );

  tod_as_ticks = _Watchdog_Ticks_from_timespec( tod );
  cpu_max = _SMP_Get_processor_maximum();

  for ( cpu_index = 0 ; cpu_index < cpu_max ; ++cpu_index ) {
    Per_CPU_Control  *cpu;
    Watchdog_Header  *header;
    ISR_lock_Context  lock_context_2;
    Watchdog_Control *first;

    cpu = _Per_CPU_Get_by_index( cpu_index );
    header = &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_REALTIME ];

    _ISR_lock_ISR_disable_and_acquire( &cpu->Watchdog.Lock, &lock_context_2 );

    first = _Watchdog_Header_first( header );

    if ( first != NULL ) {
      _Watchdog_Tickle(
        header,
        first,
        tod_as_ticks,
        &cpu->Watchdog.Lock,
        &lock_context_2
      );
    }

    _ISR_lock_Release_and_ISR_enable( &cpu->Watchdog.Lock, &lock_context_2 );
  }

  _TOD.is_set = true;

  return STATUS_SUCCESSFUL;
}
