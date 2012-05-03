/*
 *  Clock Driver for MCF5206eLITE board
 *
 *  This driver initailizes timer1 on the MCF5206E as the
 *  main system clock
 *
 *  Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 *  Based on work:
 *    David Fiddes, D.J@fiddes.surfaid.org
 *    http://www.calm.hw.ac.uk/davidf/coldfire/
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <stdlib.h>
#include <bsp.h>
#include <rtems/libio.h>
#include "mcf5206/mcf5206e.h"

/*
 * Clock_driver_ticks is a monotonically increasing counter of the
 * number of clock ticks since the driver was initialized.
 */
volatile uint32_t   Clock_driver_ticks;


/*
 * These are set by clock driver during its init
 */

rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

rtems_isr (*rtems_clock_hook)(rtems_vector_number) = NULL;

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
  /* Clear pending interrupt... */
  *MCF5206E_TER(MBAR,1) = MCF5206E_TER_REF | MCF5206E_TER_CAP;

  /* Announce the clock tick */
  Clock_driver_ticks++;
  rtems_clock_tick();
  if (rtems_clock_hook != NULL)
      rtems_clock_hook(vector);
}


/* Clock_exit --
 *     This shuts down the timer if it was enabled and removes it
 *     from the MCF5206E interrupt mask.
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
        *MCF5206E_IMR(MBAR) |= MCF5206E_INTR_BIT(MCF5206E_INTR_TIMER_1);

        /* reset timer1 */
        *MCF5206E_TMR(MBAR,1) = MCF5206E_TMR_ICLK_STOP;

        /* clear pending */
        *MCF5206E_TER(MBAR,1) = MCF5206E_TER_REF | MCF5206E_TER_CAP;
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
    Clock_driver_ticks = 0;
    if (rtems_configuration_get_ticks_per_timeslice())
    {
        /* Configure timer1 interrupts */
        *MCF5206E_ICR(MBAR,MCF5206E_INTR_TIMER_1) =
            MCF5206E_ICR_AVEC |
            ((BSP_INTLVL_TIMER1 << MCF5206E_ICR_IL_S) & MCF5206E_ICR_IL) |
            ((BSP_INTPRIO_TIMER1 << MCF5206E_ICR_IP_S) & MCF5206E_ICR_IP);

        /* Register the interrupt handler */
        set_vector(clock_isr, BSP_INTVEC_TIMER1, 1);

        /* Reset timer 1 */
        *MCF5206E_TMR(MBAR, 1) = MCF5206E_TMR_RST;
        *MCF5206E_TMR(MBAR, 1) = MCF5206E_TMR_ICLK_STOP;
        *MCF5206E_TMR(MBAR, 1) = MCF5206E_TMR_RST;
        *MCF5206E_TCN(MBAR, 1) = 0; /* Reset counter */
        *MCF5206E_TER(MBAR, 1) = MCF5206E_TER_REF | MCF5206E_TER_CAP;

        /* Set Timer 1 prescaler so that it counts in microseconds */
        *MCF5206E_TMR(MBAR, 1) =
            (((BSP_SYSTEM_FREQUENCY/1000000 - 1) << MCF5206E_TMR_PS_S) &
             MCF5206E_TMR_PS) |
            MCF5206E_TMR_CE_NONE | MCF5206E_TMR_ORI | MCF5206E_TMR_FRR |
            MCF5206E_TMR_RST;

        /* Set the timer timeout value from the BSP config */
        *MCF5206E_TRR(MBAR, 1) = rtems_configuration_get_microseconds_per_tick() - 1;

        /* Feed system frequency to the timer */
        *MCF5206E_TMR(MBAR, 1) |= MCF5206E_TMR_ICLK_MSCLK;

        /* Enable timer 1 interrupts */
        *MCF5206E_IMR(MBAR) &= ~MCF5206E_INTR_BIT(MCF5206E_INTR_TIMER_1);

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
