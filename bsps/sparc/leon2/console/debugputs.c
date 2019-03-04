/**
 * @file
 * @ingroup RTEMSBSPsSPARCLEON2
 * @brief TTY driver for the serial ports on the LEON
 */

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
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <rtems/bspIo.h>

void console_outbyte_polled( int port, unsigned char ch )
{
  if ( port == 0 ) {
    while ( (LEON_REG.UART_Status_1 & LEON_REG_UART_STATUS_THE) == 0 );
    LEON_REG.UART_Channel_1 = (unsigned int) ch;
    return;
  }

  while ( (LEON_REG.UART_Status_2 & LEON_REG_UART_STATUS_THE) == 0 );
  LEON_REG.UART_Channel_2 = (unsigned int) ch;
}

int console_inbyte_nonblocking( int port )
{
  if ( port == 0 ) {
    if (LEON_REG.UART_Status_1 & LEON_REG_UART_STATUS_ERR) {
      LEON_REG.UART_Status_1 = ~LEON_REG_UART_STATUS_ERR;
    }

    if ((LEON_REG.UART_Status_1 & LEON_REG_UART_STATUS_DR) == 0)
       return -1;
    return (int) LEON_REG.UART_Channel_1;
  }

  if (LEON_REG.UART_Status_2 & LEON_REG_UART_STATUS_ERR) {
    LEON_REG.UART_Status_2 = ~LEON_REG_UART_STATUS_ERR;
  }

  if ((LEON_REG.UART_Status_2 & LEON_REG_UART_STATUS_DR) == 0)
     return -1;
  return (int) LEON_REG.UART_Channel_2;
}

static void bsp_out_char( char c )
{
  console_outbyte_polled( 0, c );
}

BSP_output_char_function_type BSP_output_char = bsp_out_char;

static int bsp_in_char( void )
{
  return console_inbyte_nonblocking( 0 );
}

BSP_polling_getchar_function_type BSP_poll_char = bsp_in_char;
