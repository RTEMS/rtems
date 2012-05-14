/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#include <rtems.h>
#include <libcpu/au1x00.h>

void bsp_reset(void)
{
  void (*reset_func)(void);

  reset_func = (void *)0xbfc00000;

  mips_set_sr( 0x00200000 ); /* all interrupts off, boot exception vectors */

  /* Try to restart bootloader */
  reset_func();
}
