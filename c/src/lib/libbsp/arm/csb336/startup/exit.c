/*
 *  Cogent CSB336 Shutdown code
 *
 *  Copyright (c) 2004 by Cogent Computer Systems
 *  Written by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */
#include <stdio.h>
#include <bsp.h>
#include <rtems/bspIo.h>
#include <rtems/libio.h>

int uart_poll_read(int);
void BSP_reset();

void bsp_cleanup(void)
{
  static   char line[]="\nEXECUTIVE SHUTDOWN! Any key to reboot...";
  /*
   * AT this point, the console driver is disconnected => we must
   * use polled output/input. This is exactly what printk
   * does.
   */
  printk("\n");
  printk(line);
  while (uart_poll_read(0) < 0) continue;

  bsp_reset();

}
