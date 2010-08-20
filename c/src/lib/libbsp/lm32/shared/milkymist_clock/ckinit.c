/*  ckinit.c
 *
 *  Clock device driver for Lattice Mico32 (lm32).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 *
 *  COPYRIGHT (c) Yann Sionneau <yann.sionneau@telecom-sudparis.eu> (GSoC 2010)
 *  Telecom SudParis
 */

#include <bsp.h>
#include "../include/system_conf.h"
#include "clock.h"
#include "bspopts.h"

#if ON_SIMULATOR
#define CLOCK_DRIVER_USE_FAST_IDLE
#endif

static inline int clockread(unsigned int reg)
{
  return *((int*)(reg));
}

static inline void clockwrite(unsigned int reg, int value)
{
  *((int*)reg) = value;
}

/*
 *  The interrupt vector number associated with the clock tick device
 *  driver.
 */

#define TIMER0_IRQ              (1)

#define CLOCK_VECTOR    ( TIMER0_IRQ )
#define CLOCK_IRQMASK   ( 1 << CLOCK_VECTOR )

#define Clock_driver_support_at_tick() \
  do { \
    lm32_interrupt_ack(CLOCK_IRQMASK); \
  } while (0)

#define Clock_driver_support_install_isr(_new, _old ) \
  do { \
	  _old = (rtems_isr_entry) set_vector( _new, CLOCK_VECTOR, 1 ); \
  } while (0)

void Clock_driver_support_initialize_hardware(void)
{
  clockwrite(MM_TIMER0_COMPARE, (CPU_FREQUENCY / (1000000 / rtems_configuration_get_microseconds_per_tick())));
	
  clockwrite(MM_TIMER0_COUNTER, 0);
  clockwrite(MM_TIMER0_CONTROL, TIMER_ENABLE | TIMER_AUTORESTART);
  lm32_interrupt_unmask(CLOCK_IRQMASK);
}

void Clock_driver_support_shutdown_hardware(void)
{
  lm32_interrupt_mask(CLOCK_IRQMASK);
  clockwrite(MM_TIMER0_CONTROL, 0);
}

#include "../../../shared/clockdrv_shell.h"

