/* irq.c
 *
 *  This file contains the implementation of the function described in irq.h
 *
 *  Copyright (c) 2010 embedded brains GmbH.
 *
 *  CopyRight (C) 2000 Canon Research France SA.
 *  Emmanuel Raguet,  mailto:raguet@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/score/armv4.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

#include <s3c24xx.h>

void bsp_interrupt_dispatch(void)
{
  rtems_vector_number vector = *((uint32_t *) rINTOFFSET_ADDR);

  bsp_interrupt_handler_dispatch(vector);
}

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
  _CPU_ISR_install_vector(ARM_EXCEPTION_IRQ, _ARMV4_Exception_interrupt, NULL);

  return RTEMS_SUCCESSFUL;
}
