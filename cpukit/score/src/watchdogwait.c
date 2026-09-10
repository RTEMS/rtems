/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreWatchdog
 *
 * @brief This source file contains the implementation of
 *   _Watchdog_Wait_for_service_stop().
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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
#include <rtems/score/assert.h>
#include <rtems/score/isrlevel.h>
#include <rtems/score/threaddispatch.h>

void _Watchdog_Wait_for_service_stop( const Watchdog_Control *the_watchdog )
{
  Per_CPU_Control *cpu;
  unsigned int     generation;

  /*
   * A caller which holds a lock of a collection runs with interrupts
   * disabled.  A caller in a tickle phase runs with thread dispatching
   * disabled, and it would wait for the phase which it runs in itself.
   */
  _Assert( _ISR_Get_level() == 0 );
  _Assert( _Thread_Dispatch_is_enabled() );

  /*
   * The tickle phase which may run the routine of this watchdog belongs to
   * the collection which held it.  A routine which schedules the watchdog
   * again uses the processor on which it runs, so the member names that
   * processor for the time of the phase.
   */
  cpu = _Watchdog_Get_CPU( the_watchdog );
  generation = _Atomic_Load_uint(
    &cpu->Watchdog.generation,
    ATOMIC_ORDER_ACQUIRE
  );

  if ( ( generation & 1U ) == 0U ) {
    return;
  }

  /*
   * The wait ends with the phase which this party found and not with an even
   * count.  A processor under load begins the next phase at once, so a wait
   * for an even count could go on for ever.
   */
  while (
    _Atomic_Load_uint( &cpu->Watchdog.generation, ATOMIC_ORDER_ACQUIRE ) ==
    generation
  ) {
    /* Wait until the phase which runs ends. */
  }
}
