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
 * Number of uarts on AMBA bus
 */
extern int uarts;

/*
 *  console_outbyte_polled
 *
 *  This routine transmits a character using polling.
 */
void console_outbyte_polled(
  int           port,
  unsigned char ch
)
{
  if ((port >= 0) && (port < uarts)) {
    int u = LEON3_Cpu_Index+port;
    while ( (LEON3_Console_Uart[u]->status & LEON_REG_UART_STATUS_THE) == 0 );
    LEON3_Console_Uart[u]->data = (unsigned int) ch;
  }
}

/*
 *  console_inbyte_nonblocking
 *
 *  This routine polls for a character.
 */
int console_inbyte_nonblocking( int port )
{
  if ((port >= 0) && (port < uarts)) {
    int u = LEON3_Cpu_Index+port;
    if (LEON3_Console_Uart[u]->status & LEON_REG_UART_STATUS_ERR)
      LEON3_Console_Uart[u]->status = ~LEON_REG_UART_STATUS_ERR;

    if ((LEON3_Console_Uart[u]->status & LEON_REG_UART_STATUS_DR) == 0)
      return -1;
    return (int) LEON3_Console_Uart[u]->data;
  } else {
    assert( 0 );
  }
  return -1;
}

/* putchar/getchar for printk */
static void bsp_out_char(char c)
{
  console_outbyte_polled(0, c);
}

/*
 *  To support printk
 */

#include <rtems/bspIo.h>

BSP_output_char_function_type BSP_output_char = bsp_out_char;

static int bsp_in_char(void)
{
  int tmp;

  while ((tmp = console_inbyte_nonblocking(0)) < 0);
  return tmp;
}

BSP_polling_getchar_function_type BSP_poll_char = bsp_in_char;
