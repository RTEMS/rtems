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
#include <bsp/linker-symbols.h>

static inline bool lpc24xx_irq_is_valid(rtems_vector_number vector)
{
  return vector <= BSP_INTERRUPT_VECTOR_MAX;
}

void lpc24xx_irq_set_priority(rtems_vector_number vector, unsigned priority)
{
  if (lpc24xx_irq_is_valid(vector)) {
    if (priority > LPC24XX_IRQ_PRIORITY_VALUE_MAX) {
      priority = LPC24XX_IRQ_PRIORITY_VALUE_MAX;
    }

    #ifdef ARM_MULTILIB_ARCH_V4
      VICVectPriorityBase [vector] = priority;
    #else
      _ARMV7M_NVIC_Set_priority((int) vector, (int) (priority << 3));
    #endif
  }
}

unsigned lpc24xx_irq_get_priority(rtems_vector_number vector)
{
  if (lpc24xx_irq_is_valid(vector)) {
    #ifdef ARM_MULTILIB_ARCH_V4
      return VICVectPriorityBase [vector];
    #else
      return (unsigned) (_ARMV7M_NVIC_Get_priority((int) vector) >> 3);
    #endif
  } else {
    return LPC24XX_IRQ_PRIORITY_VALUE_MIN - 1U;
  }
}

#ifdef ARM_MULTILIB_ARCH_V4

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  VICIntEnable = 1U << vector;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  VICIntEnClear = 1U << vector;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
  volatile uint32_t *addr = VICVectAddrBase;
  volatile uint32_t *prio = VICVectPriorityBase;
  rtems_vector_number i = 0;

  /* Disable all interrupts */
  VICIntEnClear = 0xffffffff;

  /* Clear all software interrupts */
  VICSoftIntClear = 0xffffffff;

  /* Use IRQ category */
  VICIntSelect = 0;

  for (i = BSP_INTERRUPT_VECTOR_MIN; i <= BSP_INTERRUPT_VECTOR_MAX; ++i) {
    /* Use the vector address register to store the vector number */
    addr [i] = i;

    /* Give vector lowest priority */
    prio [i] = 15;
  }

  /* Reset priority mask register */
  VICSWPrioMask = 0xffff;

  /* Acknowledge interrupts for all priorities */
  for (
    i = LPC24XX_IRQ_PRIORITY_VALUE_MIN;
    i <= LPC24XX_IRQ_PRIORITY_VALUE_MAX;
    ++i
  ) {
    VICVectAddr = 0;
  }

  /* Install the IRQ exception handler */
  _CPU_ISR_install_vector(ARM_EXCEPTION_IRQ, arm_exc_interrupt, NULL);

  return RTEMS_SUCCESSFUL;
}

#endif /* ARM_MULTILIB_ARCH_V4 */
