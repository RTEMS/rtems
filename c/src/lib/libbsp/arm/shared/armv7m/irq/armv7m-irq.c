/*
 * Copyright (c) 2011-2012 Sebastian Huber.  All rights reserved.
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

#include <rtems/score/armv7m.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/linker-symbols.h>
#include <bsp/armv7m-irq.h>

#ifdef ARM_MULTILIB_ARCH_V7M

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  _ARMV7M_NVIC_Set_enable((int) vector);

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  _ARMV7M_NVIC_Clear_enable((int) vector);

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
  int i;
  ARMV7M_Exception_handler *vector_table =
    (ARMV7M_Exception_handler *) bsp_vector_table_begin;

  memcpy(
    vector_table,
    bsp_start_vector_table_begin,
    (size_t) bsp_vector_table_size
  );

  for (i = BSP_INTERRUPT_VECTOR_MIN; i <= BSP_INTERRUPT_VECTOR_MAX; ++i) {
    vector_table [ARMV7M_VECTOR_IRQ(i)] = _ARMV7M_NVIC_Interrupt_dispatch;
    _ARMV7M_NVIC_Clear_enable(i);
    _ARMV7M_NVIC_Clear_pending(i);
    _ARMV7M_NVIC_Set_priority(i, BSP_ARMV7M_IRQ_PRIORITY_DEFAULT);
  }

  _ARMV7M_SCB->vtor = vector_table;

  return RTEMS_SUCCESSFUL;
}

#endif /* ARM_MULTILIB_ARCH_V7M */
