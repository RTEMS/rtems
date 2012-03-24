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

#include <bsp/irq-generic.h>
#include <bsp/armv7m-irq.h>

#ifdef ARM_MULTILIB_ARCH_V7M

void _ARMV7M_NVIC_Interrupt_dispatch(void)
{
  rtems_vector_number vector =
    ARMV7M_SCB_ICSR_VECTACTIVE_GET(_ARMV7M_SCB->icsr);

  _ARMV7M_Interrupt_service_enter();
  bsp_interrupt_handler_dispatch(ARMV7M_IRQ_OF_VECTOR(vector));
  _ARMV7M_Interrupt_service_leave();
}

#endif /* ARM_MULTILIB_ARCH_V7M */
