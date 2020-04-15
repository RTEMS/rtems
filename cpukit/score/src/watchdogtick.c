/*
 * Copyright (c) 2015, 2016 embedded brains GmbH.  All rights reserved.
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
  ISR_lock_Context  lock_context;
  Watchdog_Header  *header;
  Watchdog_Control *first;
  uint64_t          ticks;
  struct timespec   now;

  if ( _Per_CPU_Is_boot_processor( cpu ) ) {
    ++_Watchdog_Ticks_since_boot;
  }

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

  _Scheduler_Tick( cpu );
}
