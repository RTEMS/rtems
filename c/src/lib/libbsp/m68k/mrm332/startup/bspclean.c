/*  bsp_cleanup()
 *
 *  This routine cleans up in the sense that it places the board
 *  in a safe state and flushes the I/O buffers before exiting. 
 *
 *  INPUT:  NONE
 *
 *  OUTPUT: NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <bsp.h>

void bsp_cleanup(void)
{
    /* interrupt driven stdio must be flushed */
    _CPU_ISR_Set_level( 7 );
    _UART_flush();
}
