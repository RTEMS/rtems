/*
 *  MC9328MXL clock specific using the System Timer
 *
 *  This is hardware specific part of the clock driver. At the end of this
 *  file, the generic part of the driver is #included.
 *
 *  Copyright (c) 2004 by Cogent Computer Systems
 *  Written by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <mc9328mxl.h>
#include <rtems/bspIo.h>  /* for printk */

/* this is defined in ../../../shared/clockdrv_shell.h */
void Clock_isr(rtems_irq_hdl_param arg);
static void clock_isr_on(const rtems_irq_connect_data *unused);
static void clock_isr_off(const rtems_irq_connect_data *unused);
static int clock_isr_is_on(const rtems_irq_connect_data *irq);

/* Replace the first value with the clock's interrupt name. */
rtems_irq_connect_data clock_isr_data = {
  .name   = BSP_INT_TIMER1,
  .hdl    = Clock_isr,
  .handle = (void *)BSP_INT_TIMER1,
  .on     = clock_isr_on,
  .off    = clock_isr_off,
  .isOn   = clock_isr_is_on,
};

/**
 * When we get the clock interrupt
 *    - clear the interrupt bit?
 *    - restart the timer?
 */
#define Clock_driver_support_at_tick()                \
  do {                                                \
        uint32_t reg;                                 \
        reg = MC9328MXL_TMR1_TSTAT;                   \
        MC9328MXL_TMR1_TSTAT = 0;                     \
  } while(0)

/**
 * Installs the clock ISR. You shouldn't need to change this.
 */
#define Clock_driver_support_install_isr( _new, _old ) \
  do {                                                 \
      (_old) = NULL;                                   \
      BSP_install_rtems_irq_handler(&clock_isr_data);  \
  } while(0)


/**
 * Initialize the hardware for the clock
 *   - Set the frequency
 *   - enable it
 *   - clear any pending interrupts
 *
 * Since you may want the clock always running, you can
 * enable interrupts here. If you do so, the clock_isr_on(),
 * clock_isr_off(), and clock_isr_is_on() functions can be
 * NOPs.
 */
#define Clock_driver_support_initialize_hardware() \
  do { \
        int freq; \
        int cnt; \
        freq = get_perclk1_freq(); \
        printk("perclk1 freq is %d\n", freq); \
        cnt = ((long long)freq * rtems_configuration_get_microseconds_per_tick() + 500000) / 1000000;\
        printk("cnt freq is %d\n", cnt); \
        MC9328MXL_TMR1_TCMP = cnt; \
        /* use PERCLK1 as input, enable timer */ \
        MC9328MXL_TMR1_TCTL = (MC9328MXL_TMR_TCTL_CLKSRC_PCLK1 | \
                               MC9328MXL_TMR_TCTL_TEN | \
                               MC9328MXL_TMR_TCTL_IRQEN); \
        /* set prescaler to 1 (register value + 1) */ \
        MC9328MXL_TMR1_TPRER = 0; \
     } while (0)

/**
 * Do whatever you need to shut the clock down and remove the
 * interrupt handler. Since this normally only gets called on
 * RTEMS shutdown, you may not need to do anything other than
 * remove the ISR.
 */
#define Clock_driver_support_shutdown_hardware()                        \
  do {                                                                  \
    /* Disable timer */ \
    MC9328MXL_TMR1_TCTL = 0; \
    BSP_remove_rtems_irq_handler(&clock_isr_data);                  \
  } while (0)

/**
 * Enables clock interrupt.
 *
 * If the interrupt is always on, this can be a NOP.
 */
static void clock_isr_on(const rtems_irq_connect_data *unused)
{
  MC9328MXL_TMR1_TCTL |= MC9328MXL_TMR_TCTL_IRQEN;
  MC9328MXL_AITC_INTENNUM = MC9328MXL_INT_TIMER1;
}

/**
 * Disables clock interrupts
 *
 * If the interrupt is always on, this can be a NOP.
 */
static void clock_isr_off(const rtems_irq_connect_data *unused)
{
  MC9328MXL_TMR1_TCTL &= ~MC9328MXL_TMR_TCTL_IRQEN;
  MC9328MXL_AITC_INTDISNUM = MC9328MXL_INT_TIMER1;
}

/**
 * Tests to see if clock interrupt is enabled, and returns 1 if so.
 * If interrupt is not enabled, returns 0.
 *
 * If the interrupt is always on, this always returns 1.
 */
static int clock_isr_is_on(const rtems_irq_connect_data *irq)
{
  return MC9328MXL_TMR1_TCTL & MC9328MXL_TMR_TCTL_IRQEN;
}

/* Make sure to include this, and only at the end of the file */
#include "../../../../libbsp/shared/clockdrv_shell.h"
