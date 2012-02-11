/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief LPC24XX interrupt support.
 */

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
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems/score/armv7m.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/lpc24xx.h>

void bsp_interrupt_dispatch(void)
{
  #ifdef ARM_MULTILIB_ARCH_V4
    /* Read current vector number */
    rtems_vector_number vector = VICVectAddr;

    /* Enable interrupts in program status register */
    uint32_t psr = arm_status_irq_enable();

    /* Dispatch interrupt handlers */
    bsp_interrupt_handler_dispatch(vector);

    /* Restore program status register */
    arm_status_restore(psr);

    /* Acknowledge interrupt */
    VICVectAddr = 0;
  #else
    rtems_vector_number vector =
      ARMV7M_SCB_ICSR_VECTACTIVE_GET(_ARMV7M_SCB->icsr);

    _ARMV7M_Interrupt_service_enter();
    bsp_interrupt_handler_dispatch(ARMV7M_IRQ_OF_VECTOR(vector));
    _ARMV7M_Interrupt_service_leave();
  #endif
}
