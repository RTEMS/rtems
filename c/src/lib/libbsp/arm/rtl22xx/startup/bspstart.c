/*
 *  LPC22XX/LPC21xx Startup code
 *
 *  Copyright (c) 2007 by Ray Xu <rayx.cn@gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq-generic.h>
#include <lpc22xx.h>

/*
 * Function prototypes
 */
extern void rtems_exception_init_mngt(void);
extern void  UART0_Ini(void);
extern void printi(unsigned long);

/*
 * bsp_start_default - BSP initialization function
 *
 * This function is called before RTEMS is initialized and used
 * adjust the kernel's configuration.
 *
 * This function also configures the CPU's memory protection unit.
 *
 * RESTRICTIONS/LIMITATIONS:
 *   Since RTEMS is not configured, no RTEMS functions can be called.
 *
 */
void bsp_start_default( void )
{
  PINSEL2 =0x0f814914;
  BCFG0 = 0x1000ffef;
  BCFG1 = 0x1000ffef;

  MEMMAP = 0x2;  //debug and excute outside chip

  PLLCON = 1;
  #if (Fpclk / (Fcclk / 4)) == 1
    VPBDIV = 0;
  #endif
  #if (Fpclk / (Fcclk / 4)) == 2
    VPBDIV = 2;
  #endif
  #if (Fpclk / (Fcclk / 4)) == 4
    VPBDIV = 1;
  #endif

  #if (Fcco / Fcclk) == 2
    PLLCFG = ((Fcclk / Fosc) - 1) | (0 << 5);
  #endif
  #if (Fcco / Fcclk) == 4
    PLLCFG = ((Fcclk / Fosc) - 1) | (1 << 5);
  #endif
  #if (Fcco / Fcclk) == 8
    PLLCFG = ((Fcclk / Fosc) - 1) | (2 << 5);
  #endif
  #if (Fcco / Fcclk) == 16
    PLLCFG = ((Fcclk / Fosc) - 1) | (3 << 5);
  #endif
  PLLFEED = 0xaa;
  PLLFEED = 0x55;
  while((PLLSTAT & (1 << 10)) == 0);
  PLLCON = 3;
  PLLFEED = 0xaa;
  PLLFEED = 0x55;

  /* memory configure */
  /* it is not needed in my formatter board */
  //MAMCR = 0;
  // MAMTIM = 3;
  //MAMCR = 2;

  UART0_Ini();

  /*
   * Init rtems exceptions management
   */
  rtems_exception_init_mngt();

  /*
   * Init rtems interrupt management
   */
  if (bsp_interrupt_initialize() != RTEMS_SUCCESSFUL) {
    _CPU_Fatal_halt(0xe);
  }
} /* bsp_start */

/*
 *  By making this a weak alias for bsp_start_default, a brave soul
 *  can override the actual bsp_start routine used.
 */

void bsp_start (void) __attribute__ ((weak, alias("bsp_start_default")));
