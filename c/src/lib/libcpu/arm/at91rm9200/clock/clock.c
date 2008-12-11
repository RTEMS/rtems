/*
 *  AT91RM9200 clock specific using the System Timer
 *
 *  Copyright (c) 2003 by Cogent Computer Systems
 *  Written by Mike Kelly <mike@cogcomp.com>
 *         and Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.com/license/LICENSE.
 *
 *
 *  $Id$
 */
#include <rtems.h>
#include <rtems/clockdrv.h>
#include <rtems/libio.h>

#include <stdlib.h>
#include <bsp.h>
#include <irq.h>
#include <at91rm9200.h>
#include <at91rm9200_pmc.h>


static unsigned long st_pimr_reload;

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
    return;
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

rtems_isr Clock_isr(rtems_vector_number vector);

/* Replace the first value with the clock's interrupt name. */
rtems_irq_connect_data clock_isr_data = {AT91RM9200_INT_SYSIRQ,   
                                         (rtems_irq_hdl)Clock_isr,
                                         clock_isr_on,
                                         clock_isr_off,
                                         clock_isr_is_on,
                                         3,     /* unused for ARM cpus */
                                         0 };   /* unused for ARM cpus */


#define Clock_driver_support_install_isr( _new, _old ) \
  BSP_install_rtems_irq_handler(&clock_isr_data)

void Clock_driver_support_initialize_hardware(void)
{
  uint32_t st_str;
  int slck;

  /* the system timer is driven from SLCK */
  slck = at91rm9200_get_slck();
  st_pimr_reload =
    (((rtems_configuration_get_microseconds_per_tick() * slck) + (1000000/2))/ 1000000);

  /* read the status to clear the int */ 
  st_str = ST_REG(ST_SR);
    
  /* set priority */
  AIC_SMR_REG(AIC_SMR_SYSIRQ) = AIC_SMR_PRIOR(0x7); 

  /* set the timer value */
  ST_REG(ST_PIMR) = st_pimr_reload;
}


#define CLOCK_VECTOR 0

#define Clock_driver_support_at_tick() \
  do { \
    uint32_t st_str; \
    \
    /* read the status to clear the int */ \
    st_str = ST_REG(ST_SR); \
  } while (0)

void Clock_driver_support_shutdown_hardware( void )
{
    BSP_remove_rtems_irq_handler(&clock_isr_data);
}

#include "../../../../libbsp/shared/clockdrv_shell.h"
