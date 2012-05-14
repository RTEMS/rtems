/*
 *  MC68360 support routines - reduced from gen68360 BSP
 *
 *  W. Eric Norum
 *  Saskatchewan Accelerator Laboratory
 *  University of Saskatchewan
 *  Saskatoon, Saskatchewan, CANADA
 *  eric@skatter.usask.ca
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include "m68360.h"

/*
 * Send a command to the CPM RISC processer
 */

void M360ExecuteRISC( volatile m360_t *m360, uint16_t command)
{
  uint16_t sr;

  rtems_interrupt_disable(sr);
  while (m360->cr & M360_CR_FLG)
  	continue;
  m360->cr = command | M360_CR_FLG;
  rtems_interrupt_enable(sr);
}
