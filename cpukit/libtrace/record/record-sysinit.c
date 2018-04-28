/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2018, 2019 embedded brains GmbH
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/record.h>
#include <rtems/config.h>
#include <rtems/sysinit.h>
#include <rtems/score/timecounter.h>
#include <rtems/score/watchdogimpl.h>

static Watchdog_Interval _Record_Tick_interval;

void _Record_Initialize( void )
{
  uint32_t  cpu_max;
  uint32_t  cpu_index;
  uintptr_t offset;

  cpu_max = rtems_configuration_get_maximum_processors();
  offset = PER_CPU_DATA_OFFSET( _Record_Per_CPU );

  for ( cpu_index = 0; cpu_index < cpu_max; ++cpu_index ) {
    Per_CPU_Control *cpu;
    Record_Control  *control;

    cpu = _Per_CPU_Get_by_index( cpu_index );
    control = PER_CPU_DATA_GET_BY_OFFSET( cpu, Record_Control, offset );
    control->mask = _Record_Item_count - 1U;
    cpu->record = control;
  }
}

static void _Record_Watchdog( Watchdog_Control *watchdog )
{
  ISR_Level  level;
  sbintime_t now;

  _ISR_Local_disable( level );
  _Watchdog_Per_CPU_insert_ticks(
    watchdog,
    _Watchdog_Get_CPU( watchdog ),
    _Record_Tick_interval
  );
  _ISR_Local_enable( level );

  now = _Timecounter_Sbinuptime();
  rtems_record_produce_2(
    RTEMS_RECORD_UPTIME_LOW,
    (uint32_t) ( now >> 0 ),
    RTEMS_RECORD_UPTIME_HIGH,
    (uint32_t) ( now >> 32 )
  );
}

static void _Record_Initialize_watchdogs( void )
{
  Watchdog_Interval interval;
  uint32_t          cpu_max;
  uint32_t          cpu_index;
  sbintime_t        now;

  interval = rtems_counter_frequency() / _Watchdog_Ticks_per_second;
  interval = ( UINT32_C( 1 ) << 22 ) / interval;

  if ( interval == 0 ) {
    interval = 1;
  }

  _Record_Tick_interval = interval;

  cpu_max = rtems_configuration_get_maximum_processors();

  for ( cpu_index = 0; cpu_index < cpu_max; ++cpu_index ) {
    Per_CPU_Control *cpu;
    Record_Control  *control;

    cpu = _Per_CPU_Get_by_index( cpu_index );
    control = cpu->record;
    _Watchdog_Preinitialize( &control->Watchdog, cpu );
    _Watchdog_Initialize( &control->Watchdog, _Record_Watchdog );
    _Watchdog_Per_CPU_insert_ticks(
      &control->Watchdog,
      cpu,
      _Record_Tick_interval
    );
  }

  now = _Timecounter_Sbinuptime();
  rtems_record_produce_2(
    RTEMS_RECORD_UPTIME_LOW,
    (uint32_t) ( now >> 0 ),
    RTEMS_RECORD_UPTIME_HIGH,
    (uint32_t) ( now >> 32 )
  );
}

RTEMS_SYSINIT_ITEM(
  _Record_Initialize_watchdogs,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_LAST
);
