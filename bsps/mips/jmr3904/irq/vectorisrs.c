/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *  
 *  TX3904 Interrupt Vectoring
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
#include <bsp/irq.h>
#include <rtems/bspIo.h>  /* for printk */
#include <bsp/irq-generic.h>
#include <libcpu/isr_entries.h>

void mips_vector_isr_handlers( CPU_Interrupt_frame *frame )
{
  unsigned int sr;
  unsigned int cause;

  mips_get_sr( sr );
  mips_get_cause( cause );

  cause &= (sr & SR_IMASK);
  cause >>= CAUSE_IPSHIFT;

  if ( cause & 0x80 )       /* IP[5] ==> INT0 */
    bsp_interrupt_handler_dispatch( TX3904_IRQ_INT0 );

  if ( cause & 0x40 ) {     /* (IP[4] == 1) ==> IP[0-3] are valid */
    unsigned int v = (cause >> 2) & 0x0f;
    bsp_interrupt_handler_dispatch( MIPS_INTERRUPT_BASE + v );
  }

  if ( cause & 0x02 )       /* SW[0] */
    bsp_interrupt_handler_dispatch( TX3904_IRQ_SOFTWARE_1 );

  if ( cause & 0x01 )       /* IP[1] */
    bsp_interrupt_handler_dispatch( TX3904_IRQ_SOFTWARE_2 );
}
