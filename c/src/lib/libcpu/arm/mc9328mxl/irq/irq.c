/*
 * Motorola MC9328MXL Interrupt handler
 *
 * Copyright (c) 2010 embedded brains GmbH.
 *
 * Copyright (c) 2004 by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/score/armv4.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

#include <mc9328mxl.h>

void bsp_interrupt_dispatch(void)
{

  rtems_vector_number vector = MC9328MXL_AITC_NIVECSR >> 16;

  bsp_interrupt_handler_dispatch(vector);
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  if (vector >= MC9328MXL_NUM_INTS)
     return RTEMS_INVALID_ID;

  MC9328MXL_AITC_INTENNUM = vector;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  if (vector >= MC9328MXL_NUM_INTS)
     return RTEMS_INVALID_ID;

  MC9328MXL_AITC_INTDISNUM = vector;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{

  _CPU_ISR_install_vector(ARM_EXCEPTION_IRQ, _ARMV4_Exception_interrupt, NULL);

  return RTEMS_SUCCESSFUL;
}
