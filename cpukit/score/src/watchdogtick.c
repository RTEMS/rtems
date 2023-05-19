/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreWatchdog
 *
 * @brief This source file contains the implementation of
 *   _Watchdog_Do_tickle() and _Watchdog_Tick().
 */

/*
 * Copyright (C) 2015, 2016 embedded brains GmbH & Co. KG
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

#include <rtems/score/watchdogimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/timecounter.h>

void _Watchdog_Do_tickle(
  Watchdog_Header  *header,
  Watchdog_Control *first,
  uint64_t          now,
#ifdef RTEMS_SMP
  ISR_lock_Control *lock,
#endif
  ISR_lock_Context *lock_context
)
{
  do {
    if ( first->expire <= now ) {
      Watchdog_Service_routine_entry routine;

      _Watchdog_Next_first( header, first );
      _RBTree_Extract( &header->Watchdogs, &first->Node.RBTree );
      _Watchdog_Set_state( first, WATCHDOG_INACTIVE );
      routine = first->routine;

      _ISR_lock_Release_and_ISR_enable( lock, lock_context );
      ( *routine )( first );
      _ISR_lock_ISR_disable_and_acquire( lock, lock_context );
    } else {
      break;
    }

    first = _Watchdog_Header_first( header );
  } while ( first != NULL );
}

void _Watchdog_Tick( Per_CPU_Control *cpu )
{
  ISR_lock_Context                    lock_context;
  Watchdog_Header                    *header;
  Watchdog_Control                   *first;
  uint64_t                            ticks;
  struct timespec                     now;
  Thread_Control                     *executing;
  const Thread_CPU_budget_operations *cpu_budget_operations;

#ifdef RTEMS_SMP
  if ( _Per_CPU_Is_boot_processor( cpu ) ) {
#endif
    ++_Watchdog_Ticks_since_boot;
#ifdef RTEMS_SMP
  }
#endif

  _ISR_lock_ISR_disable_and_acquire( &cpu->Watchdog.Lock, &lock_context );

  ticks = cpu->Watchdog.ticks;
  _Assert( ticks < UINT64_MAX );
  ++ticks;
  cpu->Watchdog.ticks = ticks;

  header = &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ];
  first = _Watchdog_Header_first( header );

  if ( first != NULL ) {
    _Watchdog_Tickle(
      header,
      first,
      ticks,
      &cpu->Watchdog.Lock,
      &lock_context
    );
  }

  header = &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_MONOTONIC ];
  first = _Watchdog_Header_first( header );

  if ( first != NULL ) {
    _Timecounter_Getnanouptime( &now );
    _Watchdog_Tickle(
      header,
      first,
      _Watchdog_Ticks_from_timespec( &now ),
      &cpu->Watchdog.Lock,
      &lock_context
    );
  }

  header = &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_REALTIME ];
  first = _Watchdog_Header_first( header );

  if ( first != NULL ) {
    _Timecounter_Getnanotime( &now );
    _Watchdog_Tickle(
      header,
      first,
      _Watchdog_Ticks_from_timespec( &now ),
      &cpu->Watchdog.Lock,
      &lock_context
    );
  }

  _ISR_lock_Release_and_ISR_enable( &cpu->Watchdog.Lock, &lock_context );

  /*
   * Each online processor has at least an idle thread as the executing thread
   * even in case it has currently no scheduler assigned.  Clock interrupts on
   * processors which are not online would be a severe bug of the Clock Driver.
   */
  executing = _Per_CPU_Get_executing( cpu );
  _Assert( executing != NULL );
  cpu_budget_operations = executing->CPU_budget.operations;

  if ( cpu_budget_operations != NULL ) {
    ( *cpu_budget_operations->at_tick )( executing );
  }
}
