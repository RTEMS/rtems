/*  ckinit.c
 *
 *  Clock device driver for Lattice Mico32 (lm32).
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Jukka Pietarinen <jukka.pietarinen@mrf.fi>, 2008,
 *  Micro-Research Finland Oy
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
  return *((int*)(TIMER0_BASE_ADDRESS + reg));
}

static inline void clockwrite(unsigned int reg, int value)
{
  *((int*)(TIMER0_BASE_ADDRESS + reg)) = value;
}

/*
 *  The interrupt vector number associated with the clock tick device
 *  driver.
 */

#define CLOCK_VECTOR    ( TIMER0_IRQ )
#define CLOCK_IRQMASK   ( 1 << CLOCK_VECTOR )

#define Clock_driver_support_at_tick() \
  do { \
    /* Clear overflow flag */ \
    clockwrite(LM32_CLOCK_SR, 0); \
    lm32_interrupt_ack(CLOCK_IRQMASK); \
  } while (0)

#define Clock_driver_support_install_isr(_new, _old ) \
  do { \
    _old = (rtems_isr_entry) set_vector( _new, CLOCK_VECTOR, 1 ); \
  } while (0)

void Clock_driver_support_initialize_hardware(void)
{
  /* Set clock period */
  clockwrite(LM32_CLOCK_PERIOD,
	     (CPU_FREQUENCY /
	      (1000000 / rtems_configuration_get_microseconds_per_tick())));

  /* Enable clock interrupts and start in continuous mode */
  clockwrite(LM32_CLOCK_CR, LM32_CLOCK_CR_ITO |
	     LM32_CLOCK_CR_CONT |
	     LM32_CLOCK_CR_START);

  lm32_interrupt_unmask(CLOCK_IRQMASK);
}

void Clock_driver_support_shutdown_hardware(void)
{
  /* Disable clock interrupts and stop */

  lm32_interrupt_unmask(CLOCK_IRQMASK);
  clockwrite(LM32_CLOCK_CR, LM32_CLOCK_CR_STOP);
}

#include "../../../shared/clockdrv_shell.h"

