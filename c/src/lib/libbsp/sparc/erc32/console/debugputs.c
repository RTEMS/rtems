/*
 *  This file contains the TTY driver for the serial ports on the erc32.
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
  unsigned char ch
)
{
  if ( port == 0 ) {
    while ( (ERC32_MEC.UART_Status & ERC32_MEC_UART_STATUS_THEA) == 0 );
    ERC32_MEC.UART_Channel_A = (unsigned int) ch;
    return;
  }

  while ( (ERC32_MEC.UART_Status & ERC32_MEC_UART_STATUS_THEB) == 0 );
  ERC32_MEC.UART_Channel_B = (unsigned int) ch;
}

/*
 *  console_inbyte_nonblocking 
 *
 *  This routine polls for a character.
 */

int console_inbyte_nonblocking( int port )
{
  int UStat;

  UStat = ERC32_MEC.UART_Status;

  switch (port) {

    case 0:
      if (UStat & ERC32_MEC_UART_STATUS_ERRA) {
        ERC32_MEC.UART_Status = ERC32_MEC_UART_STATUS_CLRA;
        ERC32_MEC.Control = ERC32_MEC.Control;
      }

      if ((UStat & ERC32_MEC_UART_STATUS_DRA) == 0)
         return -1;
      return (int) ERC32_MEC.UART_Channel_A;
      return 1;

    case 1:
      if (UStat & ERC32_MEC_UART_STATUS_ERRB) {
        ERC32_MEC.UART_Status = ERC32_MEC_UART_STATUS_CLRB;
        ERC32_MEC.Control = ERC32_MEC.Control;
      }

      if ((UStat & ERC32_MEC_UART_STATUS_DRB) == 0)
        return -1;
      return (int) ERC32_MEC.UART_Channel_B;

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

  ERC32_Disable_interrupt( ERC32_INTERRUPT_UART_A_RX_TX, old_level );
    for ( s = string ; *s ; s++ ) 
      console_outbyte_polled( 0, *s );

    console_outbyte_polled( 0, '\r' );
    console_outbyte_polled( 0, '\n' );
  ERC32_Restore_interrupt( ERC32_INTERRUPT_UART_A_RX_TX, old_level );
}
