/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief System Generates Signal for process after realtime seconds elapsed
 */

/*
 *  3.4.1 Schedule Alarm, P1003.1b-1993, p. 79
 */

/*  COPYRIGHT (c) 1989-2013.
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

#include <unistd.h>
#include <signal.h>

#include <rtems/score/todimpl.h>
#include <rtems/score/watchdogimpl.h>

ISR_LOCK_DEFINE( static, _POSIX_signals_Alarm_lock, "POSIX Alarm" )

static void _POSIX_signals_Alarm_TSR( Watchdog_Control *the_watchdog )
{
  int status;

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
}

static Watchdog_Control _POSIX_signals_Alarm_watchdog = WATCHDOG_INITIALIZER(
  _POSIX_signals_Alarm_TSR
);

unsigned int alarm(
  unsigned int seconds
)
{
  unsigned int      remaining;
  Watchdog_Control *the_watchdog;
  ISR_lock_Context  lock_context;
  ISR_lock_Context  lock_context2;
  Per_CPU_Control  *cpu;
  uint64_t          now;
  uint32_t          ticks_per_second;
  uint32_t          ticks;

  the_watchdog = &_POSIX_signals_Alarm_watchdog;
  ticks_per_second = TOD_TICKS_PER_SECOND;
  ticks = seconds * ticks_per_second;

  _ISR_lock_ISR_disable_and_acquire(
    &_POSIX_signals_Alarm_lock,
    &lock_context
  );

  cpu = _Watchdog_Get_CPU( the_watchdog );
  _Watchdog_Per_CPU_acquire_critical( cpu, &lock_context2 );
  now = cpu->Watchdog.ticks;

  remaining = (unsigned long) _Watchdog_Cancel(
    &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ],
    the_watchdog,
    now
  );

  if ( ticks != 0 ) {
    _Watchdog_Insert(
      &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ],
      the_watchdog,
      now + ticks
    );
  }

  _Watchdog_Per_CPU_release_critical( cpu, &lock_context2 );
  _ISR_lock_Release_and_ISR_enable(
    &_POSIX_signals_Alarm_lock,
    &lock_context
  );

  return ( remaining + ticks_per_second - 1 ) / ticks_per_second;
}
