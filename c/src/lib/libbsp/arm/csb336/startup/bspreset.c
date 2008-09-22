/*
 *  Cogent CSB336 Shutdown code
 *
 *  Copyright (c) 2004 by Cogent Computer Systems
 *  Written by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/bspIo.h>

void bsp_reset(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);

  printk("\n\rI should reset here.\n\r");
  while(1);
}
