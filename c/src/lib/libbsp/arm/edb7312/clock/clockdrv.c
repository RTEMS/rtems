/*
 * Cirrus EP7312 Clock driver
 *
 * Copyright (c) 2002 by Jay Monkman <jtm@smoothsmoothie.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <ep7312.h>
#include <bsp.h>
#include <bsp/irq.h>

#if ON_SKYEYE==1
  #define CLOCK_DRIVER_USE_FAST_IDLE
#endif

void Clock_isr(rtems_irq_hdl_param arg);
static void clock_isr_on(const rtems_irq_connect_data *unused);
static void clock_isr_off(const rtems_irq_connect_data *unused);
static int clock_isr_is_on(const rtems_irq_connect_data *irq);

rtems_irq_connect_data clock_isr_data = {
  .name   = BSP_TC1OI,
  .hdl    = Clock_isr,
  .handle = NULL,
  .on     = clock_isr_on,
  .off    = clock_isr_off,
  .isOn   = clock_isr_is_on,
};

#define Clock_driver_support_at_tick()                \
  do {                                                \
    *EP7312_TC1EOI = 0xffffffff;                      \
  } while(0)

#define Clock_driver_support_install_isr( _new, _old ) \
  do {                                                 \
    (_old) = NULL; /* avoid warning */;                \
    BSP_install_rtems_irq_handler(&clock_isr_data);    \
  } while(0)

/*
 * Set up the clock hardware
 */
#if ON_SKYEYE
  #define TCD_VALUE \
    (rtems_configuration_get_microseconds_per_tick() * 2000)/25000
#else
  #define TCD_VALUE \
    (rtems_configuration_get_microseconds_per_tick() * 2000)/1000000
#endif

#define Clock_driver_support_initialize_hardware()  \
  do {                                              \
    *EP7312_SYSCON1 |= EP7312_SYSCON1_TC1_PRESCALE; \
    *EP7312_TC1D = TCD_VALUE;                       \
    *EP7312_TC1EOI = 0xFFFFFFFF;                    \
  } while (0)

#define Clock_driver_support_shutdown_hardware()                        \
  do {                                                                  \
    BSP_remove_rtems_irq_handler(&clock_isr_data);                  \
  } while (0)

/**
 *  Return the nanoseconds since last tick
 */
uint32_t clock_driver_get_nanoseconds_since_last_tick(void)
{
  return 0;
}

#define Clock_driver_nanoseconds_since_last_tick \
  clock_driver_get_nanoseconds_since_last_tick

static void clock_isr_on(const rtems_irq_connect_data *unused)
{
}

static void clock_isr_off(const rtems_irq_connect_data *unused)
{
}

static int clock_isr_is_on(const rtems_irq_connect_data *irq)
{
  return 1;
}

#include "../../../shared/clockdrv_shell.h"
