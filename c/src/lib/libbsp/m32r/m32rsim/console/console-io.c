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
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>
#include <reent.h>

/* From Newlib 1.16.0 */
#define SYS_exit        1
#define SYS_open        2
#define SYS_close       3
#define SYS_read        4
#define SYS_write       5
#define SYS_lseek       6
#define SYS_unlink      7
#define SYS_getpid      8
#define SYS_kill        9
#define SYS_fstat       10

int __trap0 (int function, int p1, int p2, int p3, struct _reent *r);

#define TRAP0(f, p1, p2, p3) \
__trap0 (f, (int) (p1), (int) (p2), (int) (p3), _REENT)

void sys_exit(void)
{
  TRAP0(SYS_exit, 0, 0, 0);
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
  TRAP0(SYS_write, 1, &ch, 1);
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

void console_output_char(char c) { console_outbyte_polled( 0, c ); }

BSP_output_char_function_type           BSP_output_char = console_output_char;
BSP_polling_getchar_function_type       BSP_poll_char = NULL;
