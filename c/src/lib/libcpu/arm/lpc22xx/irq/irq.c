/*
 * Philps LPC22XX Interrupt handler
 *
 * Copyright (c) 2010 embedded brains GmbH.
 *
 * Copyright (c)  2006 by Ray<rayx.cn@gmail.com>  to support LPC ARM
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

#include <lpc22xx.h>

void bsp_interrupt_dispatch(void)
{
  rtems_vector_number vector = 31 - __builtin_clz(VICIRQStatus);

  bsp_interrupt_handler_dispatch(vector);

  VICVectAddr = 0;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  VICIntEnable |= 1 << vector;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  VICIntEnClr = 1 << vector;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
  volatile uint32_t *ctrl = (volatile uint32_t *) VICVectCntlBase;
  size_t i = 0;

  /* Disable all interrupts */
  VICIntEnClr = 0xffffffff;

  /* Use IRQ category */
  VICIntSelect = 0;

  /* Enable access in USER mode */
  VICProtection = 0;

  for (i = 0; i < 16; ++i) {
    /* Disable vector mode */
    ctrl [i] = 0;

    /* Acknowledge interrupts for all priorities */
    VICVectAddr = 0;
  }

  /* Acknowledge interrupts for all priorities */
  VICVectAddr = 0;

  /* Install the IRQ exception handler */
  _CPU_ISR_install_vector(ARM_EXCEPTION_IRQ, arm_exc_interrupt, NULL);

  return RTEMS_SUCCESSFUL;
}
