/*
 * Copyright (c) 2011 Sebastian Huber.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <string.h>

#include <rtems/score/armv7m.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/lm3s69xx.h>

static __attribute__((aligned(128))) ARMV7M_Exception_handler
  lm3s69xx_vector_table [BSP_INTERRUPT_VECTOR_MAX + 1];

void bsp_interrupt_dispatch(void)
{
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
  memcpy(&lm3s69xx_vector_table [0], (void *) 0, sizeof(lm3s69xx_vector_table));
  _ARMV7M_SCB->vtor = &lm3s69xx_vector_table [0];

  return RTEMS_SUCCESSFUL;
}

void bsp_interrupt_handler_default(rtems_vector_number vector)
{
  printk("spurious interrupt: %u\n", vector);
}
