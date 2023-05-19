/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This source file contains the definition of SetGetTimecountHandler(),
 *   GetTimecountCounter(), and SetTimecountCounter().
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tx-support.h"

#include <rtems/sysinit.h>
#include <rtems/timecounter.h>
#include <rtems/score/atomic.h>
#include <rtems/score/percpu.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/threaddispatch.h>

typedef struct {
  struct timecounter base;
  GetTimecountHandler handler;
  Atomic_Ulong counter;
} TimecounterControl;

static TimecounterControl TimecounterInstance;

GetTimecountHandler SetGetTimecountHandler( GetTimecountHandler handler )
{
  GetTimecountHandler previous;

  previous = TimecounterInstance.handler;
  TimecounterInstance.handler = handler;
  return previous;
}

uint32_t GetTimecountCounter( void )
{
  return (uint32_t) _Atomic_Load_ulong(
    &TimecounterInstance.counter,
    ATOMIC_ORDER_RELAXED
  );
}

uint32_t SetTimecountCounter( uint32_t counter )
{
  return (uint32_t) _Atomic_Exchange_ulong(
    &TimecounterInstance.counter,
    counter,
    ATOMIC_ORDER_RELAXED
  );
}

static uint32_t GetTimecountSoftware( void )
{
  return (uint32_t) _Atomic_Fetch_add_ulong(
    &TimecounterInstance.counter,
    1,
    ATOMIC_ORDER_RELAXED
  );
}

static uint32_t GetTimecountWrapper( struct timecounter *tc )
{
  TimecounterControl *self;

  self = (TimecounterControl *) tc;
  return ( *self->handler )();
}

static void InstallTimecounter( void )
{
  TimecounterControl *self;

  self = &TimecounterInstance;
  self->handler = GetTimecountSoftware;
  self->base.tc_get_timecount = GetTimecountWrapper;
  self->base.tc_counter_mask = 0xffffffff;
  self->base.tc_frequency = SOFTWARE_TIMECOUNTER_FREQUENCY;
  self->base.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER + 1;
  rtems_timecounter_install( &self->base );
}

RTEMS_SYSINIT_ITEM(
  InstallTimecounter,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_LAST
);

static void DoTimecounterTick( void *arg )
{
  (void) arg;
  _Timecounter_Tick();
}

void TimecounterTick( void )
{
  unsigned long    counter_ticks_per_clock_tick;
  Per_CPU_Control *cpu_self;
  bool             success;

  counter_ticks_per_clock_tick =
    SOFTWARE_TIMECOUNTER_FREQUENCY / rtems_clock_get_ticks_per_second();
  cpu_self = _Thread_Dispatch_disable();

  do {
    unsigned long old_counter;
    unsigned long new_counter;

    old_counter = _Atomic_Load_ulong(
      &TimecounterInstance.counter,
      ATOMIC_ORDER_RELAXED
    );
    new_counter = old_counter + counter_ticks_per_clock_tick -
      ( old_counter % counter_ticks_per_clock_tick );
    success = _Atomic_Compare_exchange_ulong(
      &TimecounterInstance.counter,
      &old_counter,
      new_counter,
      ATOMIC_ORDER_RELAXED,
      ATOMIC_ORDER_RELAXED
    );
  } while ( !success );

  DoTimecounterTick( NULL );
#if defined( RTEMS_SMP )
  _SMP_Othercast_action( DoTimecounterTick, NULL );
#endif
  _Thread_Dispatch_enable( cpu_self );
}
