/**
 * @file
 *
 * @brief CPU Port Implementation API
 */

/*
 * Copyright (c) 2013, 2018 embedded brains GmbH
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

#ifndef _RTEMS_SCORE_CPUIMPL_H
#define _RTEMS_SCORE_CPUIMPL_H

#include <rtems/score/cpu.h>

#define CPU_PER_CPU_CONTROL_SIZE 0

#ifdef RTEMS_SMP
#define RISCV_CONTEXT_IS_EXECUTING 0
#endif

#define RISCV_CONTEXT_ISR_DISPATCH_DISABLE 4

#if __riscv_xlen == 32

#define RISCV_CONTEXT_RA 8
#define RISCV_CONTEXT_SP 12
#define RISCV_CONTEXT_TP 16
#define RISCV_CONTEXT_S0 20
#define RISCV_CONTEXT_S1 24
#define RISCV_CONTEXT_S2 28
#define RISCV_CONTEXT_S3 32
#define RISCV_CONTEXT_S4 36
#define RISCV_CONTEXT_S5 40
#define RISCV_CONTEXT_S6 44
#define RISCV_CONTEXT_S7 48
#define RISCV_CONTEXT_S8 52
#define RISCV_CONTEXT_S9 56
#define RISCV_CONTEXT_S10 60
#define RISCV_CONTEXT_S11 64

#define RISCV_INTERRUPT_FRAME_MSTATUS 0
#define RISCV_INTERRUPT_FRAME_MEPC 4
#define RISCV_INTERRUPT_FRAME_A2 8
#define RISCV_INTERRUPT_FRAME_S0 12
#define RISCV_INTERRUPT_FRAME_S1 16
#define RISCV_INTERRUPT_FRAME_RA 20
#define RISCV_INTERRUPT_FRAME_A3 24
#define RISCV_INTERRUPT_FRAME_A4 28
#define RISCV_INTERRUPT_FRAME_A5 32
#define RISCV_INTERRUPT_FRAME_A6 36
#define RISCV_INTERRUPT_FRAME_A7 40
#define RISCV_INTERRUPT_FRAME_T0 44
#define RISCV_INTERRUPT_FRAME_T1 48
#define RISCV_INTERRUPT_FRAME_T2 52
#define RISCV_INTERRUPT_FRAME_T3 56
#define RISCV_INTERRUPT_FRAME_T4 60
#define RISCV_INTERRUPT_FRAME_T5 64
#define RISCV_INTERRUPT_FRAME_T6 68
#define RISCV_INTERRUPT_FRAME_A0 72
#define RISCV_INTERRUPT_FRAME_A1 76

#define CPU_INTERRUPT_FRAME_SIZE 80

#elif __riscv_xlen == 64

#define RISCV_CONTEXT_RA 8
#define RISCV_CONTEXT_SP 16
#define RISCV_CONTEXT_TP 24
#define RISCV_CONTEXT_S0 32
#define RISCV_CONTEXT_S1 40
#define RISCV_CONTEXT_S2 48
#define RISCV_CONTEXT_S3 56
#define RISCV_CONTEXT_S4 64
#define RISCV_CONTEXT_S5 72
#define RISCV_CONTEXT_S6 80
#define RISCV_CONTEXT_S7 88
#define RISCV_CONTEXT_S8 96
#define RISCV_CONTEXT_S9 104
#define RISCV_CONTEXT_S10 112
#define RISCV_CONTEXT_S11 120

#define RISCV_INTERRUPT_FRAME_MSTATUS 0
#define RISCV_INTERRUPT_FRAME_MEPC 8
#define RISCV_INTERRUPT_FRAME_A2 16
#define RISCV_INTERRUPT_FRAME_S0 24
#define RISCV_INTERRUPT_FRAME_S1 32
#define RISCV_INTERRUPT_FRAME_RA 40
#define RISCV_INTERRUPT_FRAME_A3 48
#define RISCV_INTERRUPT_FRAME_A4 56
#define RISCV_INTERRUPT_FRAME_A5 64
#define RISCV_INTERRUPT_FRAME_A6 72
#define RISCV_INTERRUPT_FRAME_A7 80
#define RISCV_INTERRUPT_FRAME_T0 88
#define RISCV_INTERRUPT_FRAME_T1 96
#define RISCV_INTERRUPT_FRAME_T2 104
#define RISCV_INTERRUPT_FRAME_T3 112
#define RISCV_INTERRUPT_FRAME_T4 120
#define RISCV_INTERRUPT_FRAME_T5 128
#define RISCV_INTERRUPT_FRAME_T6 136
#define RISCV_INTERRUPT_FRAME_A0 144
#define RISCV_INTERRUPT_FRAME_A1 152

#define CPU_INTERRUPT_FRAME_SIZE 160

#endif /* __riscv_xlen */

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uintptr_t mstatus;
  uintptr_t mepc;
  uintptr_t a2;
  uintptr_t s0;
  uintptr_t s1;
  uintptr_t ra;
  uintptr_t a3;
  uintptr_t a4;
  uintptr_t a5;
  uintptr_t a6;
  uintptr_t a7;
  uintptr_t t0;
  uintptr_t t1;
  uintptr_t t2;
  uintptr_t t3;
  uintptr_t t4;
  uintptr_t t5;
  uintptr_t t6;
  uintptr_t a0;
  uintptr_t a1;
} RTEMS_ALIGNED( CPU_STACK_ALIGNMENT ) CPU_Interrupt_frame;

#ifdef RTEMS_SMP

static inline struct Per_CPU_Control *_RISCV_Get_current_per_CPU_control( void )
{
  struct Per_CPU_Control *cpu_self;

  __asm__ volatile ( "csrr %0, mscratch" : "=r" ( cpu_self ) );

  return cpu_self;
}

#define _CPU_Get_current_per_CPU_control() _RISCV_Get_current_per_CPU_control()

#endif /* RTEMS_SMP */

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif /* _RTEMS_SCORE_CPUIMPL_H */
