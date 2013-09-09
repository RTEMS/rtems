/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>

void zynq_fatal(zynq_fatal_code code)
{
  rtems_fatal(RTEMS_FATAL_SOURCE_BSP_SPECIFIC, code);
}

void bsp_start(void)
{
  bsp_interrupt_initialize();
}
