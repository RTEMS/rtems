/*  ckinit.c
 *
 *  Clock device driver for Lattice Mico32 (lm32).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) Yann Sionneau <yann.sionneau@telecom-sudparis.eu> (GSoC 2010)
 *  Telecom SudParis
 */

#include <bsp.h>
#include <bsp/irq-generic.h>
#include "../include/system_conf.h"
#include "clock.h"
#include "bspopts.h"

#if ON_SIMULATOR
#define CLOCK_DRIVER_USE_FAST_IDLE
#endif

#define Clock_driver_support_at_tick() \
  do { \
    lm32_interrupt_ack(1 << MM_IRQ_TIMER0); \
  } while (0)

#define Clock_driver_support_install_isr(_new, _old ) \
  do { \
    rtems_interrupt_catch(_new, MM_IRQ_TIMER0, &_old); \
  } while (0)

void Clock_driver_support_initialize_hardware(void)
{
  MM_WRITE(MM_TIMER0_COMPARE, 
   (MM_READ(MM_FREQUENCY)/(1000000/rtems_configuration_get_microseconds_per_tick())));
  MM_WRITE(MM_TIMER0_COUNTER, 0);
  MM_WRITE(MM_TIMER0_CONTROL, TIMER_ENABLE | TIMER_AUTORESTART);
  bsp_interrupt_vector_enable(MM_IRQ_TIMER0);
}

void Clock_driver_support_shutdown_hardware(void)
{
  bsp_interrupt_vector_disable(MM_IRQ_TIMER0);
  MM_WRITE(MM_TIMER0_CONTROL, 0);
}

#include "../../../shared/clockdrv_shell.h"
