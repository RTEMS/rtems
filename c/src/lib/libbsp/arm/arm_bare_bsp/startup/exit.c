/*-------------------------------------------------------------------------+
| exit.c - ARM BSP 
+--------------------------------------------------------------------------+
| Routines to shutdown and reboot the BSP.
+--------------------------------------------------------------------------+
|
| Copyright (c) 2000 Canon Research Centre France SA.
| Emmanuel Raguet, mailto:raguet@crf.canon.fr
|
|   The license and distribution terms for this file may be
|   found in found in the file LICENSE in this distribution or at
|   http://www.OARcorp.com/rtems/license.html.
|
+--------------------------------------------------------------------------*/


#include <stdio.h>
#include <bsp.h>
#include <rtems/bspIo.h>
#include <rtems/libio.h>

void rtemsReboot (void)
{
  asm volatile ("b _start");
}

void bsp_cleanup(void)
{
  unsigned char ch;
  static   char line[]="\nEXECUTIVE SHUTDOWN! Any key to reboot...";
  /*
   * AT this point, the console driver is disconnected => we must
   * use polled output/input. This is exactly what printk
   * does.
   */
  printk("\n");
  printk(line);
  ch = BSP_poll_char();
  rtemsReboot();
}








