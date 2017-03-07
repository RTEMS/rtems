/*
 * Cirrus EP7312 Startup code
 */

/*
 * Copyright (c) 2002 by Jay Monkman <jtm@smoothsmoothie.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq-generic.h>
#include <ep7312.h>
#include <uart.h>

/*
 * NAME: bsp_start_default - BSP initialization function
 *
 *   This function is called before RTEMS is initialized
 *   This function also configures the CPU's memory protection unit.
 *
 *
 * RESTRICTIONS/LIMITATIONS:
 *   Since RTEMS is not configured, no RTEMS functions can be called.
 *
 */
static void bsp_start_default( void )
{
  /* disable interrupts */
  *EP7312_INTMR1 = 0;
  *EP7312_INTMR2 = 0;

  /*
   * Init rtems interrupt management
   */
  bsp_interrupt_initialize();
} /* bsp_start */

/*
 *  By making this a weak alias for bsp_start_default, a brave soul
 *  can override the actual bsp_start routine used.
 */
void bsp_start (void) __attribute__ ((weak, alias("bsp_start_default")));
