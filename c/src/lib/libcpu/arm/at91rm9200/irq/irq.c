/*
 * Atmel AT91RM9200 Interrupt handler
 *
 * Copyright (c) 2010 embedded brains GmbH.
 *
 * Copyright (c) 2004 by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

#include <at91rm9200.h>

void bsp_interrupt_dispatch(void)
{
  rtems_vector_number vector = AIC_CTL_REG(AIC_IVR);

  bsp_interrupt_handler_dispatch(vector);

  AIC_CTL_REG(AIC_EOICR) = 0;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  AIC_CTL_REG(AIC_IECR) = 1 << vector;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  AIC_CTL_REG(AIC_IDCR) = 1 << vector;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
  unsigned long i = 0;

  for (i = 0; i < 32; ++i) {
    AIC_SVR_REG(i<<2) = i;
  }

  /* disable all interrupts */
  AIC_CTL_REG(AIC_IDCR) = 0xffffffff;

  _CPU_ISR_install_vector(ARM_EXCEPTION_IRQ, arm_exc_interrupt, NULL);

  return RTEMS_SUCCESSFUL;
}
