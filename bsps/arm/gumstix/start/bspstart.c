/* SPDX-License-Identifier: GPL-2.0+-with-RTEMS-exception */

/*
 * Copyright (c) 2009 Yang Xi <hiyangxi@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq-generic.h>
#include <rtems/libcsupport.h>
#include <rtems/libio.h>
#include <pxa255.h>

/*
 *
 * NAME: bsp_start_default - BSP initialization function
 *
 * DESCRIPTION:
 *   This function is called before RTEMS is initialized and used
 *   adjust the kernel's configuration.
 *
 *   This function also configures the CPU's memory protection unit.
 *
 * RESTRICTIONS/LIMITATIONS:
 *   Since RTEMS is not configured, no RTEMS functions can be called.
 *
 */
static void bsp_start_default( void )
{
  /* disable interrupts */
  XSCALE_INT_ICMR = 0x0;
  bsp_interrupt_initialize();
} /* bsp_start */

/*
 *  By making this a weak alias for bsp_start_default, a brave soul
 *  can override the actual bsp_start routine used.
 */
void bsp_start (void) __attribute__ ((weak, alias("bsp_start_default")));
