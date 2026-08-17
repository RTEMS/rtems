/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *  
 *  Instantiate the clock driver shell.
 *
 *  The TX3904 simulator in gdb counts instructions.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
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

#include <rtems.h>
#include <rtems/counter.h>
#include <rtems/timecounter.h>
#include <bsp/irq.h>
#include <bsp/jmr3904-counter.h>
#include <bsp.h>

#define CLOCK_DRIVER_USE_FAST_IDLE 1

#define CLOCK_VECTOR TX3904_IRQ_TMR0

static struct timecounter jmr3904_tc;

static uint32_t jmr3904_tc_get_timecount( struct timecounter *tc )
{
  (void) tc;
  return _CPU_Counter_read();
}

static void jmr3904_clock_initialize( void )
{
  TX3904_TIMER_WRITE( TX3904_TIMER0_BASE, TX3904_TIMER_CCDR, 0x3 );
  TX3904_TIMER_WRITE(
    TX3904_TIMER0_BASE,
    TX3904_TIMER_CPRA,
    JMR3904_TIMER_CLICKS_PER_TICK
  );
  TX3904_TIMER_WRITE( TX3904_TIMER0_BASE, TX3904_TIMER_TISR, 0x00 );
  TX3904_TIMER_WRITE( TX3904_TIMER0_BASE, TX3904_TIMER_ITMR, 0x8001 );
  TX3904_TIMER_WRITE( TX3904_TIMER0_BASE, TX3904_TIMER_TCR, 0xC0 );
  *( (volatile uint32_t *) 0xFFFFC01C ) = 0x00000700;

  /*
   * The free running counter is the time source.  It is already running, the
   * system initialization handler of the CPU counter started it.
   */
  jmr3904_tc.tc_get_timecount = jmr3904_tc_get_timecount;
  jmr3904_tc.tc_counter_mask = JMR3904_COUNTER_MASK;
  jmr3904_tc.tc_frequency = _CPU_Counter_frequency();
  jmr3904_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install( &jmr3904_tc );
}

#define Clock_driver_support_install_isr( _new ) \
  rtems_interrupt_handler_install( CLOCK_VECTOR, "clock", 0, _new, NULL )

#define Clock_driver_support_initialize_hardware() jmr3904_clock_initialize()


#include "../../../shared/dev/clock/clockimpl.h"
