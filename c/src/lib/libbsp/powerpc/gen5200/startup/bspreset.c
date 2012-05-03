/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <bsp/mpc5200.h>
#include <bsp/bootcard.h>

void bsp_reset( void )
{
  #if (BENCHMARK_IRQ_PROCESSING == 1)
  {
    BSP_IRQ_Benchmarking_Report();
  }
  #endif

  /*
   * Now reset the CPU
   */
  mpc5200.gpt[0].count_in = 0xf;
  mpc5200.gpt[0].emsel = 0x9004;

  while(1) ;
}
