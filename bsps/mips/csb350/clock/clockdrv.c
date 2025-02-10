/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *  
 *  Instantiate the clock driver shell.
 *
 *  This uses the TOY (Time of Year) timer to implement the clock.
 */

/*
 * Copyright (C) 2005 by Cogent Computer Systems
 * Written by Jay Monkman <jtm@lopingdog.com>
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
#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/bspIo.h>

uint32_t tick_interval;
uint32_t last_match;

void au1x00_clock_init(void);

#define CLOCK_VECTOR AU1X00_IRQ_TOY_MATCH2

#define Clock_driver_support_at_tick(arg) \
  do { \
    while (AU1X00_SYS_CNTCTRL(AU1X00_SYS_ADDR) & AU1X00_SYS_CNTCTRL_TM0); \
    last_match = AU1X00_SYS_TOYREAD(AU1X00_SYS_ADDR); \
    AU1X00_SYS_TOYMATCH2(AU1X00_SYS_ADDR) = last_match + tick_interval; \
    au_sync(); \
  } while(0)

/* Set for rising edge interrupt */
#define Clock_driver_support_install_isr( _new ) \
  do { \
    rtems_interrupt_handler_install( \
      CLOCK_VECTOR, \
      "clock", \
      0, \
      _new, \
      NULL \
    ); \
    AU1X00_IC_MASKCLR(AU1X00_IC0_ADDR) = AU1X00_IC_IRQ_TOY_MATCH2; \
    AU1X00_IC_SRCSET(AU1X00_IC0_ADDR) = AU1X00_IC_IRQ_TOY_MATCH2; \
    AU1X00_IC_CFG0SET(AU1X00_IC0_ADDR) = AU1X00_IC_IRQ_TOY_MATCH2; \
    AU1X00_IC_CFG1CLR(AU1X00_IC0_ADDR) = AU1X00_IC_IRQ_TOY_MATCH2; \
    AU1X00_IC_CFG2CLR(AU1X00_IC0_ADDR) = AU1X00_IC_IRQ_TOY_MATCH2; \
    AU1X00_IC_ASSIGNSET(AU1X00_IC0_ADDR) = AU1X00_IC_IRQ_TOY_MATCH2; \
  } while(0)

void au1x00_clock_init(void)
{
    uint32_t wakemask;
    /* Clear the trim register */
    AU1X00_SYS_TOYTRIM(AU1X00_SYS_ADDR) = 0;

    /* Clear the TOY counter */
    while (AU1X00_SYS_CNTCTRL(AU1X00_SYS_ADDR) & AU1X00_SYS_CNTCTRL_TS);
    AU1X00_SYS_TOYWRITE(AU1X00_SYS_ADDR) = 0;
    while (AU1X00_SYS_CNTCTRL(AU1X00_SYS_ADDR) & AU1X00_SYS_CNTCTRL_TS);

    wakemask = AU1X00_SYS_WAKEMSK(AU1X00_SYS_ADDR);
    wakemask |= AU1X00_SYS_WAKEMSK_M20;
    AU1X00_SYS_WAKEMSK(AU1X00_SYS_ADDR) = wakemask;
    AU1X00_IC_WAKESET(AU1X00_IC0_ADDR) = AU1X00_IC_IRQ_TOY_MATCH2;

    tick_interval = 32768 * rtems_configuration_get_microseconds_per_tick();
    tick_interval = tick_interval / 1000000;

    last_match = AU1X00_SYS_TOYREAD(AU1X00_SYS_ADDR);
    AU1X00_SYS_TOYMATCH2(AU1X00_SYS_ADDR) = last_match + (50*tick_interval);
    AU1X00_IC_MASKSET(AU1X00_IC0_ADDR) = AU1X00_IC_IRQ_TOY_MATCH2;
    while (AU1X00_SYS_CNTCTRL(AU1X00_SYS_ADDR) & AU1X00_SYS_CNTCTRL_TM0);
}

#define Clock_driver_support_initialize_hardware() \
  do {                                                  \
     au1x00_clock_init(); \
  } while(0)

#define CLOCK_DRIVER_USE_DUMMY_TIMECOUNTER

#include "../../../shared/dev/clock/clockimpl.h"
