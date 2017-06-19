/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief GDB ARM Simulator interrupt support.
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
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/score/armv4.h>
#include <rtems/score/armv7m.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/linker-symbols.h>

/*
 *  Prototypes
 */
void lpc24xx_irq_set_priority(rtems_vector_number, unsigned);
unsigned lpc24xx_irq_get_priority(rtems_vector_number);

static inline bool lpc24xx_irq_is_valid(rtems_vector_number vector)
{
  return vector <= BSP_INTERRUPT_VECTOR_MAX;
}

void lpc24xx_irq_set_priority(rtems_vector_number vector, unsigned priority)
{
}

unsigned lpc24xx_irq_get_priority(rtems_vector_number vector)
{
  return 0; /* bogus value to avoid warning */
}

#ifdef ARM_MULTILIB_ARCH_V4

void bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
}

void bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
  /* Install the IRQ exception handler */
  _CPU_ISR_install_vector(ARM_EXCEPTION_IRQ, _ARMV4_Exception_interrupt, NULL);

  return RTEMS_SUCCESSFUL;
}

#endif /* ARM_MULTILIB_ARCH_V4 */
