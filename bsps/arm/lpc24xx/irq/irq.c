/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicIntr
 *
 * @brief LPC24XX interrupt support.
 */

/*
 * Copyright (C) 2008, 2012 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <rtems/score/armv4.h>
#include <rtems/score/armv7m.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/lpc24xx.h>
#include <bsp/linker-symbols.h>

static inline bool lpc24xx_irq_is_valid(rtems_vector_number vector)
{
  return vector < BSP_INTERRUPT_VECTOR_COUNT;
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

rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(pending != NULL);
  *pending = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_raise(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_clear(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(enabled != NULL);
  *enabled = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  VICIntEnable = 1U << vector;
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  VICIntEnClear = 1U << vector;
  return RTEMS_SUCCESSFUL;
}

void bsp_interrupt_facility_initialize(void)
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

  for (i = 0; i < BSP_INTERRUPT_VECTOR_COUNT; ++i) {
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
  _CPU_ISR_install_vector(ARM_EXCEPTION_IRQ, _ARMV4_Exception_interrupt, NULL);
}

#endif /* ARM_MULTILIB_ARCH_V4 */
