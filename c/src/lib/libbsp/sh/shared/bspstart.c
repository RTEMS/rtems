/*
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  COPYRIGHT (c) 2001.
 *  Ralf Corsepius (corsepiu@faw.uni-ulm.de).
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  COPYRIGHT (c) 2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>

uint32_t bsp_clicks_per_second;

#ifndef START_HW_INIT
  void bsp_hw_init(void);
#endif

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */
void bsp_start( void )
{
  /*
   * For real boards you need to setup the hardware
   * and need to copy the vector table from rom to ram.
   *
   * Depending on the board this can either be done from inside the rom
   * startup code, rtems startup code or here.
   */

  #ifndef START_HW_INIT
    /* board hardware setup here, or from 'start.S' */
    bsp_hw_init();
  #endif

  /*
   *  initialize the interrupt stack for this BSP
   */
  #if ( CPU_ALLOCATE_INTERRUPT_STACK == FALSE )
    _CPU_Interrupt_stack_low = &CPU_Interrupt_stack_low ;
    _CPU_Interrupt_stack_high = &CPU_Interrupt_stack_high ;
  #endif

  /*
   *  initialize the device driver parameters
   */
  bsp_clicks_per_second = CPU_CLOCK_RATE_HZ;
}
