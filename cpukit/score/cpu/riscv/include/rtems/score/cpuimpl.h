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

/**
 * @defgroup RTEMSScoreCPURISCV RISC-V
 *
 * @ingroup RTEMSScoreCPU
 *
 * @brief RISCV Architecture Support
 *
 * @{
 */

#if defined(__riscv_atomic) && __riscv_xlen == 64
#define CPU_PER_CPU_CONTROL_SIZE 48
#elif defined(__riscv_atomic) && __riscv_xlen == 32
#define CPU_PER_CPU_CONTROL_SIZE 32
#elif __riscv_xlen == 64
#define CPU_PER_CPU_CONTROL_SIZE 32
#elif __riscv_xlen == 32
#define CPU_PER_CPU_CONTROL_SIZE 16
#endif

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

#if __riscv_flen == 0

#define RISCV_INTERRUPT_FRAME_A0 72
#define RISCV_INTERRUPT_FRAME_A1 76

#define CPU_INTERRUPT_FRAME_SIZE 80

#elif __riscv_flen == 32

#define RISCV_CONTEXT_FCSR 68

#define RISCV_CONTEXT_F( x ) ( 72 + 4 * x )

#define RISCV_INTERRUPT_FRAME_FCSR 72

#define RISCV_INTERRUPT_FRAME_F( x ) ( 76 + 4 * x )

#define RISCV_INTERRUPT_FRAME_A0 156
#define RISCV_INTERRUPT_FRAME_A1 160

#define CPU_INTERRUPT_FRAME_SIZE 176

#elif __riscv_flen == 64

#define RISCV_CONTEXT_FCSR 68

#define RISCV_CONTEXT_F( x ) ( 72 + 8 * x )

#define RISCV_INTERRUPT_FRAME_FCSR 72

#define RISCV_INTERRUPT_FRAME_F( x ) ( 80 + 8 * x )

#define RISCV_INTERRUPT_FRAME_A0 240
#define RISCV_INTERRUPT_FRAME_A1 244

#define CPU_INTERRUPT_FRAME_SIZE 256

#endif /* __riscv_flen */

#define RISCV_EXCEPTION_FRAME_X( x ) ( CPU_INTERRUPT_FRAME_SIZE + 4 * x )

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

#if __riscv_flen == 0

#define RISCV_INTERRUPT_FRAME_A0 144
#define RISCV_INTERRUPT_FRAME_A1 152

#define CPU_INTERRUPT_FRAME_SIZE 160

#elif __riscv_flen == 32

#define RISCV_CONTEXT_FCSR 128

#define RISCV_CONTEXT_F( x ) ( 132 + 4 * x )

#define RISCV_INTERRUPT_FRAME_FCSR 144

#define RISCV_INTERRUPT_FRAME_F( x ) ( 148 + 4 * x )

#define RISCV_INTERRUPT_FRAME_A0 232
#define RISCV_INTERRUPT_FRAME_A1 240

#define CPU_INTERRUPT_FRAME_SIZE 256

#elif __riscv_flen == 64

#define RISCV_CONTEXT_FCSR 128

#define RISCV_CONTEXT_F( x ) ( 136 + 8 * x )

#define RISCV_INTERRUPT_FRAME_FCSR 144

#define RISCV_INTERRUPT_FRAME_F( x ) ( 152 + 8 * x )

#define RISCV_INTERRUPT_FRAME_A0 312
#define RISCV_INTERRUPT_FRAME_A1 320

#define CPU_INTERRUPT_FRAME_SIZE 336

#endif /* __riscv_flen */

#define RISCV_EXCEPTION_FRAME_X( x ) ( CPU_INTERRUPT_FRAME_SIZE + 8 * x )

#endif /* __riscv_xlen */

#define RISCV_EXCEPTION_FRAME_MCAUSE RISCV_EXCEPTION_FRAME_X( 0 )
#define RISCV_EXCEPTION_FRAME_SP RISCV_EXCEPTION_FRAME_X( 1 )
#define RISCV_EXCEPTION_FRAME_GP RISCV_EXCEPTION_FRAME_X( 2 )
#define RISCV_EXCEPTION_FRAME_TP RISCV_EXCEPTION_FRAME_X( 3 )
#define RISCV_EXCEPTION_FRAME_S2 RISCV_EXCEPTION_FRAME_X( 4 )
#define RISCV_EXCEPTION_FRAME_S3 RISCV_EXCEPTION_FRAME_X( 5 )
#define RISCV_EXCEPTION_FRAME_S4 RISCV_EXCEPTION_FRAME_X( 6 )
#define RISCV_EXCEPTION_FRAME_S5 RISCV_EXCEPTION_FRAME_X( 7 )
#define RISCV_EXCEPTION_FRAME_S6 RISCV_EXCEPTION_FRAME_X( 8 )
#define RISCV_EXCEPTION_FRAME_S7 RISCV_EXCEPTION_FRAME_X( 9 )
#define RISCV_EXCEPTION_FRAME_S8 RISCV_EXCEPTION_FRAME_X( 10 )
#define RISCV_EXCEPTION_FRAME_S9 RISCV_EXCEPTION_FRAME_X( 11 )
#define RISCV_EXCEPTION_FRAME_S10 RISCV_EXCEPTION_FRAME_X( 12 )
#define RISCV_EXCEPTION_FRAME_S11 RISCV_EXCEPTION_FRAME_X( 13 )

