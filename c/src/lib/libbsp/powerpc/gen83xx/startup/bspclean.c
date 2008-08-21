/**
 * @file
 *
 * @ingroup mpc83xx
 *
 * @brief Source for BSP startup code.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#include <string.h>

#include <rtems.h>
#include <rtems/bspIo.h>

#include <bsp.h>
#include <bsp/bootcard.h>

extern int bsp_uart_pollRead(int minor);

void bsp_reset(void)
{
  #ifdef MPC8313ERDB
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
  #else /* MPC8313ERDB */

    /* Do nothing */

  #endif /* MPC8313ERDB */
}

void bsp_cleanup(void)
{
  #if defined(BSP_PRESS_KEY_FOR_RESET)
    printk( "\nEXECUTIVE SHUTDOWN! Any key to reboot..." );

    /*
     * Wait for a key to be pressed
     */
    while ( bsp_uart_pollRead(0) == -1 ) {
        /* Wait */
    }
  #endif

  /*
   *  Check both conditions -- if you want to ask for reboot, then
   *  you must have meant to reset the board.
   */
  #if defined(BSP_PRESS_KEY_FOR_RESET) || defined(BSP_RESET_BOARD_AT_EXIT)
    bsp_reset();
  #endif 
}
