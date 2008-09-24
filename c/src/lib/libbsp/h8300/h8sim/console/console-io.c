/*
 *  This file contains the hardware specific portions of the TTY driver
 *  for the serial ports on the erc32.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
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
/*
  typedef void (*_write_p_t)(int, char *, int);
  _write_p_t _write_p = (_write_p_t)0xc7;
  _write_p( 1, &ch, 1 );

  asm volatile( "mov.b #1,r1l ;  mov.b %0l,r2l ; mov.b #1,r3l; jsr @@0xc7"
       :  : "r" (&ch)  : "r1", "r2");
*/
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
