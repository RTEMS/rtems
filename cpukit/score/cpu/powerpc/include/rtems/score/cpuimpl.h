/**
 * @file
 *
 * @brief CPU Port Implementation API
 */

/*
 * Copyright (C) 1999 Eric Valette (valette@crf.canon.fr)
 *                    Canon Centre Recherche France.
 *
 * Copyright (C) 2007 Till Straumann <strauman@slac.stanford.edu>
 *
 * Copyright (c) 2009, 2017 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CPUIMPL_H
#define _RTEMS_SCORE_CPUIMPL_H

#include <rtems/score/cpu.h>

/**
 * @defgroup RTEMSScoreCPUPowerPC PowerPC
 *
 * @ingroup RTEMSScoreCPU
 *
 * @brief PowerPC Architecture Support
 *
 * @{
 */

/* Exception stack frame -> BSP_Exception_frame */
#ifdef __powerpc64__
  #define FRAME_LINK_SPACE 32
#else
  #define FRAME_LINK_SPACE 8
#endif

#define SRR0_FRAME_OFFSET FRAME_LINK_SPACE
#define SRR1_FRAME_OFFSET (SRR0_FRAME_OFFSET + PPC_REG_SIZE)
#define EXCEPTION_NUMBER_OFFSET (SRR1_FRAME_OFFSET + PPC_REG_SIZE)
#define PPC_EXC_INTERRUPT_ENTRY_INSTANT_OFFSET (EXCEPTION_NUMBER_OFFSET + 4)
#define EXC_CR_OFFSET (EXCEPTION_NUMBER_OFFSET + 8)
#define EXC_XER_OFFSET (EXC_CR_OFFSET + 4)
#define EXC_CTR_OFFSET (EXC_XER_OFFSET + 4)
#define EXC_LR_OFFSET (EXC_CTR_OFFSET + PPC_REG_SIZE)
#define PPC_EXC_INTERRUPT_FRAME_OFFSET (EXC_LR_OFFSET + PPC_REG_SIZE)

#ifndef __SPE__
  #define PPC_EXC_GPR_OFFSET(gpr) \
    ((gpr) * PPC_GPR_SIZE + PPC_EXC_INTERRUPT_FRAME_OFFSET + PPC_REG_SIZE)
  #define PPC_EXC_GPR3_PROLOGUE_OFFSET PPC_EXC_GPR_OFFSET(3)
  #if defined(PPC_MULTILIB_ALTIVEC) && defined(PPC_MULTILIB_FPU)
    #define PPC_EXC_VRSAVE_OFFSET PPC_EXC_GPR_OFFSET(33)
    #define PPC_EXC_VSCR_OFFSET (PPC_EXC_VRSAVE_OFFSET + 28)
    #define PPC_EXC_VR_OFFSET(v) ((v) * 16 + PPC_EXC_VSCR_OFFSET + 4)
    #define PPC_EXC_FR_OFFSET(f) ((f) * 8 + PPC_EXC_VR_OFFSET(32))
    #define PPC_EXC_FPSCR_OFFSET PPC_EXC_FR_OFFSET(32)
    #define PPC_EXC_FRAME_SIZE PPC_EXC_FR_OFFSET(34)
    #define PPC_EXC_MIN_VSCR_OFFSET (PPC_EXC_GPR_OFFSET(13) + 12)
    #define PPC_EXC_MIN_VR_OFFSET(v) ((v) * 16 + PPC_EXC_MIN_VSCR_OFFSET + 4)
    #define PPC_EXC_MIN_FR_OFFSET(f) ((f) * 8 + PPC_EXC_MIN_VR_OFFSET(20))
    #define PPC_EXC_MIN_FPSCR_OFFSET PPC_EXC_MIN_FR_OFFSET(14)
    #define CPU_INTERRUPT_FRAME_SIZE \
      (PPC_EXC_MIN_FR_OFFSET(16) + PPC_STACK_RED_ZONE_SIZE)
  #elif defined(PPC_MULTILIB_ALTIVEC)
    #define PPC_EXC_VRSAVE_OFFSET PPC_EXC_GPR_OFFSET(33)
    #define PPC_EXC_VSCR_OFFSET (PPC_EXC_VRSAVE_OFFSET + 28)
    #define PPC_EXC_VR_OFFSET(v) ((v) * 16 + PPC_EXC_VSCR_OFFSET + 4)
    #define PPC_EXC_FRAME_SIZE PPC_EXC_VR_OFFSET(32)
    #define PPC_EXC_MIN_VSCR_OFFSET (PPC_EXC_GPR_OFFSET(13) + 12)
    #define PPC_EXC_MIN_VR_OFFSET(v) ((v) * 16 + PPC_EXC_MIN_VSCR_OFFSET + 4)
    #define CPU_INTERRUPT_FRAME_SIZE \
      (PPC_EXC_MIN_VR_OFFSET(20) + PPC_STACK_RED_ZONE_SIZE)
  #elif defined(PPC_MULTILIB_FPU)
    #define PPC_EXC_FR_OFFSET(f) ((f) * 8 + PPC_EXC_GPR_OFFSET(33))
    #define PPC_EXC_FPSCR_OFFSET PPC_EXC_FR_OFFSET(32)
    #define PPC_EXC_FRAME_SIZE PPC_EXC_FR_OFFSET(34)
    #define PPC_EXC_MIN_FR_OFFSET(f) ((f) * 8 + PPC_EXC_GPR_OFFSET(13))
    #define PPC_EXC_MIN_FPSCR_OFFSET PPC_EXC_MIN_FR_OFFSET(14)
    #define CPU_INTERRUPT_FRAME_SIZE \
      (PPC_EXC_MIN_FR_OFFSET(16) + PPC_STACK_RED_ZONE_SIZE)
  #else
    #define PPC_EXC_FRAME_SIZE PPC_EXC_GPR_OFFSET(33)
    #define CPU_INTERRUPT_FRAME_SIZE \
      (PPC_EXC_GPR_OFFSET(13) + PPC_STACK_RED_ZONE_SIZE)
  #endif
