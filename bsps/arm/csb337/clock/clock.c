/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief AT91RM9200 clock specific using the System Timer
 */

/*
 * Copyright (C) 2003 Cogent Computer Systems
 * Written by Mike Kelly <mike@cogcomp.com>
 * and Jay Monkman <jtm@lopingdog.com>
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
#include <rtems/clockdrv.h>
#include <rtems/libio.h>

#include <stdlib.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <at91rm9200.h>
#include <at91rm9200_pmc.h>

/**
 * Enables clock interrupt.
 *
 * If the interrupt is always on, this can be a NOP.
 */
static void clock_isr_on(const rtems_irq_connect_data *unused)
{
  (void) unused;

  /* enable timer interrupt */
  ST_REG(ST_IER) = ST_SR_PITS;
}

/**
 * Disables clock interrupts
 *
 * If the interrupt is always on, this can be a NOP.
 */
static void clock_isr_off(const rtems_irq_connect_data *unused)
{
  (void) unused;

  /* disable timer interrupt */
  ST_REG(ST_IDR) = ST_SR_PITS;
}

/**
 * Tests to see if clock interrupt is enabled, and returns 1 if so.
 * If interrupt is not enabled, returns 0.
 *
 * If the interrupt is always on, this always returns 1.
 */
static int clock_isr_is_on(const rtems_irq_connect_data *irq)
{
  (void) irq;

  /* check timer interrupt */
  return ST_REG(ST_IMR) & ST_SR_PITS;
}

void Clock_isr(rtems_irq_hdl_param arg);

/* Replace the first value with the clock's interrupt name. */
rtems_irq_connect_data clock_isr_data = {
  .name   = AT91RM9200_INT_SYSIRQ,
  .hdl    = Clock_isr,
  .handle = NULL,
  .on     = clock_isr_on,
  .off    = clock_isr_off,
  .isOn   = clock_isr_is_on,
};


#define Clock_driver_support_install_isr( _new ) \
  BSP_install_rtems_irq_handler(&clock_isr_data)

static void Clock_driver_support_initialize_hardware(void)
{
  uint32_t st_str;
  int slck;
  unsigned long value;

  /* the system timer is driven from SLCK */
  slck = at91rm9200_get_slck();
  value = (((rtems_configuration_get_microseconds_per_tick() * slck) +
                      (1000000/2))/ 1000000);

  /* read the status to clear the int */
  st_str = ST_REG(ST_SR);
  (void) st_str; /* avoid set but not used warning */ \

  /* set priority */
  AIC_SMR_REG(AIC_SMR_SYSIRQ) = AIC_SMR_PRIOR(0x7);

  /* set the timer value */
  ST_REG(ST_PIMR) = value;
}

#define Clock_driver_support_at_tick(arg) \
  do { \
    uint32_t st_str; \
    \
    /* read the status to clear the int */ \
    st_str = ST_REG(ST_SR); \
    (void) st_str; /* avoid set but not used warning */ \
  } while (0)

#define CLOCK_DRIVER_USE_DUMMY_TIMECOUNTER

#include "../../../shared/dev/clock/clockimpl.h"
