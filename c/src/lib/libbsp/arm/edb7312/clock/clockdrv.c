/*
 * Cirrus EP7312 Clock driver
 *
 * Copyright (c) 2002 by Jay Monkman <jtm@smoothsmoothie.com>
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
#include <ep7312.h>
#include <bsp.h>
#include <irq.h>


rtems_isr clock_isr(rtems_vector_number vector);
rtems_isr Clock_isr(rtems_vector_number vector);
static void clock_isr_on(const rtems_irq_connect_data *unused);
static void clock_isr_off(const rtems_irq_connect_data *unused);
static int clock_isr_is_on(const rtems_irq_connect_data *irq);

rtems_irq_connect_data clock_isr_data = {BSP_TC1OI,
                                         (rtems_irq_hdl)Clock_isr,
                                         clock_isr_on,
                                         clock_isr_off,
                                         clock_isr_is_on,
                                         3,
                                         0 };

#define CLOCK_VECTOR 0

#define Clock_driver_support_at_tick()                \
  do {                                                \
    *EP7312_TC1EOI = 0xffffffff;                      \
  } while(0)

#define Clock_driver_support_install_isr( _new, _old )                      \
  do {                                                                      \
      BSP_install_rtems_irq_handler(&clock_isr_data);                       \
     } while(0)


/* 
 * Set up the clock hardware
*/
#define Clock_driver_support_initialize_hardware()                            \
  do {                                                                        \
      *EP7312_SYSCON1 |= EP7312_SYSCON1_TC1_PRESCALE;                         \
      *EP7312_TC1D =(BSP_Configuration.microseconds_per_tick * 2000)/1000000; \
      *EP7312_TC1EOI = 0xFFFFFFFF;                                            \
     } while (0)

#define Clock_driver_support_shutdown_hardware()                        \
  do {                                                                  \
	BSP_remove_rtems_irq_handler(&clock_isr_data);                  \
     } while (0)

static void clock_isr_on(const rtems_irq_connect_data *unused)
{
    return;
}

static void clock_isr_off(const rtems_irq_connect_data *unused)
{
    return;
}

static int clock_isr_is_on(const rtems_irq_connect_data *irq)
{
    return 1;
}


#include "../../../shared/clockdrv_shell.c"
