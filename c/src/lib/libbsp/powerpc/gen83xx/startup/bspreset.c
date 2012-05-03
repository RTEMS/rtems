/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

void bsp_reset(void)
{
  _ISR_Set_level( 0 );

  /* Set Reset Protection Register (RPR) to "RSTE" */
  mpc83xx.res.rpr = 0x52535445;

  /*
   * Wait for Control Register Enabled in the
   * Reset Control Enable Register (RCER).
   */
  while (mpc83xx.res.rcer != 0x00000001) {
    /* Wait */
  }

  /* Set Software Hard Reset in the Reset Control Register (RCR) */
  mpc83xx.res.rcr = 0x00000002;
}
