/*
 *  This file contains the TTY driver for the serial ports on the LEON.
 *
 *  This driver uses the termios pseudo driver.
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
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>

/*
 *  console_outbyte_polled
 *
 *  This routine transmits a character using polling.
 */

void console_outbyte_polled(
  int  port,
  char ch
)
{
  if ( port == 0 ) {
    while ( (LEON_REG.UART_Status_1 & LEON_REG_UART_STATUS_THE) == 0 );
      LEON_REG.UART_Channel_1 = (int) ch;
      return;
    }

    while ( (LEON_REG.UART_Status_2 & LEON_REG_UART_STATUS_THE) == 0 );
    LEON_REG.UART_Channel_2 = (int) ch;
}

/*
 *  console_inbyte_nonblocking 
 *
 *  This routine polls for a character.
 */

int console_inbyte_nonblocking( int port )
{

  switch (port) {

    case 0:
      if (LEON_REG.UART_Status_1 & LEON_REG_UART_STATUS_ERR) {
        LEON_REG.UART_Status_1 = ~LEON_REG_UART_STATUS_ERR;
      }

      if ((LEON_REG.UART_Status_1 & LEON_REG_UART_STATUS_DR) == 0)
         return -1;
      return (int) LEON_REG.UART_Channel_1;
      return 1;

    case 1:
      if (LEON_REG.UART_Status_2 & LEON_REG_UART_STATUS_ERR) {
        LEON_REG.UART_Status_2 = ~LEON_REG_UART_STATUS_ERR;
      }

      if ((LEON_REG.UART_Status_2 & LEON_REG_UART_STATUS_DR) == 0)
         return -1;
      return (int) LEON_REG.UART_Channel_2;

    default:
      assert( 0 );
  }

  return -1;
}

/*
 *  DEBUG_puts
 *
 *  This should be safe in the event of an error.  It attempts to insure
 *  that no TX empty interrupts occur while it is doing polled IO.  Then
 *  it restores the state of that external interrupt.
 *
 *  Input parameters:
 *    string  - pointer to debug output string
 *
 *  Output parameters:  NONE
 *
 *  Return values:      NONE
 */

void DEBUG_puts(
  char *string
)
{
  char *s;
  unsigned32 old_level;

  LEON_Disable_interrupt( LEON_INTERRUPT_UART_1_RX_TX, old_level );
  LEON_REG.UART_Control_1 = LEON_REG_UART_CTRL_TE;
    for ( s = string ; *s ; s++ ) 
      console_outbyte_polled( 0, *s );

    console_outbyte_polled( 0, '\r' );
    console_outbyte_polled( 0, '\n' );
  LEON_Restore_interrupt( LEON_INTERRUPT_UART_1_RX_TX, old_level );
}
