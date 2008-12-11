/*
 *  LPC22XX/LPC21xx clock specific using the System Timer
 * Copyright (c) 2006 by Ray <rayx.cn@gmail.com>
 *  Set the Time0 to generate click for RTEMS
 *
 *  This is hardware specific part of the clock driver. At the end of this
 *  file, the generic part of the driver is #included.
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
#include <bsp.h>
#include <irq.h>
#include <lpc22xx.h>
#include <rtems/bspIo.h>  /* for printk */

/* this is defined in ../../../shared/clockdrv_shell.h */
rtems_isr Clock_isr(rtems_vector_number vector);
static void clock_isr_on(const rtems_irq_connect_data *unused);
static void clock_isr_off(const rtems_irq_connect_data *unused);
static int clock_isr_is_on(const rtems_irq_connect_data *irq);

/* Replace the first value with the clock's interrupt name. */
rtems_irq_connect_data clock_isr_data = {LPC22xx_INTERRUPT_TIMER0,
                                         (rtems_irq_hdl)Clock_isr,
                                         clock_isr_on,
                                         clock_isr_off,
                                         clock_isr_is_on,
                                         3,     /* unused for ARM cpus */
                                         0 };   /* unused for ARM cpus */

/* If you follow the code, this is never used, so any value 
 * should work
 */
#define CLOCK_VECTOR 0



/* use the /shared/clockdrv_shell.h code template */

/**
 * When we get the clock interrupt
 *    - clear the interrupt bit?
 *    - restart the timer?
 */
#define Clock_driver_support_at_tick() \
 do {							\
	  if (!(T0IR & 0x01)) return; 	\
	  T0IR = 0x01; 			\
	  VICVectAddr = 0x00;\
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
 
  /* set timer to generate interrupt every rtems_configuration_get_microseconds_per_tick()
   * MR0/(LPC22xx_Fpclk/(PR0+1)) = 10/1000 = 0.01s
   */			
	
#define Clock_driver_support_initialize_hardware() \
  do { \
	T0TCR &= 0; 	 /* disable and clear timer 0, set to  */ \
	T0PC  = 0;            /* TC is incrementet on every pclk.*/ \
	T0MR0 = ((LPC22xx_Fpclk/1000* rtems_configuration_get_microseconds_per_tick()) / 1000); /* initialize the timer period and prescaler */  \
	/*T0PR = (((LPC22xx_Fpclk / 1000) * rtems_configuration_get_microseconds_per_tick()) / 1000-1); \ */ \
	T0MCR |= 0x03;		  /* generate interrupt when T0MR0 match T0TC and Reset Timer Count*/ \
	T0EMR = 0;  /*No external match*/ \
	T0TCR = 1; /*enable timer0*/ \
	T0IR|=0x01;/*enable interrupt, skyeye will check this*/\
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
       T0TCR&=~0x02; \
	BSP_remove_rtems_irq_handler(&clock_isr_data);                  \
     } while (0)

uint32_t bsp_clock_nanoseconds_since_last_tick(void)
{
	uint32_t clicks;
	
	clicks = T0TC;  /*T0TC is the 32bit time counter 0*/
	
	return (uint32_t) (rtems_configuration_get_microseconds_per_tick() - clicks) * 1000;
}
	
#define Clock_driver_nanoseconds_since_last_tick bsp_clock_nanoseconds_since_last_tick



/**
 * Enables clock interrupt.
 *
 * If the interrupt is always on, this can be a NOP.
 */
static void clock_isr_on(const rtems_irq_connect_data *unused)
{
	T0IR&=0x01;
	//return;
}

/**
 * Disables clock interrupts
 *
 * If the interrupt is always on, this can be a NOP.
 */
static void clock_isr_off(const rtems_irq_connect_data *unused)
{
	T0IR=0x00;
	//return;
}

/**
 * Tests to see if clock interrupt is enabled, and returns 1 if so.
 * If interrupt is not enabled, returns 0.
 *
 * If the interrupt is always on, this always returns 1.
 */
static int clock_isr_is_on(const rtems_irq_connect_data *irq)
{
    return T0IR & 0x01;  /*MR0 mask*/
}


/* Make sure to include this, and only at the end of the file */
#include "../../../../libbsp/shared/clockdrv_shell.h"

