/*
 *  This file contains the ep1a printk support routines
 *
 *  COPYRIGHT (c) 2011.
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
#include <termios.h>
#include <rtems/bspIo.h>

rtems_device_minor_number         BSPPrintkPort = 0;

/* const char arg to be compatible with BSP_output_char decl. */
void
debug_putc_onlcr(const char c)
{
  volatile int i;

  /*
   * Note:  Hack to get printk to work.  Depends upon bit
   *        and silverchip to initialize the port and just
   *        forces a character to be polled out of com1
   *        regardless of where the console is.
   */

  volatile unsigned char *ptr = (void *)0xff800000;

  if ('\n'==c){
     *ptr = '\r';
     __asm__ volatile("sync");
     for (i=0;i<0x0fff;i++);
  }

  *ptr = c;
  __asm__ volatile("sync");
  for (i=0;i<0x0fff;i++);
}

BSP_output_char_function_type     BSP_output_char = debug_putc_onlcr;
BSP_polling_getchar_function_type BSP_poll_char = NULL;

