/**
 *  @file exit.c
 *
 *  Routines to shutdown and reboot the BSP.
 */
/*
 *  RTEMS GBA BSP
 *
 *  Copyright (c) 2004 Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <stdio.h>
#include <bsp.h>
#include <rtems/bspIo.h>
#include <rtems/libio.h>
#include <gba.h>


/**
 *  @brief rtemsReboot BSP routine reboot rtems
 *
 *  Input parameters:  None
 *
 *  Output parameters: None
 */
void rtemsReboot (void)
{
   asm volatile ("ldr  r0, =_gba_reset");
   asm volatile ("bx   r0");
}

/**
 *  @brief bsp_cleanup BSP routine wait input from user for rebooting
 *
 *  Normally at this point, the console driver is disconnected => we must
 *  use polled output/input. This is exactly what printk does.
 *
 *  @param  None
 *  @return None
 */
void bsp_cleanup(void)
{
  static const char line[]="\nEXECUTIVE SHUTDOWN! Press button to reboot...";
  printk("\n");
  printk("%s",line);
  while ( !GBA_ANY_KEY(GBA_KEY_ALL) );
  rtemsReboot();
}
