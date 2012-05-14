/*
 *  This file contains the hardware specific portions of the TTY driver
 *  for the serial ports on the erc32.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>

/*
 *  console_initialize_hardware
 *
 *  This routine initializes the console hardware.
 *
 */

void console_initialize_hardware(void)
{
  return;
}

/*
 *  console_outbyte_polled
 *
 *  This routine transmits a character using polling.
 */
ssize_t _sys_write(int fd, const void *buf, size_t count);
void console_outbyte_polled(
  int  port,
  char ch
)
{
  _sys_write( 1, &ch, 1 );
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
  return -1;
}

#include <rtems/bspIo.h>

void H8simBSP_output_char(char c) { console_outbyte_polled( 0, c ); }

BSP_output_char_function_type           BSP_output_char = H8simBSP_output_char;
BSP_polling_getchar_function_type       BSP_poll_char = NULL;
