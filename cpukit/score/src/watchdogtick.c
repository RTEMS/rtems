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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/watchdogimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/timecounter.h>

void _Watchdog_Do_tickle(
  Watchdog_Header  *header,
  uint64_t          now,
#ifdef RTEMS_SMP
  ISR_lock_Control *lock,
#endif
  ISR_lock_Context *lock_context
)
{
  while ( true ) {
    Watchdog_Control *the_watchdog;

    the_watchdog = (Watchdog_Control *) header->first;

    if ( the_watchdog == NULL ) {
      break;
    }

    if ( the_watchdog->expire <= now ) {
      Watchdog_Service_routine_entry routine;

      _Watchdog_Next_first( header, the_watchdog );
      _RBTree_Extract( &header->Watchdogs, &the_watchdog->Node.RBTree );
      _Watchdog_Set_state( the_watchdog, WATCHDOG_INACTIVE );
      routine = the_watchdog->routine;

      _ISR_lock_Release_and_ISR_enable( lock, lock_context );
      ( *routine )( the_watchdog );
      _ISR_lock_ISR_disable_and_acquire( lock, lock_context );
    } else {
      break;
    }
  }

  _ISR_lock_Release_and_ISR_enable( lock, lock_context );
}

void _Watchdog_Tick( Per_CPU_Control *cpu )
{
  ISR_lock_Context lock_context;
  uint64_t         ticks;
  struct timespec  now;

  if ( _Per_CPU_Is_boot_processor( cpu ) ) {
    ++_Watchdog_Ticks_since_boot;
  }

  _ISR_lock_ISR_disable_and_acquire( &cpu->Watchdog.Lock, &lock_context );

  ticks = cpu->Watchdog.ticks;
  _Assert( ticks < UINT64_MAX );
  ++ticks;
  cpu->Watchdog.ticks = ticks;

  _Watchdog_Tickle(
    &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_RELATIVE ],
    ticks,
    &cpu->Watchdog.Lock,
    &lock_context
  );

  _Timecounter_Getnanotime( &now );
  _Watchdog_Per_CPU_tickle_absolute(
    cpu,
    _Watchdog_Ticks_from_timespec( &now )
  );

  _Scheduler_Tick( cpu );
}