#if __riscv_flen > 0

#define RISCV_CONTEXT_FS0 RISCV_CONTEXT_F( 0 )
#define RISCV_CONTEXT_FS1 RISCV_CONTEXT_F( 1 )
#define RISCV_CONTEXT_FS2 RISCV_CONTEXT_F( 2 )
#define RISCV_CONTEXT_FS3 RISCV_CONTEXT_F( 3 )
#define RISCV_CONTEXT_FS4 RISCV_CONTEXT_F( 4 )
#define RISCV_CONTEXT_FS5 RISCV_CONTEXT_F( 5 )
#define RISCV_CONTEXT_FS6 RISCV_CONTEXT_F( 6 )
#define RISCV_CONTEXT_FS7 RISCV_CONTEXT_F( 7 )
#define RISCV_CONTEXT_FS8 RISCV_CONTEXT_F( 8 )
#define RISCV_CONTEXT_FS9 RISCV_CONTEXT_F( 9 )
#define RISCV_CONTEXT_FS10 RISCV_CONTEXT_F( 10 )
#define RISCV_CONTEXT_FS11 RISCV_CONTEXT_F( 11 )

#define RISCV_INTERRUPT_FRAME_FT0 RISCV_INTERRUPT_FRAME_F( 0 )
#define RISCV_INTERRUPT_FRAME_FT1 RISCV_INTERRUPT_FRAME_F( 1 )
#define RISCV_INTERRUPT_FRAME_FT2 RISCV_INTERRUPT_FRAME_F( 2 )
#define RISCV_INTERRUPT_FRAME_FT3 RISCV_INTERRUPT_FRAME_F( 3 )
#define RISCV_INTERRUPT_FRAME_FT4 RISCV_INTERRUPT_FRAME_F( 4 )
#define RISCV_INTERRUPT_FRAME_FT5 RISCV_INTERRUPT_FRAME_F( 5 )
#define RISCV_INTERRUPT_FRAME_FT6 RISCV_INTERRUPT_FRAME_F( 6 )
#define RISCV_INTERRUPT_FRAME_FT7 RISCV_INTERRUPT_FRAME_F( 7 )
#define RISCV_INTERRUPT_FRAME_FT8 RISCV_INTERRUPT_FRAME_F( 8 )
#define RISCV_INTERRUPT_FRAME_FT9 RISCV_INTERRUPT_FRAME_F( 9 )
#define RISCV_INTERRUPT_FRAME_FT10 RISCV_INTERRUPT_FRAME_F( 10 )
#define RISCV_INTERRUPT_FRAME_FT11 RISCV_INTERRUPT_FRAME_F( 11 )
#define RISCV_INTERRUPT_FRAME_FA0 RISCV_INTERRUPT_FRAME_F( 12 )
#define RISCV_INTERRUPT_FRAME_FA1 RISCV_INTERRUPT_FRAME_F( 13 )
#define RISCV_INTERRUPT_FRAME_FA2 RISCV_INTERRUPT_FRAME_F( 14 )
#define RISCV_INTERRUPT_FRAME_FA3 RISCV_INTERRUPT_FRAME_F( 15 )
#define RISCV_INTERRUPT_FRAME_FA4 RISCV_INTERRUPT_FRAME_F( 16 )
#define RISCV_INTERRUPT_FRAME_FA5 RISCV_INTERRUPT_FRAME_F( 17 )
#define RISCV_INTERRUPT_FRAME_FA6 RISCV_INTERRUPT_FRAME_F( 18 )
#define RISCV_INTERRUPT_FRAME_FA7 RISCV_INTERRUPT_FRAME_F( 19 )

#if __riscv_flen == 32
#define RISCV_EXCEPTION_FRAME_F( x ) ( RISCV_EXCEPTION_FRAME_X( 14 ) + 4 * x )
#elif __riscv_flen == 64
#define RISCV_EXCEPTION_FRAME_F( x ) ( RISCV_EXCEPTION_FRAME_X( 14 ) + 8 * x )
#endif

