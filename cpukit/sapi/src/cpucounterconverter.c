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

uint64_t rtems_counter_ticks_to_nanoseconds( rtems_counter_ticks counter )
{
  return (uint32_t) ((counter * to_ns_scaler) >> 32);
}

rtems_counter_ticks rtems_counter_nanoseconds_to_ticks( uint32_t nanoseconds )
{
  return (rtems_counter_ticks) ((nanoseconds * from_ns_scaler) >> 32);
}

void rtems_counter_initialize_converter( uint32_t frequency )
{
  uint64_t ns_per_s = UINT64_C(1000000000);

  to_ns_scaler = ((ns_per_s << 32) + frequency - 1) / frequency;
  from_ns_scaler = ((UINT64_C(1) << 32) * frequency + ns_per_s - 1) / ns_per_s;
}

static void rtems_counter_sysinit( void )
{
  rtems_counter_initialize_converter( rtems_counter_frequency() );
}

RTEMS_SYSINIT_ITEM(
  rtems_counter_sysinit,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_LAST
);
