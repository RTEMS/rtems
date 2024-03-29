/* SPDX-License-Identifier: BSD-2-Clause */

/*  ckinit.c
 *
 *  This file provides a template for the clock device driver initialization.
 *
 *  Modified for sun4v - niagara
 */

/*
 *  Copyright (c) 2010 Gedare Bloom.
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

#include <stdlib.h>

#include <rtems.h>
#include <bsp.h>
#include <bspopts.h>
#include <boot/ofw.h>
#include <rtems/bspIo.h>

/* This is default frequency for simics simulator of niagara. Use the
 * get_Frequency function to determine the CPU clock frequency at runtime.
 */
#define CPU_FREQ (5000000)

uint64_t sparc64_cycles_per_tick;

/* TICK_CMPR and STICK_CMPR trigger soft interrupt 14 */
#define CLOCK_VECTOR SPARC_SYNCHRONOUS_TRAP(0x4E)

static unsigned int get_Frequency(void)
{
  phandle root = ofw_find_device("/");
  unsigned int freq;

  if (ofw_get_property(root, "clock-frequency", &freq, sizeof(freq)) <= 0) {
    printk("Unable to determine frequency, default: 0x%x\n",CPU_FREQ);
    return CPU_FREQ;
  }

  return freq;
}

#define Clock_driver_support_at_tick(arg) \
  Clock_driver_support_at_tick_helper()

static void Clock_driver_support_at_tick_helper(void)
{
  uint64_t tick_reg;
  int bit_mask;
  uint64_t pil_reg;

  bit_mask = SPARC_SOFTINT_TM_MASK | SPARC_SOFTINT_SM_MASK | (1<<14);
  sparc64_clear_interrupt_bits(bit_mask);

  sparc64_get_pil(pil_reg);
  if(pil_reg == 0xe) { /* 0xe is the tick compare interrupt (softint(14)) */
    pil_reg--;
    sparc64_set_pil(pil_reg); /* enable the next timer interrupt */
  }
  /* Note: sun4v uses stick_cmpr for clock driver for M5 simulator, which 
   * does not currently have tick_cmpr implemented */
  /* TODO: this could be more efficiently implemented as a single assembly 
   * inline */
#if defined (SUN4U)
  sparc64_read_tick(tick_reg);
#elif defined (SUN4V)
  sparc64_read_stick(tick_reg);
#endif
  tick_reg &= ~(1UL<<63); /* mask out NPT bit, prevents int_dis from being set */

  tick_reg += sparc64_cycles_per_tick;

#if defined (SUN4U)
  sparc64_write_tick_cmpr(tick_reg);
#elif defined (SUN4V)
  sparc64_write_stick_cmpr(tick_reg);
#endif
}

#define Clock_driver_support_install_isr(_new) \
  set_vector( _new, CLOCK_VECTOR, 1 )

static void Clock_driver_support_initialize_hardware(void)
{
  uint64_t tick_reg;
  int bit_mask;

  bit_mask = SPARC_SOFTINT_TM_MASK | SPARC_SOFTINT_SM_MASK | (1<<14);
  sparc64_clear_interrupt_bits(bit_mask);

  sparc64_cycles_per_tick =
    rtems_configuration_get_microseconds_per_tick()*(get_Frequency()/1000000);

#if defined (SUN4U)
  sparc64_read_tick(tick_reg);
#elif defined (SUN4V)
  sparc64_read_stick(tick_reg);
#endif

  tick_reg &= ~(1UL<<63); /* mask out NPT bit, prevents int_dis from being set */
  tick_reg += sparc64_cycles_per_tick;

#if defined (SUN4U)
  sparc64_write_tick_cmpr(tick_reg);
#elif defined (SUN4V)
  sparc64_write_stick_cmpr(tick_reg);
#endif
}

#define CLOCK_DRIVER_USE_DUMMY_TIMECOUNTER

#include "../../../shared/dev/clock/clockimpl.h"