#else
  #define PPC_EXC_SPEFSCR_OFFSET 44
  #define PPC_EXC_ACC_OFFSET 48
  #define PPC_EXC_GPR_OFFSET(gpr) ((gpr) * PPC_GPR_SIZE + 56)
  #define PPC_EXC_GPR3_PROLOGUE_OFFSET (PPC_EXC_GPR_OFFSET(3) + 4)
  #define CPU_INTERRUPT_FRAME_SIZE (160 + PPC_STACK_RED_ZONE_SIZE)
  #define PPC_EXC_FRAME_SIZE 320
#endif

#define GPR0_OFFSET PPC_EXC_GPR_OFFSET(0)
#define GPR1_OFFSET PPC_EXC_GPR_OFFSET(1)
#define GPR2_OFFSET PPC_EXC_GPR_OFFSET(2)
#define GPR3_OFFSET PPC_EXC_GPR_OFFSET(3)
#define GPR4_OFFSET PPC_EXC_GPR_OFFSET(4)
#define GPR5_OFFSET PPC_EXC_GPR_OFFSET(5)
#define GPR6_OFFSET PPC_EXC_GPR_OFFSET(6)
#define GPR7_OFFSET PPC_EXC_GPR_OFFSET(7)
#define GPR8_OFFSET PPC_EXC_GPR_OFFSET(8)
#define GPR9_OFFSET PPC_EXC_GPR_OFFSET(9)
#define GPR10_OFFSET PPC_EXC_GPR_OFFSET(10)
#define GPR11_OFFSET PPC_EXC_GPR_OFFSET(11)
#define GPR12_OFFSET PPC_EXC_GPR_OFFSET(12)
#define GPR13_OFFSET PPC_EXC_GPR_OFFSET(13)
#define GPR14_OFFSET PPC_EXC_GPR_OFFSET(14)
#define GPR15_OFFSET PPC_EXC_GPR_OFFSET(15)
#define GPR16_OFFSET PPC_EXC_GPR_OFFSET(16)
#define GPR17_OFFSET PPC_EXC_GPR_OFFSET(17)
#define GPR18_OFFSET PPC_EXC_GPR_OFFSET(18)
#define GPR19_OFFSET PPC_EXC_GPR_OFFSET(19)
#define GPR20_OFFSET PPC_EXC_GPR_OFFSET(20)
#define GPR21_OFFSET PPC_EXC_GPR_OFFSET(21)
#define GPR22_OFFSET PPC_EXC_GPR_OFFSET(22)
#define GPR23_OFFSET PPC_EXC_GPR_OFFSET(23)
#define GPR24_OFFSET PPC_EXC_GPR_OFFSET(24)
#define GPR25_OFFSET PPC_EXC_GPR_OFFSET(25)
#define GPR26_OFFSET PPC_EXC_GPR_OFFSET(26)
#define GPR27_OFFSET PPC_EXC_GPR_OFFSET(27)
#define GPR28_OFFSET PPC_EXC_GPR_OFFSET(28)
#define GPR29_OFFSET PPC_EXC_GPR_OFFSET(29)
#define GPR30_OFFSET PPC_EXC_GPR_OFFSET(30)
#define GPR31_OFFSET PPC_EXC_GPR_OFFSET(31)