#define RISCV_EXCEPTION_FRAME_FS0 RISCV_EXCEPTION_FRAME_F( 0 )
#define RISCV_EXCEPTION_FRAME_FS1 RISCV_EXCEPTION_FRAME_F( 1 )
#define RISCV_EXCEPTION_FRAME_FS2 RISCV_EXCEPTION_FRAME_F( 2 )
#define RISCV_EXCEPTION_FRAME_FS3 RISCV_EXCEPTION_FRAME_F( 3 )
#define RISCV_EXCEPTION_FRAME_FS4 RISCV_EXCEPTION_FRAME_F( 4 )
#define RISCV_EXCEPTION_FRAME_FS5 RISCV_EXCEPTION_FRAME_F( 5 )
#define RISCV_EXCEPTION_FRAME_FS6 RISCV_EXCEPTION_FRAME_F( 6 )
#define RISCV_EXCEPTION_FRAME_FS7 RISCV_EXCEPTION_FRAME_F( 7 )
#define RISCV_EXCEPTION_FRAME_FS8 RISCV_EXCEPTION_FRAME_F( 8 )
#define RISCV_EXCEPTION_FRAME_FS9 RISCV_EXCEPTION_FRAME_F( 9 )
#define RISCV_EXCEPTION_FRAME_FS10 RISCV_EXCEPTION_FRAME_F( 10 )
#define RISCV_EXCEPTION_FRAME_FS11 RISCV_EXCEPTION_FRAME_F( 11 )

#endif /* __riscv_flen */

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

/* Core Local Interruptor (CLINT) */

typedef union {
  uint64_t val_64;
  uint32_t val_32[2];
} RISCV_CLINT_timer_reg;

typedef struct {
  uint32_t msip[4096];
  RISCV_CLINT_timer_reg mtimecmp[2048];
  uint32_t reserved_8000[4094];
  RISCV_CLINT_timer_reg mtime;
  uint32_t reserved_c000[4096];
} RISCV_CLINT_regs;

/* Platform-Level Interrupt Controller (PLIC) */

#define RISCV_PLIC_MAX_INTERRUPTS 1024

typedef struct {
  uint32_t priority_threshold;
  uint32_t claim_complete;
  uint32_t reserved_8[1022];
} RISCV_PLIC_hart_regs;

typedef struct {
  uint32_t priority[RISCV_PLIC_MAX_INTERRUPTS];
  uint32_t pending[1024];
  uint32_t enable[16320][32];
  RISCV_PLIC_hart_regs harts[CPU_MAXIMUM_PROCESSORS];
} RISCV_PLIC_regs;

typedef struct {
#ifdef __riscv_atomic
  uint64_t clear_reservations;
  uint32_t reserved_for_alignment_of_interrupt_frame[ 2 ];
#endif
  volatile RISCV_PLIC_hart_regs *plic_hart_regs;
  volatile uint32_t *plic_m_ie;
  volatile RISCV_CLINT_timer_reg *clint_mtimecmp;
  volatile uint32_t *clint_msip;
} CPU_Per_CPU_control;

struct Per_CPU_Control;

void _RISCV_Interrupt_dispatch(
  uintptr_t               mcause,
  struct Per_CPU_Control *cpu_self
);

static inline uint32_t _RISCV_Read_FCSR( void )
{
  uint32_t fcsr;

  __asm__ volatile ( "frcsr %0" : "=&r" ( fcsr ) );

  return fcsr;
}

/*
 * The RISC-V ISA provides a rdtime instruction, however, it is implemented in
 * most chips via a trap-and-emulate.  Using this in machine mode makes no
 * sense.  Use the memory-mapped mtime register directly instead.  The address
 * of this register is platform-specific and provided via the device tree.
 *
 * To allow better code generation provide a const (_RISCV_Counter) and a
 * mutable (_RISCV_Counter_mutable) declaration for this pointer variable
 * (defined in assembler code).
 *
 * See code generated for this test case:
 *
 * extern volatile int * const c;
 *
 * extern volatile int *v;
 *
 * int fc(void)
 * {
 *   int a = *c;
 *   __asm__ volatile("" ::: "memory");
 *   return *c - a;
 * }
 *
 * int fv(void)
 * {
 *   int a = *v;
 *   __asm__ volatile("" ::: "memory");
 *   return *v - a;
 * }
 */
extern volatile uint32_t *_RISCV_Counter_mutable;

/*
 * Initial value of _RISCV_Counter and _RISCV_Counter_mutable.  Must be
 * provided by the BSP.
 */
extern volatile uint32_t _RISCV_Counter_register;

#ifdef RTEMS_SMP

static inline struct Per_CPU_Control *_RISCV_Get_current_per_CPU_control( void )
{
  struct Per_CPU_Control *cpu_self;

  __asm__ volatile ( "csrr %0, mscratch" : "=r" ( cpu_self ) );

  return cpu_self;
}

#define _CPU_Get_current_per_CPU_control() _RISCV_Get_current_per_CPU_control()

#endif /* RTEMS_SMP */

void _CPU_Context_volatile_clobber( uintptr_t pattern );

void _CPU_Context_validate( uintptr_t pattern );

RTEMS_INLINE_ROUTINE void _CPU_Instruction_illegal( void )
{
  __asm__ volatile ( "unimp" );
}

RTEMS_INLINE_ROUTINE void _CPU_Instruction_no_operation( void )
{
  __asm__ volatile ( "nop" );
}

#ifdef __cplusplus
}
#endif

#endif /* ASM */

/** @} */

#endif /* _RTEMS_SCORE_CPUIMPL_H */
