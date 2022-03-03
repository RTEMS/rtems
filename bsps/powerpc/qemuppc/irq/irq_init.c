/*
 * RTEMS generic MPC83xx BSP
 *
 * This file integrates the IPIC irq controller.
 */

/*
 * Copyright (c) 2007 embedded brains GmbH. All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>

#include <libcpu/powerpc-utility.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/vectors.h>

static int qemuppc_exception_handler(
  BSP_Exception_frame *frame,
  unsigned exception_number
)
{
  rtems_panic("Unexpected interrupt occured");
  return 0;
}

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

/*
 * functions to enable/disable a source at the ipic
 */
rtems_status_code bsp_interrupt_vector_enable( rtems_vector_number irqnum)
{
  /* FIXME: do something */
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(irqnum));
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable( rtems_vector_number irqnum)
{
  /* FIXME: do something */
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(irqnum));
  return RTEMS_SUCCESSFUL;
}

void bsp_interrupt_facility_initialize(void)
{
  rtems_status_code sc;

  /* Install exception handler */
  sc = ppc_exc_set_handler( ASM_EXT_VECTOR, qemuppc_exception_handler);
  _Assert_Unused_variable_equals( sc, RTEMS_SUCCESSFUL);
}
