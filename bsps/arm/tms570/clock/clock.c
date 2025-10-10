/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This source file contains the Clock Driver implementation.
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
 * Copyright (C) 2014 Premysl Houdek <kom541000@gmail.com>
 *
 * Google Summer of Code 2014 at
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
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
#include <bsp/fatal.h>
#include <bsp/irq.h>
#include <bsp/tms570.h>
#include <rtems/counter.h>
#include <rtems/sysinit.h>
#include <rtems/timecounter.h>

static struct timecounter tms570_rti_tc;

uint32_t _CPU_Counter_frequency(void)
{
  return TMS570_RTICLK_HZ / 2;
}

CPU_Counter_ticks _CPU_Counter_read(void)
{
  return TMS570_RTI.CNT[0].FRCx;
}

static void tms570_rti_initialize( void )
{
  /* Initialize module */
  TMS570_RTI.GCTRL = 0;
  TMS570_RTI.CAPCTRL = 0;
  TMS570_RTI.COMPCTRL = 0;
  TMS570_RTI.TBCTRL = TMS570_RTI_TBCTRL_INC;
  TMS570_RTI.INTCLRENABLE = 0x05050505;

  /* Disable interrupts */
  TMS570_RTI.CLEARINTENA = TMS570_RTI_CLEARINTENA_CLEAROVL1INT |
                           TMS570_RTI_CLEARINTENA_CLEAROVL0INT |
                           TMS570_RTI_CLEARINTENA_CLEARTBINT |
                           TMS570_RTI_CLEARINTENA_CLEARDMA3 |
                           TMS570_RTI_CLEARINTENA_CLEARDMA2 |
                           TMS570_RTI_CLEARINTENA_CLEARDMA1 |
                           TMS570_RTI_CLEARINTENA_CLEARDMA0 |
                           TMS570_RTI_CLEARINTENA_CLEARINT3 |
                           TMS570_RTI_CLEARINTENA_CLEARINT2 |
                           TMS570_RTI_CLEARINTENA_CLEARINT1 |
                           TMS570_RTI_CLEARINTENA_CLEARINT0;

  /* Initialize counter 0 */
  TMS570_RTI.CNT[0].CPUCx = 1;
  TMS570_RTI.CNT[0].UCx = 0;
  TMS570_RTI.CNT[0].FRCx = 0;

  /* Enable counter 0 */
  TMS570_RTI.GCTRL = TMS570_RTI_GCTRL_CNT0EN;
}

RTEMS_SYSINIT_ITEM(
  tms570_rti_initialize,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

static uint32_t tms570_rti_get_timecount(struct timecounter *tc)
{
  (void) tc;

  return TMS570_RTI.CNT[0].FRCx;
}

static void tms570_clock_driver_support_initialize_hardware( void )
{

  uint64_t usec_per_tick;
  uint32_t tc_frequency;
  uint32_t tc_increments_per_tick;
  struct timecounter *tc;

  usec_per_tick = rtems_configuration_get_microseconds_per_tick();
  tc_frequency = TMS570_RTICLK_HZ / 2;
  tc_increments_per_tick = (usec_per_tick * tc_frequency + 500000) / 1000000;

  /* Initialize compare 0 */
  TMS570_RTI.CMP[0].UDCPx = tc_increments_per_tick;
  TMS570_RTI.CMP[0].COMPx = TMS570_RTI.CNT[0].FRCx + tc_increments_per_tick;

  /* Clear interrupts */
  TMS570_RTI.INTFLAG = TMS570_RTI_INTFLAG_OVL1INT |
                       TMS570_RTI_INTFLAG_OVL0INT |
                       TMS570_RTI_INTFLAG_TBINT |
                       TMS570_RTI_INTFLAG_INT3 |
                       TMS570_RTI_INTFLAG_INT2 |
                       TMS570_RTI_INTFLAG_INT1 |
                       TMS570_RTI_INTFLAG_INT0;

  /* Enable interrupts for counter 0 */
  TMS570_RTI.SETINTENA = TMS570_RTI_SETINTENA_SETINT0;

  tc = &tms570_rti_tc;
  tc->tc_get_timecount = tms570_rti_get_timecount;
  tc->tc_counter_mask = 0xffffffff;
  tc->tc_frequency = tc_frequency;
  tc->tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(tc);
}

static void tms570_clock_driver_support_at_tick(volatile tms570_rti_t *rti)
{
  rti->INTFLAG = TMS570_RTI_INTFLAG_INT0;
}

static void tms570_clock_driver_support_install_isr(
  rtems_interrupt_handler handler
)
{
  rtems_status_code sc;

  sc = rtems_interrupt_handler_install(
    TMS570_IRQ_TIMER_0,
    "Clock",
    RTEMS_INTERRUPT_UNIQUE,
    handler,
    RTEMS_DEVOLATILE(tms570_rti_t *, &TMS570_RTI)
  );
  if (sc != RTEMS_SUCCESSFUL) {
    bsp_fatal(TMS570_FATAL_RTI_IRQ_INSTALL);
  }
}

#define Clock_driver_support_initialize_hardware() \
  tms570_clock_driver_support_initialize_hardware()
#define Clock_driver_support_at_tick(arg) \
  tms570_clock_driver_support_at_tick(arg)
#define Clock_driver_support_install_isr(handler) \
  tms570_clock_driver_support_install_isr(handler)

#include "../../../shared/dev/clock/clockimpl.h"
