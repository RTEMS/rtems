/*
 * Copyright (c) 2010 embedded brains GmbH.
 *
 * PXA255 Interrupt handler by Yang Xi <hiyangxi@gmail.com>
 * Copyright (c) 2004 by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

#include <pxa255.h>

void bsp_interrupt_dispatch(void)
{
  rtems_vector_number vector = 31 - __builtin_clz(XSCALE_INT_ICIP);

  bsp_interrupt_handler_dispatch(vector);
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  XSCALE_INT_ICMR |= 1 << vector;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  XSCALE_INT_ICMR  &= ~(1 << vector);

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
  /* disable all interrupts */
  XSCALE_INT_ICMR = 0x0;

  /* Direct the interrupt to IRQ*/
  XSCALE_INT_ICLR = 0x0;

  /* Install the IRQ exception handler */
  _CPU_ISR_install_vector(ARM_EXCEPTION_IRQ, arm_exc_interrupt, NULL);

  return RTEMS_SUCCESSFUL;
}
