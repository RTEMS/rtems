/*
 *  This file contains the TTY driver for the serial ports on the LEON.
 *
 *  This driver uses the termios pseudo driver.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON3 BSP.
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
  unsigned char ch
)
{
  if ((port >= 0) && (port <= CONFIGURE_NUMBER_OF_TERMIOS_PORTS))
  {
    while ( (LEON3_Console_Uart[port]->status & LEON_REG_UART_STATUS_THE) == 0 );
    LEON3_Console_Uart[port]->data = (unsigned int) ch;
  }
}

/*
 *  console_inbyte_nonblocking 
 *
 *  This routine polls for a character.
 */

int console_inbyte_nonblocking( int port )
{

  if ((port >=0) && (port < CONFIGURE_NUMBER_OF_TERMIOS_PORTS)) 
  {

      if (LEON3_Console_Uart[port]->status & LEON_REG_UART_STATUS_ERR) {
        LEON3_Console_Uart[port]->status = ~LEON_REG_UART_STATUS_ERR;
      }

      if ((LEON3_Console_Uart[port]->status & LEON_REG_UART_STATUS_DR) == 0)
         return -1;
      return (int) LEON3_Console_Uart[port]->data;
  }

  else
  {
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
  uint32_t level;

  /* LEON_Disable_interrupt( LEON_INTERRUPT_UART_1_RX_TX, old_level ); */
  level = sparc_disable_interrupts();
  LEON3_Console_Uart[0]->ctrl = LEON_REG_UART_CTRL_TE;
    for ( s = string ; *s ; s++ ) 
      console_outbyte_polled( 0, *s );

    console_outbyte_polled( 0, '\r' );
    console_outbyte_polled( 0, '\n' );
  sparc_enable_interrupts(level);
    /* LEON_Restore_interrupt( LEON_INTERRUPT_UART_1_RX_TX, old_level ); */
}
