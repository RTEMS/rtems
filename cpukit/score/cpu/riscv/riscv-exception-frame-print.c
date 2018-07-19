/*
 * Copyright (c) 2018 embedded brains GmbH
 * Copyright (c) 2015 Hesham Almatary <hesham@alumni.york.ac.uk>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/cpu.h>
#include <rtems/bspIo.h>
#include <inttypes.h>

#if __riscv_xlen == 32
#define PRINT_REG "0x%08" PRIxPTR
#elif __riscv_xlen == 64
#define PRINT_REG "0x%016" PRIxPTR
#endif

void _CPU_Exception_frame_print( const CPU_Exception_frame *frame )
{
  printk("mstatus 0x%08" PRIxPTR "\n", frame->Interrupt_frame.mstatus);
  printk("mcause  " PRINT_REG "\n", frame->mcause);
  printk("mepc    " PRINT_REG "\n", frame->Interrupt_frame.mepc);
  printk("ra      " PRINT_REG "\n", frame->Interrupt_frame.ra);
  printk("sp      " PRINT_REG "\n", frame->sp);
  printk("gp      " PRINT_REG "\n", frame->gp);
  printk("tp      " PRINT_REG "\n", frame->tp);
  printk("t0      " PRINT_REG "\n", frame->Interrupt_frame.t0);
  printk("t1      " PRINT_REG "\n", frame->Interrupt_frame.t1);
  printk("t2      " PRINT_REG "\n", frame->Interrupt_frame.t2);
  printk("s0      " PRINT_REG "\n", frame->Interrupt_frame.s0);
  printk("s1      " PRINT_REG "\n", frame->Interrupt_frame.s1);
  printk("a0      " PRINT_REG "\n", frame->Interrupt_frame.a0);
  printk("a1      " PRINT_REG "\n", frame->Interrupt_frame.a1);
  printk("a2      " PRINT_REG "\n", frame->Interrupt_frame.a2);
  printk("a3      " PRINT_REG "\n", frame->Interrupt_frame.a3);
  printk("a4      " PRINT_REG "\n", frame->Interrupt_frame.a4);
  printk("a5      " PRINT_REG "\n", frame->Interrupt_frame.a5);
  printk("a6      " PRINT_REG "\n", frame->Interrupt_frame.a6);
  printk("a7      " PRINT_REG "\n", frame->Interrupt_frame.a7);
  printk("s2      " PRINT_REG "\n", frame->s2);
  printk("s3      " PRINT_REG "\n", frame->s3);
  printk("s4      " PRINT_REG "\n", frame->s4);
  printk("s5      " PRINT_REG "\n", frame->s5);
  printk("s6      " PRINT_REG "\n", frame->s6);
  printk("s7      " PRINT_REG "\n", frame->s7);
  printk("s8      " PRINT_REG "\n", frame->s8);
  printk("s9      " PRINT_REG "\n", frame->s9);
  printk("s10     " PRINT_REG "\n", frame->s10);
  printk("s11     " PRINT_REG "\n", frame->s11);
  printk("t3      " PRINT_REG "\n", frame->Interrupt_frame.t3);
  printk("t4      " PRINT_REG "\n", frame->Interrupt_frame.t4);
  printk("t5      " PRINT_REG "\n", frame->Interrupt_frame.t5);
  printk("t6      " PRINT_REG "\n", frame->Interrupt_frame.t6);
#if __riscv_flen > 0
  printk("fcsr    0x%08" PRIx32 "\n", frame->Interrupt_frame.fcsr);
#endif /* __riscv_flen */
}
