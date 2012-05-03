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

static void
__outb(int port, unsigned char v)
{
  *((volatile unsigned char *)(0x80000000 + port)) = v;
}

static unsigned char
__inb(int port)
{
  return *((volatile unsigned char *)(0x80000000 + port));
}

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
void console_outbyte_polled(
  int  port,
  char ch
)
{
  __outb(0x3f8 + 0x00, ch);
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

  if ( __inb(0x3f8 + 0x05) & 0x01 )
    return __inb(0x3f8 + 0x00);
  return -1;
}

#include <rtems/bspIo.h>

void simBSP_output_char(char c) { console_outbyte_polled( 0, c ); }

BSP_output_char_function_type           BSP_output_char = simBSP_output_char;
BSP_polling_getchar_function_type       BSP_poll_char = NULL;