#define CPU_PER_CPU_CONTROL_SIZE 0

#ifdef RTEMS_SMP

/* Use SPRG0 for the per-CPU control of the current processor */
#define PPC_PER_CPU_CONTROL_REGISTER 272

#endif /* RTEMS_SMP */

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uintptr_t FRAME_SP;
  #ifdef __powerpc64__
    uint32_t FRAME_CR;
    uint32_t FRAME_RESERVED;
  #endif
  uintptr_t FRAME_LR;
  #ifdef __powerpc64__
    uintptr_t FRAME_TOC;
  #endif
  uintptr_t EXC_SRR0;
  uintptr_t EXC_SRR1;
  uint32_t RESERVED_FOR_ALIGNMENT_0;
  uint32_t EXC_INTERRUPT_ENTRY_INSTANT;
  uint32_t EXC_CR;
  uint32_t EXC_XER;
  uintptr_t EXC_CTR;
  uintptr_t EXC_LR;
  uintptr_t EXC_INTERRUPT_FRAME;
  #ifdef __SPE__
    uint32_t EXC_SPEFSCR;
    uint64_t EXC_ACC;
  #endif
  PPC_GPR_TYPE GPR0;
  PPC_GPR_TYPE GPR1;
  PPC_GPR_TYPE GPR2;
  PPC_GPR_TYPE GPR3;
  PPC_GPR_TYPE GPR4;
  PPC_GPR_TYPE GPR5;
  PPC_GPR_TYPE GPR6;
  PPC_GPR_TYPE GPR7;
  PPC_GPR_TYPE GPR8;
  PPC_GPR_TYPE GPR9;
  PPC_GPR_TYPE GPR10;
  PPC_GPR_TYPE GPR11;
  PPC_GPR_TYPE GPR12;
  #ifdef PPC_MULTILIB_ALTIVEC
    /* This field must take stvewx/lvewx requirements into account */
    uint32_t RESERVED_FOR_ALIGNMENT_3[3];
    uint32_t VSCR;

    uint8_t V0[16];
    uint8_t V1[16];
    uint8_t V2[16];
    uint8_t V3[16];
    uint8_t V4[16];
    uint8_t V5[16];
    uint8_t V6[16];
    uint8_t V7[16];
    uint8_t V8[16];
    uint8_t V9[16];
    uint8_t V10[16];
    uint8_t V11[16];
    uint8_t V12[16];
    uint8_t V13[16];
    uint8_t V14[16];
    uint8_t V15[16];
    uint8_t V16[16];
    uint8_t V17[16];
    uint8_t V18[16];
    uint8_t V19[16];
  #endif
  #ifdef PPC_MULTILIB_FPU
    double F0;
    double F1;
    double F2;
    double F3;
    double F4;
    double F5;
    double F6;
    double F7;
    double F8;
    double F9;
    double F10;
    double F11;
    double F12;
    double F13;
    uint64_t FPSCR;
    uint64_t RESERVED_FOR_ALIGNMENT_4;
  #endif
  #if PPC_STACK_RED_ZONE_SIZE > 0
    uint8_t RED_ZONE[ PPC_STACK_RED_ZONE_SIZE ];
  #endif
} CPU_Interrupt_frame;

#ifdef RTEMS_SMP

static inline struct Per_CPU_Control *_PPC_Get_current_per_CPU_control( void )
{
  struct Per_CPU_Control *cpu_self;

  __asm__ volatile (
    "mfspr %0, " RTEMS_XSTRING( PPC_PER_CPU_CONTROL_REGISTER )
    : "=r" ( cpu_self )
  );

  return cpu_self;
}

#define _CPU_Get_current_per_CPU_control() _PPC_Get_current_per_CPU_control()

#endif /* RTEMS_SMP */

void _CPU_Context_volatile_clobber( uintptr_t pattern );

void _CPU_Context_validate( uintptr_t pattern );

RTEMS_INLINE_ROUTINE void _CPU_Instruction_illegal( void )
{
  __asm__ volatile ( ".long 0" );
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
