/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Schedule Alarm
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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

#include <signal.h>
#include <unistd.h>

#include <rtems/score/todimpl.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/config.h>

ISR_LOCK_DEFINE( static, _POSIX_signals_Ualarm_lock, "POSIX Ualarm" )

static uint32_t _POSIX_signals_Ualarm_interval;

static void _POSIX_signals_Ualarm_TSR( Watchdog_Control *the_watchdog )
{
  int              status;
  ISR_lock_Context lock_context;

  status = kill( getpid(), SIGALRM );

  #if defined(RTEMS_DEBUG)
    /*
     *  There is no reason to think this might fail but we should be
     *  cautious.
     */
    _Assert(status == 0);
  #else
    (void) status;
  #endif

  _ISR_lock_ISR_disable_and_acquire(
    &_POSIX_signals_Ualarm_lock,
    &lock_context
  );

  /*
   * If the reset interval is non-zero, reschedule ourselves.
   */
  if ( _POSIX_signals_Ualarm_interval != 0 ) {
    _Watchdog_Per_CPU_insert_ticks(
      the_watchdog,
      _Per_CPU_Get(),
      _POSIX_signals_Ualarm_interval
    );
  }

  _ISR_lock_Release_and_ISR_enable(
    &_POSIX_signals_Ualarm_lock,
    &lock_context
  );
}

static Watchdog_Control _POSIX_signals_Ualarm_watchdog = WATCHDOG_INITIALIZER(
  _POSIX_signals_Ualarm_TSR
);

static uint32_t _POSIX_signals_Ualarm_us_to_ticks( useconds_t us )
{
  uint32_t us_per_tick = rtems_configuration_get_microseconds_per_tick();

  return ( us + us_per_tick - 1 ) / us_per_tick;
}

useconds_t ualarm(
  useconds_t useconds,
  useconds_t interval
)
{
  useconds_t        remaining;
  Watchdog_Control *the_watchdog;
  ISR_lock_Context  lock_context;
  ISR_lock_Context  lock_context2;
  Per_CPU_Control  *cpu;
  uint64_t          now;
  uint32_t          ticks_initial;
  uint32_t          ticks_interval;

  the_watchdog = &_POSIX_signals_Ualarm_watchdog;
  ticks_initial = _POSIX_signals_Ualarm_us_to_ticks( useconds );
  ticks_interval = _POSIX_signals_Ualarm_us_to_ticks( interval );

  _ISR_lock_ISR_disable_and_acquire(
    &_POSIX_signals_Ualarm_lock,
    &lock_context
  );

  cpu = _Watchdog_Get_CPU( the_watchdog );
  _Watchdog_Per_CPU_acquire_critical( cpu, &lock_context2 );
  now = cpu->Watchdog.ticks;

  remaining = (useconds_t) _Watchdog_Cancel(
    &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ],
    the_watchdog,
    now
  );

  if ( ticks_initial != 0 ) {
    _POSIX_signals_Ualarm_interval = ticks_interval;

    cpu = _Per_CPU_Get();
    _Watchdog_Set_CPU( the_watchdog, cpu );
    _Watchdog_Insert(
      &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ],
      the_watchdog,
      now + ticks_initial
    );
  }

  _Watchdog_Per_CPU_release_critical( cpu, &lock_context2 );
  _ISR_lock_Release_and_ISR_enable(
    &_POSIX_signals_Ualarm_lock,
    &lock_context
  );

  remaining *= rtems_configuration_get_microseconds_per_tick();

  return remaining;
}
