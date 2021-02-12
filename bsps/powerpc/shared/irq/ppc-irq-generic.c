/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPC
 *
 * @brief Generic Interrupt suppoer
 */

/*
 * Copyright (C) 2021 Chris Johns. All rights reserved.
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

#include <stdlib.h>

#include <rtems.h>
#include <stdlib.h>
#include <rtems/bspIo.h> /* for printk */
#include <libcpu/spr.h>
#include <bsp/irq_supp.h>
#include <bsp/irq-generic.h>
#include <bsp/vectors.h>

SPR_RW(BOOKE_TSR)
SPR_RW(PPC405_TSR)

/* legacy mode for bookE DEC exception;
 * to avoid the double layer of function calls
 * (dec_handler_bookE -> C_dispatch_irq_handler -> user handler)
 * it is preferrable for the user to hook the DEC
 * exception directly.
 * However, the legacy mode works with less modifications
 * of user code.
 */
static int C_dispatch_dec_handler_bookE (BSP_Exception_frame *frame, unsigned int excNum)
{
  /* clear interrupt; we must do this
   * before C_dispatch_irq_handler()
   * re-enables MSR_EE.
   * Note that PPC405 uses a different SPR# for TSR
   */
  if (ppc_cpu_is_bookE()==PPC_BOOKE_405)
    _write_PPC405_TSR( BOOKE_TSR_DIS );
  else
    _write_BOOKE_TSR( BOOKE_TSR_DIS );
  return C_dispatch_irq_handler(frame, ASM_DEC_VECTOR);
}

/*
 * RTEMS Global Interrupt Handler Management Routines
 */
int BSP_rtems_irq_generic_set(rtems_irq_global_settings* config)
{
  int r;

  r = BSP_setup_the_pic(config);
  if (!r)
    return r;

  ppc_exc_set_handler(ASM_EXT_VECTOR, C_dispatch_irq_handler);
  if ( ppc_cpu_is_bookE() ) {
    /* bookE decrementer interrupt needs to be cleared BEFORE
     * dispatching the user ISR (because the user ISR is called
     * with EE enabled)
     * We do this so that existing DEC handlers can be used
     * with minor modifications.
     */
    ppc_exc_set_handler(ASM_BOOKE_DEC_VECTOR, C_dispatch_dec_handler_bookE);
  } else {
    ppc_exc_set_handler(ASM_DEC_VECTOR, C_dispatch_irq_handler);
  }

  return 1;
}

void bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  BSP_enable_irq_at_pic(vector);
}

void bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  BSP_disable_irq_at_pic(vector);
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
  /*
   * Initialize RTEMS IRQ system
   */
  BSP_rtems_irq_mng_init(0);
  return RTEMS_SUCCESSFUL;
}
