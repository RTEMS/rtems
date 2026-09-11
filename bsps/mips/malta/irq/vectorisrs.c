/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
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
#include <stdlib.h>
#include <bsp/irq-generic.h>
#include <bsp/pci.h>
#include <bsp/i8259.h>
#include <bsp.h>
#include <libcpu/isr_entries.h>

void mips_vector_isr_handlers( CPU_Interrupt_frame *frame )
{
  unsigned int sr;
  unsigned int cause;
  unsigned int pending;
  unsigned int bit;

  (void) frame;

  mips_get_sr( sr );
  mips_get_cause( cause );

  pending = ( cause & sr & 0xff00 ) >> CAUSE_IPSHIFT;

  /*
   * The eight sources of the processor occupy the first eight vectors of the
   * interrupt range in the order of the pending field.  The third of them
   * carries every line of the south bridge, which has a decode of its own.
   */
  for ( bit = 0; bit < 8; ++bit ) {
    if ( ( pending & ( 1U << bit ) ) == 0 ) {
      continue;
    }

    if ( bit == 2 ) {
      BSP_i8259s_int_process();
    } else {
      bsp_interrupt_handler_dispatch( MALTA_CPU_INT_START + bit );
    }
  }
}
