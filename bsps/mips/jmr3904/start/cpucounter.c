/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsMIPSJMR3904
 *
 * @brief This source file contains the CPU counter implementation.
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
#include <bsp/jmr3904-counter.h>

#include <rtems/sysinit.h>

uint32_t _CPU_Counter_frequency( void )
{
  return JMR3904_COUNTER_FREQUENCY;
}

/*
 * The counter is 24 bits wide, so a difference taken over more than one wrap
 * of it is wrong.  At the frequency of this BSP that is a measurement of more
 * than thirty seconds, which no user of this interface takes.
 */
CPU_Counter_ticks _CPU_Counter_read( void )
{
  return TX3904_TIMER_READ( JMR3904_COUNTER_BASE, TX3904_TIMER_TRR );
}

static void jmr3904_counter_initialize( void )
{
  /*
   * Run the timer as a free running up counter: it wraps at the width of its
   * counter, so it needs no interrupt to carry a period and a reader never
   * sees it go backwards.  Timer 0 is the clock tick and uses the same
   * divider, which is what ties the frequency to the clicks per tick.
   */
  TX3904_TIMER_WRITE( JMR3904_COUNTER_BASE, TX3904_TIMER_TCR, 0x20 );
  TX3904_TIMER_WRITE( JMR3904_COUNTER_BASE, TX3904_TIMER_CCDR, 0x3 );
  TX3904_TIMER_WRITE( JMR3904_COUNTER_BASE, TX3904_TIMER_TRR, 0x0 );
  TX3904_TIMER_WRITE(
    JMR3904_COUNTER_BASE,
    TX3904_TIMER_CPRA,
    JMR3904_COUNTER_MASK
  );
  TX3904_TIMER_WRITE( JMR3904_COUNTER_BASE, TX3904_TIMER_TISR, 0x00 );
  TX3904_TIMER_WRITE( JMR3904_COUNTER_BASE, TX3904_TIMER_ITMR, 0x0001 );
  TX3904_TIMER_WRITE( JMR3904_COUNTER_BASE, TX3904_TIMER_TCR, 0xC0 );
}

/*
 * The benchmark timer and getentropy() read the counter and neither of them
 * needs the clock driver, so start it here rather than in the clock driver.
 */
RTEMS_SYSINIT_ITEM(
  jmr3904_counter_initialize,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_FIRST
);
