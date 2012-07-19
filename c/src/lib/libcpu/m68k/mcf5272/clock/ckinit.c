/*
 *  Clock Driver for MCF5272 CPU
 *
 *  This driver initailizes timer1 on the MCF5272 as the
 *  main system clock
 *
 *  Copyright 2004 Cogent Computer Systems
 *  Author: Jay Monkman <jtm@lopingdog.com>
 *
 *  Based on MCF5206 clock driver by
 *    Victor V. Vengerov <vvv@oktet.ru>
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
#include <mcf5272/mcf5272.h>

/*
 * Clock_driver_ticks is a monotonically increasing counter of the
 * number of clock ticks since the driver was initialized.
 */
volatile uint32_t Clock_driver_ticks;


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

 * RETURNS:
 *     none
 */
rtems_isr
Clock_isr (rtems_vector_number vector)
{
    /* Clear pending interrupt... */
    g_timer_regs->ter1 = MCF5272_TER_REF | MCF5272_TER_CAP;

    /* Announce the clock tick */
    Clock_driver_ticks++;
    rtems_clock_tick();
    if (rtems_clock_hook != NULL) {
        rtems_clock_hook(vector);
    }
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
    if (rtems_configuration_get_ticks_per_timeslice()) {
        uint32_t icr;
        /* disable all timer1 interrupts */
        icr = g_intctrl_regs->icr1;
        icr = icr & ~(MCF5272_ICR1_TMR1_MASK | MCF5272_ICR1_TMR1_PI);
        icr |= (MCF5272_ICR1_TMR1_IPL(0) | MCF5272_ICR1_TMR1_PI);
        g_intctrl_regs->icr1 = icr;

        /* reset timer1 */
        g_timer_regs->tmr1 = MCF5272_TMR_CLK_STOP;

        /* clear pending */
        g_timer_regs->ter1 = MCF5272_TER_REF | MCF5272_TER_CAP;
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
  uint32_t icr;
  Clock_driver_ticks = 0;
  if (rtems_configuration_get_ticks_per_timeslice()) {

      /* Register the interrupt handler */
      set_vector(clock_isr, BSP_INTVEC_TMR1, 1);

      /* Reset timer 1 */
      g_timer_regs->tmr1 = MCF5272_TMR_RST;
      g_timer_regs->tmr1 = MCF5272_TMR_CLK_STOP;
      g_timer_regs->tmr1 = MCF5272_TMR_RST;
      g_timer_regs->tcn1 = 0;  /* reset counter */
      g_timer_regs->ter1 = MCF5272_TER_REF | MCF5272_TER_CAP;

      /* Set Timer 1 prescaler so that it counts in microseconds */
      g_timer_regs->tmr1 = (
          ((((BSP_SYSTEM_FREQUENCY / 1000000) - 1) << MCF5272_TMR_PS_SHIFT) |
           MCF5272_TMR_CE_DISABLE                                      |
           MCF5272_TMR_ORI                                             |
           MCF5272_TMR_FRR                                             |
           MCF5272_TMR_CLK_MSTR                                        |
           MCF5272_TMR_RST));

      /* Set the timer timeout value from the BSP config */
      g_timer_regs->trr1 = rtems_configuration_get_microseconds_per_tick() - 1;

      /* Feed system frequency to the timer */
      g_timer_regs->tmr1 |= MCF5272_TMR_CLK_MSTR;

      /* Configure timer1 interrupts */
      icr = g_intctrl_regs->icr1;
      icr = icr & ~(MCF5272_ICR1_TMR1_MASK | MCF5272_ICR1_TMR1_PI);
      icr |= (MCF5272_ICR1_TMR1_IPL(BSP_INTLVL_TMR1) | MCF5272_ICR1_TMR1_PI);
      g_intctrl_regs->icr1 = icr;

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
