/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicIntr
 *
 * @brief This source file contains the interrupt dispatch of the
 *   generic_or1k BSP.
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

#include <bsp/irq-generic.h>

#include <rtems/score/cpu.h>
#include <rtems/score/or1k-utility.h>

/*
 * The exception handlers of this architecture are called with the vector
 * number of the exception and the frame of the interrupted context.
 */
void bsp_interrupt_dispatch( uint32_t vector, CPU_Exception_frame *frame );

void bsp_interrupt_dispatch( uint32_t vector, CPU_Exception_frame *frame )
{
  uint32_t pending;

  (void) vector;
  (void) frame;

  /*
   * A source which is masked in the interrupt controller does not raise the
   * exception, but its status bit is set nevertheless, so the mask has to be
   * applied here as well.
   */
  pending = _OR1K_mfspr( CPU_OR1K_SPR_PICSR ) &
    _OR1K_mfspr( CPU_OR1K_SPR_PICMR );

  while ( pending != 0 ) {
    uint32_t bit = pending & ( ~pending + 1 );
    uint32_t index = (uint32_t) __builtin_ctz( bit );

    /*
     * The inputs of the interrupt controller are level triggered, so the
     * status bit is set again as long as the device raises the interrupt.
     * Acknowledge after the handler served the device.
     */
    bsp_interrupt_handler_dispatch( index );
    _OR1K_mtspr( CPU_OR1K_SPR_PICSR, bit );
    pending &= ~bit;
  }
}
