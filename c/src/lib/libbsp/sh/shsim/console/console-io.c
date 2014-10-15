/*
 *  This file contains the hardware specific portions of the TTY driver
 *  for the simulators stdin/out.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/console-polled.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>

#include <bsp/syscall.h>

/*
 *  console_initialize_hardware
 *
 *  This routine initializes the console hardware.
 */
void console_initialize_hardware(void)
{
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
  __trap34 (SYS_write, 1, &ch, 1);
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

  return __trap34 (SYS_read, 0, &c, 1);
}
