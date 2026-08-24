/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsI386
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

#include <bsp.h>
#include <bsp/bspimpl.h>

#include <libcpu/cpuModel.h>

#include <rtems/sysinit.h>

/*
 * The frequency of the software counter below.  It is a nominal value: that
 * counter counts the calls to _CPU_Counter_read() and not the time.
 */
#define PC386_COUNTER_SOFTWARE_FREQUENCY 1000000000

uint64_t pc386_tsc_frequency;

uint32_t _CPU_Counter_frequency( void )
{
  if ( pc386_tsc_frequency != 0 ) {
    return (uint32_t) pc386_tsc_frequency;
  }

  return PC386_COUNTER_SOFTWARE_FREQUENCY;
}

/*
 * A processor without a time stamp counter gets a counter which counts the
 * calls to this function.  The counters of the 8254 are the only other ones of
 * this BSP.  They are 16 bits wide and a read of one takes three accesses to an
 * I/O port, so neither the range nor the cost of a read fits this interface.
 */
CPU_Counter_ticks _CPU_Counter_read( void )
{
  static CPU_Counter_ticks counter;

  CPU_Counter_ticks snapshot;

  if ( pc386_tsc_frequency != 0 ) {
    return (CPU_Counter_ticks) rdtsc();
  }

  snapshot = counter;
  counter = snapshot + 1;

  return snapshot;
}

/*
 * The ports and the command of channel 2 of the 8254.  Software gates that
 * channel and no interrupt takes its output, so the calibration owns it.  The
 * clock driver takes the tick interrupt from channel 0.
 */
#define PC386_COUNTER_PIT_CHAN2        0x42
#define PC386_COUNTER_PIT_MCR          0x43
#define PC386_COUNTER_PIT_GATE         0x61
#define PC386_COUNTER_PIT_GATE_TIMER   0x01
#define PC386_COUNTER_PIT_GATE_SPEAKER 0x02
#define PC386_COUNTER_PIT_SELECT_CHAN2 0x80
#define PC386_COUNTER_PIT_ACCESS_LOHI  0x30
#define PC386_COUNTER_PIT_ONE_SHOT     0x02

/*
 * The part of a second which the calibration measures.  The counter of the
 * 8254 is 16 bits wide, so the count of the interval must stay below 65536.
 */
#define PC386_COUNTER_CALIBRATION_DIVIDER 20

#define PC386_COUNTER_CALIBRATION_COUNTS \
  ( TIMER_TICK / PC386_COUNTER_CALIBRATION_DIVIDER )

RTEMS_STATIC_ASSERT(
  PC386_COUNTER_CALIBRATION_COUNTS <= 0xffff,
  PC386_COUNTER_CALIBRATION_DIVIDER
);

/*
 * Channel 2 runs in the hardware retriggerable one shot mode, so a rising
 * edge of the gate starts the count.  The counter counts down through zero
 * and continues at 0xffff, so a count above the load value shows that the
 * interval elapsed.
 */
static void pc386_counter_calibrate( void )
{
  uint64_t begin;
  uint64_t end;
  uint32_t count;
  uint8_t  gate;
  uint8_t  lsb;
  uint8_t  msb;

  inport_byte( PC386_COUNTER_PIT_GATE, gate );
  gate = ( gate | PC386_COUNTER_PIT_GATE_TIMER ) &
         ~PC386_COUNTER_PIT_GATE_SPEAKER;
  outport_byte( PC386_COUNTER_PIT_GATE, gate );

  outport_byte(
    PC386_COUNTER_PIT_MCR,
    PC386_COUNTER_PIT_SELECT_CHAN2 | PC386_COUNTER_PIT_ACCESS_LOHI |
      PC386_COUNTER_PIT_ONE_SHOT
  );
  outport_byte(
    PC386_COUNTER_PIT_CHAN2,
    PC386_COUNTER_CALIBRATION_COUNTS & 0xff
  );
  outport_byte(
    PC386_COUNTER_PIT_CHAN2,
    ( PC386_COUNTER_CALIBRATION_COUNTS >> 8 ) & 0xff
  );

  gate &= ~PC386_COUNTER_PIT_GATE_TIMER;
  outport_byte( PC386_COUNTER_PIT_GATE, gate );
  gate |= PC386_COUNTER_PIT_GATE_TIMER;
  outport_byte( PC386_COUNTER_PIT_GATE, gate );

  begin = rdtsc();

  do {
    outport_byte( PC386_COUNTER_PIT_MCR, PC386_COUNTER_PIT_SELECT_CHAN2 );
    inport_byte( PC386_COUNTER_PIT_CHAN2, lsb );
    inport_byte( PC386_COUNTER_PIT_CHAN2, msb );
    count = ( (uint32_t) msb << 8 ) | lsb;
  } while ( count <= PC386_COUNTER_CALIBRATION_COUNTS );

  end = rdtsc();

  gate &= ~PC386_COUNTER_PIT_GATE_TIMER;
  outport_byte( PC386_COUNTER_PIT_GATE, gate );

  pc386_tsc_frequency = ( end - begin ) * PC386_COUNTER_CALIBRATION_DIVIDER;
}

static void pc386_counter_initialize( void )
{
  if ( x86_has_tsc() ) {
    pc386_counter_calibrate();
  }
}

/*
 * getentropy() and the users of rtems_counter_delay_nanoseconds() need no
 * clock driver, so calibrate here rather than in the clock driver.
 */
RTEMS_SYSINIT_ITEM(
  pc386_counter_initialize,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_FIRST
);
