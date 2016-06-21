/*
 * Copyright (c) 2008-2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <inttypes.h>

#include <rtems/bspIo.h>

#include <bsp/irq-generic.h>

void bsp_interrupt_handler_default(rtems_vector_number vector)
{
  printk("spurious interrupt: %" PRIu32 "\n", vector);
}
