/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2013.
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

#include <bsp.h>
#include <bsp/irq.h>
#include <bspopts.h>

/* XXX convert to macros? Move to score/cpu? */
void mips_set_timer(uint32_t timer_clock_interval);
uint32_t mips_get_timer(void);

/* XXX move to BSP.h or irq.h?? */
#define EXT_INT5             0x8000  /* external interrupt 5 */
#define CLOCK_VECTOR_MASK    EXT_INT5
#define CLOCK_VECTOR         (MIPS_INTERRUPT_BASE+0x7)

extern uint32_t bsp_clicks_per_microsecond;

static uint32_t mips_timer_rate = 0;

/* refresh the internal CPU timer */
#define Clock_driver_support_at_tick() \
  mips_set_timer( mips_timer_rate );

#define Clock_driver_support_install_isr( _new ) \
  rtems_interrupt_handler_install(CLOCK_VECTOR, "PIT clock",0, _new, NULL)

#define Clock_driver_support_initialize_hardware() \
  do { \
    mips_timer_rate = rtems_configuration_get_microseconds_per_tick() * \
      bsp_clicks_per_microsecond; \
    mips_set_timer( mips_timer_rate ); \
    mips_enable_in_interrupt_mask(CLOCK_VECTOR_MASK); \
  } while(0)

#define CLOCK_DRIVER_USE_DUMMY_TIMECOUNTER

#include "../../../shared/dev/clock/clockimpl.h"
