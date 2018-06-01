/*
 *  AT91RM9200 clock specific using the System Timer
 */

/*
 *  Copyright (c) 2003 by Cogent Computer Systems
 *  Written by Mike Kelly <mike@cogcomp.com>
 *         and Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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

#define Clock_driver_support_at_tick() \
  do { \
    uint32_t st_str; \
    \
    /* read the status to clear the int */ \
    st_str = ST_REG(ST_SR); \
    (void) st_str; /* avoid set but not used warning */ \
  } while (0)

#define CLOCK_DRIVER_USE_DUMMY_TIMECOUNTER

#include "../../../shared/dev/clock/clockimpl.h"
