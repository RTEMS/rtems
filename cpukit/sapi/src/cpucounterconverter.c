/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImpl
 *
 * @brief This source file contains a implementation of the counter value
 *   conversion functions.
 */

/*
 * Copyright (C) 2014, 2018 embedded brains GmbH & Co. KG
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
  return (rtems_counter_ticks) (((uint64_t) sbt * from_sbt_scaler) >> 32);
}

void rtems_counter_initialize_converter( uint32_t frequency )
{
  uint64_t ns_per_s = UINT64_C(1000000000);
  uint64_t bin_per_s = UINT64_C(1) << 32;
  uint64_t bin_freq = (uint64_t) frequency << 32;

  to_ns_scaler = ((ns_per_s << 32) + frequency - 1) / frequency;
  from_ns_scaler = (bin_freq + ns_per_s - 1) / ns_per_s;

  to_sbt_scaler = ((bin_per_s << 31) + frequency - 1) / frequency;
  from_sbt_scaler = (bin_freq + bin_per_s - 1) / bin_per_s;
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
