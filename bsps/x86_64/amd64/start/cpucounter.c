/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsX8664AMD64
 *
 * @brief This source file contains the CPU counter support.
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

#include <apic.h>
#include <bsp.h>
#include <clock.h>

#include <rtems/sysinit.h>
#include <rtems/score/cpu.h>

static uint64_t amd64_tsc_ticks_per_sec;

uint64_t amd64_tsc_frequency( void )
{
  return amd64_tsc_ticks_per_sec;
}

uint32_t _CPU_Counter_frequency( void )
{
  return (uint32_t) amd64_tsc_ticks_per_sec;
}

CPU_Counter_ticks _CPU_Counter_read( void )
{
  return (CPU_Counter_ticks) amd64_rdtsc();
}

/*
 * Channel 2 of the PIT is gated by software and drives no interrupt, so the
 * calibration owns it.  The local APIC timer of the clock driver uses the
 * same channel later.
 */
static void amd64_counter_initialize( void )
{
  rtems_interrupt_level level;
  uint32_t              pit_ticks;
  uint64_t              begin;
  uint64_t              end;
  uint8_t               chan2_value;

  PIT_CHAN2_ENABLE( chan2_value );

  rtems_interrupt_local_disable( level );

  pit_ticks = PIT_CALIBRATE_TICKS;
  PIT_CHAN2_WRITE_TICKS( pit_ticks );

  PIT_CHAN2_START_DELAY( chan2_value );
  begin = amd64_rdtsc();

  PIT_CHAN2_WAIT_DELAY( pit_ticks );
  end = amd64_rdtsc();

  rtems_interrupt_local_enable( level );

  amd64_tsc_ticks_per_sec = ( end - begin ) * PIT_CALIBRATE_DIVIDER;
}

/*
 * getentropy() and the users of rtems_counter_delay_nanoseconds() need no
 * clock driver, so calibrate here rather than in the clock driver.
 */
RTEMS_SYSINIT_ITEM(
  amd64_counter_initialize,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_FIRST
);
