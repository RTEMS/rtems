/*
 * Copyright (c) 2014, 2018 embedded brains GmbH.  All rights reserved.
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

#include <rtems/counter.h>
#include <rtems/sysinit.h>

RTEMS_STATIC_ASSERT(sizeof(rtems_counter_ticks) <= sizeof(uint32_t), type);

static uint64_t to_ns_scaler;

static uint64_t from_ns_scaler;

static uint64_t to_sbt_scaler;

static uint64_t from_sbt_scaler;

uint64_t rtems_counter_ticks_to_nanoseconds( rtems_counter_ticks ticks )
{
  return (uint32_t) ((ticks * to_ns_scaler) >> 32);
}

rtems_counter_ticks rtems_counter_nanoseconds_to_ticks( uint32_t nanoseconds )
{
  return (rtems_counter_ticks) ((nanoseconds * from_ns_scaler) >> 32);
}

int64_t rtems_counter_ticks_to_sbintime( rtems_counter_ticks ticks )
{
  return (int64_t) ((ticks * to_sbt_scaler) >> 31);
}

rtems_counter_ticks rtems_counter_sbintime_to_ticks( int64_t sbt )
{
  return (rtems_counter_ticks) (((uint64_t) sbt * from_sbt_scaler) >> 31);
}

void rtems_counter_initialize_converter( uint32_t frequency )
{
  uint64_t ns_per_s = UINT64_C(1000000000);
  uint64_t bin_per_s = UINT64_C(1) << 32;

  to_ns_scaler = ((ns_per_s << 32) + frequency - 1) / frequency;
  from_ns_scaler = ((UINT64_C(1) << 32) * frequency + ns_per_s - 1) / ns_per_s;

  to_sbt_scaler = ((bin_per_s << 31) + frequency - 1) / frequency;
  from_sbt_scaler = ((UINT64_C(1) << 31) * frequency + bin_per_s - 1) / bin_per_s;
}

static void rtems_counter_sysinit( void )
{
  rtems_counter_initialize_converter( rtems_counter_frequency() );
}

RTEMS_SYSINIT_ITEM(
  rtems_counter_sysinit,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_LAST_BUT_5
);
