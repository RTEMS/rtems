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

void mips_default_isr( int vector );

#include <rtems/bspIo.h>  /* for printk */

void mips_vector_isr_handlers( CPU_Interrupt_frame *frame )
{
  unsigned int sr;
  unsigned int cause;
  unsigned int pending;

  mips_get_sr( sr );
  mips_get_cause( cause );

  pending = (cause & sr & 0xff00) >> CAUSE_IPSHIFT;

  /* SW Bits */
  if ( pending & 0x01) {
    printk("Pending IRQ Q 0x%x\n", pending );
  }

  if ( pending & 0x02) {
    printk("Pending IRQ Q 0x%x\n", pending );
  }

  /* South Bridge Interrupt */
  if ( pending & 0x04) {
     BSP_i8259s_int_process();
  }

  /* South Bridge SMI */
  if (pending & 0x08){
    printk( "Pending IRQ 0x%x\n", pending );
  }

  /* TTY 2 */
  if (pending & 0x10) {
    printk( "Pending IRQ 0x%x\n", pending );
  }
  /* Core HI */
  if (pending & 0x20) {
    printk( "Pending IRQ 0x%x\n", pending );
  }
   /* Core LO */
  if (pending & 0x40) {
    printk( "Pending IRQ 0x%x\n", pending );
  }

  if ( pending & 0x80 ) {
    bsp_interrupt_handler_dispatch( MALTA_INT_TICKER );
  }
}

void mips_default_isr( int vector )
{
  unsigned int sr;
  unsigned int cause;

  mips_get_sr( sr );
  mips_get_cause( cause );

  printk( "Unhandled isr exception: vector 0x%02x, cause 0x%08X, sr 0x%08X\n",
      vector, cause, sr );

  while(1);      /* Lock it up */

  rtems_fatal_error_occurred(1);
}

