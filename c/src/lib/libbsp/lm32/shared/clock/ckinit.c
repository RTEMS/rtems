/*  ckinit.c
 *
 *  Clock device driver for Lattice Mico32 (lm32).
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 *
 *  Jukka Pietarinen <jukka.pietarinen@mrf.fi>, 2008,
 *  Micro-Research Finland Oy
 */

#include <stdlib.h>

#include <rtems.h>
#include <bsp.h>
#include "../include/system_conf.h"
#include "clock.h"

static inline int clockread(unsigned int reg)
{
  return *((int*)(TIMER0_BASE_ADDRESS + reg));
}

static inline void clockwrite(unsigned int reg, int value)
{
  *((int*)(TIMER0_BASE_ADDRESS + reg)) = value;
}

void Clock_exit( void );
rtems_isr Clock_isr( rtems_vector_number vector );
extern lm32_isr_entry set_vector(rtems_isr_entry handler,
                                 rtems_vector_number vector, int type);

/*
 *  The interrupt vector number associated with the clock tick device
 *  driver.
 */

#define CLOCK_VECTOR    ( TIMER0_IRQ )
#define CLOCK_IRQMASK   ( 1 << CLOCK_VECTOR )

/*
 *  Clock_driver_ticks is a monotonically increasing counter of the
 *  number of clock ticks since the driver was initialized.
 */

volatile uint32_t         Clock_driver_ticks;

/*
 * These are set by clock driver during its init
 */

rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

/*
 *  The previous ISR on this clock tick interrupt vector.
 */

rtems_isr_entry  Old_ticker;

void Clock_exit( void );

/*
 *  Isr Handler
 */

rtems_isr Clock_isr(
  rtems_vector_number vector
)
{
/*
 * bump the number of clock driver ticks since initialization
 *
 * determine if it is time to announce the passing of tick as configured
 * to RTEMS through the rtems_clock_tick directive
 *
 * perform any timer dependent tasks
 */

  /* Clear overflow flag */
  clockwrite(LM32_CLOCK_SR, 0);
  lm32_interrupt_ack(CLOCK_IRQMASK);

  /* Increment clock ticks */
  Clock_driver_ticks += 1;

  rtems_clock_tick();
}

/*
 *  Install_clock
 *
 *  Install a clock tick handler and reprograms the chip.  This
 *  is used to initially establish the clock tick.
 */

void Install_clock(
  rtems_isr_entry clock_isr
)
{
  /*
   *  Initialize the clock tick device driver variables
   */
  
  Clock_driver_ticks = 0;
  
  Old_ticker = (rtems_isr_entry) set_vector( clock_isr, CLOCK_VECTOR, 1 );
  /*
   *  Hardware specific initialize goes here
   */
  
  printk("rtems_configuration_get_microseconds_per_tick() %d\n", 
	 rtems_configuration_get_microseconds_per_tick());
  printk("PERIOD %d\n", 
	 (CPU_FREQUENCY / (1000000 / rtems_configuration_get_microseconds_per_tick())));
  
  /* Set clock period */
  clockwrite(LM32_CLOCK_PERIOD,
	     (CPU_FREQUENCY /
	      (1000000 / rtems_configuration_get_microseconds_per_tick())));
  
  /* Enable clock interrupts and start in continuous mode */
  clockwrite(LM32_CLOCK_CR, LM32_CLOCK_CR_ITO | 
	     LM32_CLOCK_CR_CONT | 
	     LM32_CLOCK_CR_START);
  
  lm32_interrupt_unmask(CLOCK_IRQMASK);
  
  /*
   *  Schedule the clock cleanup routine to execute if the application exits.
   */
  
  atexit( Clock_exit );      
}

/*
 *  Clean up before the application exits
 */

void Clock_exit( void )
{
  /* Disable clock interrupts and stop */

  lm32_interrupt_unmask(CLOCK_IRQMASK);
  clockwrite(LM32_CLOCK_CR, LM32_CLOCK_CR_STOP);
}

/*
 *  Clock_initialize
 *
 *  Device driver entry point for clock tick driver initialization.
 */

rtems_device_driver Clock_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  printk("Clock initialize %d %d\n", major, minor);

  Install_clock( Clock_isr );

  /*
   * make major/minor avail to others such as shared memory driver
   */

  rtems_clock_major = major;
  rtems_clock_minor = minor;

  return RTEMS_SUCCESSFUL;
}
