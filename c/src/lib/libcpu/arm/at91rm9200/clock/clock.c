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
 *  http://www.OARcorp.com/rtems/license.html.
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


rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;
volatile uint32_t Clock_driver_ticks;
static unsigned long st_pimr_reload;

rtems_isr clock_isr(rtems_vector_number vector);
void Clock_exit(void);
static void clock_isr_on(const rtems_irq_connect_data *unused);
static void clock_isr_off(const rtems_irq_connect_data *unused);
static int clock_isr_is_on(const rtems_irq_connect_data *irq);

/* Replace the first value with the clock's interrupt name. */
rtems_irq_connect_data clock_isr_data = {AT91RM9200_INT_SYSIRQ,   
                                         (rtems_irq_hdl)clock_isr,
                                         clock_isr_on,
                                         clock_isr_off,
                                         clock_isr_is_on,
                                         3,     /* unused for ARM cpus */
                                         0 };   /* unused for ARM cpus */


rtems_isr clock_isr(rtems_vector_number vector)
{
    uint32_t st_str;

    Clock_driver_ticks++;

    /* read the status to clear the int */
    st_str = ST_REG(ST_SR);

    /* reload the timer value */
    ST_REG(ST_PIMR) = st_pimr_reload; 

    rtems_clock_tick();
}

void Install_clock(rtems_isr_entry clock_isr)
{
    uint32_t st_str;
    int slck;

    Clock_driver_ticks = 0;

    BSP_install_rtems_irq_handler(&clock_isr_data);

    /* the system timer is driven from SLCK */
    slck = at91rm9200_get_slck();
    st_pimr_reload = ((BSP_Configuration.microseconds_per_tick * 1000) /
                      slck);

    /* read the status to clear the int */ 
    st_str = ST_REG(ST_SR);
    
    /* set priority */
    AIC_SMR_REG(AIC_SMR_SYSIRQ) = AIC_SMR_PRIOR(0x7); 

    /* set the timer value */
    ST_REG(ST_PIMR) = st_pimr_reload;

    atexit( Clock_exit );
}

void Clock_exit( void )
{
    BSP_remove_rtems_irq_handler(&clock_isr_data);
}

rtems_device_driver Clock_initialize(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *pargp
    )
{
    Install_clock( clock_isr );
 
    rtems_clock_major = major;
    rtems_clock_minor = minor;

    return RTEMS_SUCCESSFUL;
}

rtems_device_driver Clock_control(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *pargp
)
{
    uint32_t  isrlevel;
    rtems_libio_ioctl_args_t *args = pargp;
    
    if (args == 0) {
        return RTEMS_SUCCESSFUL;
    }
 
    if (args->command == rtems_build_name('I', 'S', 'R', ' ')) {
        clock_isr(0);
    } else if (args->command == rtems_build_name('N', 'E', 'W', ' ')) {
        rtems_interrupt_disable(isrlevel);

        BSP_install_rtems_irq_handler(args->buffer);

        rtems_interrupt_enable(isrlevel);
    }
 
    return RTEMS_SUCCESSFUL;
}

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
