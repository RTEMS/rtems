/*
 * RTEMS for Nintendo DS clock driver.
 *
 * Copyright (c) 2008 by Matthieu Bucchianeri <mbucchia@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE
 *
 * $Id$
 */

#include <rtems.h>
#include <bsp.h>
#include "../irq/irq.h"
#include <nds.h>

#define CLOCK_VECTOR  IRQ_TIMER0

/*
 * forward declaration for clock isr in clockdrv_shell.c
 */

rtems_isr Clock_isr (rtems_vector_number vector);

/*
 * isr registration variables.
 */

static rtems_irq_connect_data clock_isr_data = {
  IRQ_TIMER0,
  (rtems_irq_hdl) Clock_isr,
  NULL,
  NULL,
  NULL,
  0,
  0
};

void update_touchscreen (void);

/*
 * function called on every ticks.
 * NOTE: replaced by macro to avoid empty function call.
 */

#define Clock_driver_support_at_tick()					\
  update_touchscreen();

/*
 * install isr for clock driver.
 */

void
Clock_driver_support_install_isr (rtems_isr_entry new, rtems_isr_entry old)
{
  BSP_install_rtems_irq_handler (&clock_isr_data);
}

/*
 * disable clock.
 */

void
Clock_driver_support_shutdown_hardware (void)
{
  BSP_remove_rtems_irq_handler (&clock_isr_data);
  TIMER_CR (0) &= ~(TIMER_ENABLE);
}

/*
 * initialize clock on timer 0.
 */

void
Clock_driver_support_initialize_hardware (void)
{
  uint32_t freq =
    1000 / (rtems_configuration_get_microseconds_per_tick () / 1000);

  printk ("[+] clock started\n");
  TIMER_CR (0) = TIMER_ENABLE | TIMER_IRQ_REQ | TIMER_DIV_64;
  printk ("[#] setting clock to %u hz\n", freq);
  TIMER_DATA (0) = TIMER_FREQ_64 ((uint16_t) freq);
}

#include "../../../shared/clockdrv_shell.c"
