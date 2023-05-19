/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS generic MPC83xx BSP
 *
 * This file integrates the IPIC irq controller.
 */

/*
 * Copyright (c) 2007 embedded brains GmbH & Co. KG
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
