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
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <stdlib.h>
#include <bsp.h>
#include <rtems/libio.h>

extern uint32_t bsp_clicks_per_second;

/*
 * Clock_driver_ticks is a monotonically increasing counter of the
 * number of clock ticks since the driver was initialized.
 */
volatile uint32_t         Clock_driver_ticks;

/*
 * These are set by clock driver during its init
 */

rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

rtems_isr (*rtems_clock_hook)(rtems_vector_number) = NULL;

static void
set_clock_period(uint32_t         period)
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
    if (rtems_configuration_get_ticks_per_timeslice())
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
  uint32_t         period;

  Clock_driver_ticks = 0;
  if (rtems_configuration_get_ticks_per_timeslice()) {
      rtems_isr_entry  old_isr;
      period = bsp_clicks_per_second / rtems_configuration_get_ticks_per_timeslice();

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
