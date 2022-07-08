/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Clock device driver for Lattice Mico32 (lm32).
 */

/*
 *  COPYRIGHT (c) 1989-2009.
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
 *
 *  Jukka Pietarinen <jukka.pietarinen@mrf.fi>, 2008,
 *  Micro-Research Finland Oy
 */

#include <bsp.h>
#include "../include/system_conf.h"
#include "clock.h"
#include "bspopts.h"

#if LM32_ON_SIMULATOR
#define CLOCK_DRIVER_USE_FAST_IDLE 1
#endif

static inline int clockread(unsigned int reg)
{
  return *((int*)(TIMER0_BASE_ADDRESS + reg));
}

static inline void clockwrite(unsigned int reg, int value)
{
  *((int*)(TIMER0_BASE_ADDRESS + reg)) = value;
}

/*
 *  The interrupt vector number associated with the clock tick device
 *  driver.
 */
#define CLOCK_VECTOR    ( TIMER0_IRQ )
#define CLOCK_IRQMASK   ( 1 << CLOCK_VECTOR )

#define Clock_driver_support_at_tick() \
  do { \
    /* Clear overflow flag */ \
    clockwrite(LM32_CLOCK_SR, 0); \
    lm32_interrupt_ack(CLOCK_IRQMASK); \
  } while (0)

#define Clock_driver_support_install_isr(_new ) \
  set_vector( _new, CLOCK_VECTOR, 1 )

static void Clock_driver_support_initialize_hardware(void)
{
  /* Set clock period */
  clockwrite(LM32_CLOCK_PERIOD,
	     (CPU_FREQUENCY /
	      (1000000 / rtems_configuration_get_microseconds_per_tick())));

  /* Enable clock interrupts and start in continuous mode */
  clockwrite(LM32_CLOCK_CR, LM32_CLOCK_CR_ITO |
	     LM32_CLOCK_CR_CONT |
	     LM32_CLOCK_CR_START);

  lm32_interrupt_unmask(CLOCK_IRQMASK);
}

#define CLOCK_DRIVER_USE_DUMMY_TIMECOUNTER

#include "../../../shared/dev/clock/clockimpl.h"

