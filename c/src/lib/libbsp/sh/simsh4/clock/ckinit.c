/*
 *  Clock Driver for SH4 simulator (timer interrupt not supported now).
 *
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <stdlib.h>
#include <bsp.h>
#include <rtems/libio.h>

/*
 * Clock_driver_ticks is a monotonically increasing counter of the
 * number of clock ticks since the driver was initialized.
 */
volatile rtems_unsigned32 Clock_driver_ticks;


/*
 * These are set by clock driver during its init
 */
 
rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

rtems_isr (*rtems_clock_hook)(rtems_vector_number) = NULL;

static void
set_clock_period(rtems_unsigned32 period)
{
    asm volatile ("\tmov %0,r0\n"
                  "\ttrapa\t#4\n" 
                  : 
                  : "r" (period)
                  : "r0" );
}

/* Clock_isr --
 *     This handles the timer interrupt by clearing the timer's interrupt
 *     flag and announcing the clock tick to the system.
 *
 * PARAMETERS:
 *     vector - timer interrupt vector number
 *
 * RETURNS:
 *     none
 */
rtems_isr
Clock_isr (rtems_vector_number vector)
{
    /* Announce the clock tick */
    Clock_driver_ticks++;
    rtems_clock_tick();
}


/* Clock_exit --
 *     This shuts down the timer if it was enabled and removes it
 *     from the interrupt mask.
 *
 * PARAMETERS:
 *     none
 *
 * RETURNS:
 *     none
 */
void
Clock_exit(void)
{
    if (BSP_Configuration.ticks_per_timeslice)
    {
        /* disable all timer1 interrupts */
        set_clock_period(0);
    }
}


/* Install_clock --
 *     This initialises timer1 with the BSP timeslice config value
 *     as a reference and sets up the interrupt handler for clock ticks.
 *
 * PARAMETERS:
 *     clock_isr - clock interrupt handler routine
 *
 * RETURNS:
 *     none.
 */
static void
Install_clock(rtems_isr_entry clock_isr)
{
    rtems_unsigned32 period;
    Clock_driver_ticks = 0;
    if (BSP_Configuration.ticks_per_timeslice)
    {
        rtems_isr_entry  old_isr;
        period = Cpu_table.clicks_per_second / 
                 BSP_Configuration.ticks_per_timeslice;
        
        /* Configure timer interrupts */
        set_clock_period(period);
        
        /* Register the interrupt handler */
        rtems_interrupt_catch(clock_isr, CLOCK_VECTOR, &old_isr);
        
        /* Register the driver exit procedure so we can shutdown */
        atexit(Clock_exit);
    }
}


/* Clock_initialize --
 *     This is called to setup the clock driver. It calls the hardware
 *     setup function and make the driver major/minor values available
 *     for other.
 *
 * PARAMETERS:
 *     major - clock device major number
 *     minor - clock device minor number
 *     pargp - device driver initialization argument (not used)
 *
 * RETURNS:
 *     RTEMS status code
 */
rtems_device_driver
Clock_initialize(rtems_device_major_number major,
                 rtems_device_minor_number minor,
                 void *pargp)
{
    Install_clock (Clock_isr);
 
    /* Make major/minor avail to others such as shared memory driver */
    rtems_clock_major = major;
    rtems_clock_minor = minor;
 
    return RTEMS_SUCCESSFUL;
}
 

/* Clock_control --
 *     I/O control (IOCTL) function for Clock driver. At this moment this
 *     just runs the interrupt handler or re-registers the interrupt handler
 *     on request.
 *
 * PARAMETERS:
 *     major - clock major device number
 *     minor - clock minor device number
 *     pargp - pointer to IOCTL arguments
 *
 * RETURNS:
 *     RTEMS status code
 */
rtems_device_driver
Clock_control(rtems_device_major_number major,
              rtems_device_minor_number minor,
              void *pargp)
{
    rtems_unsigned32 isrlevel;
    rtems_libio_ioctl_args_t *args = pargp;

    if (args)
    {
        rtems_isr_entry  old_isr;
        /*
         * This is hokey, but until we get a defined interface
         * to do this, it will just be this simple...
         */
        if (args->command == rtems_build_name('I', 'S', 'R', ' ')) 
        {
            Clock_isr(CLOCK_VECTOR);
        }
        else if (args->command == rtems_build_name('N', 'E', 'W', ' '))
        {
            rtems_interrupt_disable( isrlevel );
            rtems_interrupt_catch(Clock_isr, CLOCK_VECTOR, &old_isr);
            rtems_interrupt_enable( isrlevel );
        }
    }
    return RTEMS_SUCCESSFUL;
}
