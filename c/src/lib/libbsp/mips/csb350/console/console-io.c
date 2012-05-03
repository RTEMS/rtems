/*
 *  This file contains the hardware specific portions of the TTY driver
 *  for the serial ports on the csb350.
 *
 *  Logic based on the jmr3904-io.c file in newlib 1.8.2
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <libcpu/au1x00.h>

/*
 *  console_initialize_hardware
 *
 *  This routine initializes the console hardware.
 *
 */

void console_initialize_hardware(void)
{
    uart0->fifoctrl = 0xf1;   /* enable fifo, max sizes */
    au_sync();
}


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
    /* wait for the fifo to make room */
    while ((uart0->linestat & 0x20) == 0) {
        continue;
    }

    uart0->txdata = ch;
    au_sync();
}

/*
 *  console_inbyte_nonblocking
 *
 *  This routine polls for a character.
 */

int console_inbyte_nonblocking(
  int port
)
{
  unsigned char c;

  if (uart0->linestat & 1) {
      c = (char)uart0->rxdata;
      return c;
  } else {
      return -1;
  }
}

#include <rtems/bspIo.h>

void csb250_output_char(char c)
{
    console_outbyte_polled( 0, c );
    if (c == '\n') {
        console_outbyte_polled( 0, '\r' );
    }
}

int csb250_get_char(void)
{
  return console_inbyte_nonblocking(0);
}

BSP_output_char_function_type           BSP_output_char = csb250_output_char;
BSP_polling_getchar_function_type       BSP_poll_char = csb250_get_char;

