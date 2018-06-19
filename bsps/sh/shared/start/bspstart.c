/*
 *  This routine does the bulk of the system initialization.
 */

/*
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
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <rtems/score/percpu.h>

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
   *  initialize the device driver parameters
   */
  bsp_clicks_per_second = CPU_CLOCK_RATE_HZ;
}
