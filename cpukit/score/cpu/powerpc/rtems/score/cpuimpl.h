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
 * Copyright (c) 2009, 2016 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CPUIMPL_H
#define _RTEMS_SCORE_CPUIMPL_H

#include <rtems/score/cpu.h>

#define SRR0_FRAME_OFFSET 8
#define SRR1_FRAME_OFFSET 12
#define EXCEPTION_NUMBER_OFFSET 16
#define EXC_CR_OFFSET 20
#define EXC_CTR_OFFSET 24
#define EXC_XER_OFFSET 28
#define EXC_LR_OFFSET 32
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

/* Exception stack frame -> BSP_Exception_frame */
#define FRAME_LINK_SPACE 8

#ifndef __SPE__
  #define PPC_EXC_GPR_OFFSET(gpr) ((gpr) * PPC_GPR_SIZE + 36)
  #define PPC_EXC_VECTOR_PROLOGUE_OFFSET PPC_EXC_GPR_OFFSET(4)
  #if defined(PPC_MULTILIB_ALTIVEC) && defined(PPC_MULTILIB_FPU)
    #define PPC_EXC_VRSAVE_OFFSET 168
    #define PPC_EXC_VSCR_OFFSET 172
    #define PPC_EXC_VR_OFFSET(v) ((v) * 16 + 176)
    #define PPC_EXC_FR_OFFSET(f) ((f) * 8 + 688)
    #define PPC_EXC_FPSCR_OFFSET 944
    #define PPC_EXC_FRAME_SIZE 960
    #define PPC_EXC_MIN_VSCR_OFFSET 92
    #define PPC_EXC_MIN_VR_OFFSET(v) ((v) * 16 + 96)
    #define PPC_EXC_MIN_FR_OFFSET(f) ((f) * 8 + 416)
    #define PPC_EXC_MIN_FPSCR_OFFSET 528
    #define CPU_INTERRUPT_FRAME_SIZE 544
  #elif defined(PPC_MULTILIB_ALTIVEC)
    #define PPC_EXC_VRSAVE_OFFSET 168
    #define PPC_EXC_VSCR_OFFSET 172
    #define PPC_EXC_VR_OFFSET(v) ((v) * 16 + 176)
    #define PPC_EXC_FRAME_SIZE 688
    #define PPC_EXC_MIN_VSCR_OFFSET 92
    #define PPC_EXC_MIN_VR_OFFSET(v) ((v) * 16 + 96)
    #define CPU_INTERRUPT_FRAME_SIZE 416
  #elif defined(PPC_MULTILIB_FPU)
    #define PPC_EXC_FR_OFFSET(f) ((f) * 8 + 168)
    #define PPC_EXC_FPSCR_OFFSET 424
    #define PPC_EXC_FRAME_SIZE 448
    #define PPC_EXC_MIN_FR_OFFSET(f) ((f) * 8 + 96)
    #define PPC_EXC_MIN_FPSCR_OFFSET 92
    #define CPU_INTERRUPT_FRAME_SIZE 224
  #else
    #define PPC_EXC_FRAME_SIZE 176
    #define CPU_INTERRUPT_FRAME_SIZE 96
  #endif
#else
  #define PPC_EXC_SPEFSCR_OFFSET 36
  #define PPC_EXC_ACC_OFFSET 40
  #define PPC_EXC_GPR_OFFSET(gpr) ((gpr) * PPC_GPR_SIZE + 48)
  #define PPC_EXC_VECTOR_PROLOGUE_OFFSET (PPC_EXC_GPR_OFFSET(4) + 4)
  #define CPU_INTERRUPT_FRAME_SIZE 160
  #define PPC_EXC_FRAME_SIZE 320
#endif

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
  uint32_t FRAME_SP;
  uint32_t FRAME_LR;
  uint32_t EXC_SRR0;
  uint32_t EXC_SRR1;
  uint32_t unused;
  uint32_t EXC_CR;
  uint32_t EXC_CTR;
  uint32_t EXC_XER;
  uint32_t EXC_LR;
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
  uint32_t EARLY_INSTANT;
  #ifdef PPC_MULTILIB_ALTIVEC
    /* This field must take stvewx/lvewx requirements into account */
    uint32_t VSCR;

    uint8_t V0[16] RTEMS_ALIGNED(16);
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

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif /* _RTEMS_SCORE_CPUIMPL_H */
