/**
 * @file
 *
 * @brief CPU Port Implementation API
 */

/*
 * Copyright (c) 1989, 2007 On-Line Applications Research Corporation (OAR)
 * Copyright (c) 2013, 2016 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CPUIMPL_H
#define _RTEMS_SCORE_CPUIMPL_H

#include <rtems/score/cpu.h>

/**
 * @defgroup RTEMSScoreCPUSPARC SPARC
 *
 * @ingroup RTEMSScoreCPU
 *
 * @brief SPARC Architecture Support
 *
 * @{
 */

/** This defines the size of the minimum stack frame. */
#define SPARC_MINIMUM_STACK_FRAME_SIZE 0x60

/*
 *  Offsets of fields with CPU_Interrupt_frame for assembly routines.
 */

/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_PSR_OFFSET         SPARC_MINIMUM_STACK_FRAME_SIZE + 0x00
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_PC_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x04
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_NPC_OFFSET         SPARC_MINIMUM_STACK_FRAME_SIZE + 0x08
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_G1_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x0c
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_G2_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x10
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_G3_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x14
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_G4_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x18
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_G5_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x1c
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_G7_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x24
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I0_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x28
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I1_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x2c
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I2_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x30
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I3_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x34
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I4_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x38
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I5_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x3c
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I6_FP_OFFSET       SPARC_MINIMUM_STACK_FRAME_SIZE + 0x40
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I7_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x44
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_Y_OFFSET           SPARC_MINIMUM_STACK_FRAME_SIZE + 0x48
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_TPC_OFFSET         SPARC_MINIMUM_STACK_FRAME_SIZE + 0x4c

/** This defines the size of the ISF area for use in assembly. */
#define CPU_INTERRUPT_FRAME_SIZE SPARC_MINIMUM_STACK_FRAME_SIZE + 0x50

#define SPARC_FP_CONTEXT_OFFSET_F0_F1 0
#define SPARC_FP_CONTEXT_OFFSET_F2_F3 8
#define SPARC_FP_CONTEXT_OFFSET_F4_F5 16
#define SPARC_FP_CONTEXT_OFFSET_F6_F7 24
#define SPARC_FP_CONTEXT_OFFSET_F8_F9 32
#define SPARC_FP_CONTEXT_OFFSET_F10_F11 40
#define SPARC_FP_CONTEXT_OFFSET_F12_F13 48
#define SPARC_FP_CONTEXT_OFFSET_F14_F15 56
#define SPARC_FP_CONTEXT_OFFSET_F16_F17 64
#define SPARC_FP_CONTEXT_OFFSET_F18_F19 72
#define SPARC_FP_CONTEXT_OFFSET_F20_F21 80
#define SPARC_FP_CONTEXT_OFFSET_F22_F23 88
#define SPARC_FP_CONTEXT_OFFSET_F24_F25 96
#define SPARC_FP_CONTEXT_OFFSET_F26_F27 104
#define SPARC_FP_CONTEXT_OFFSET_F28_F29 112
#define SPARC_FP_CONTEXT_OFFSET_F30_F31 120
#define SPARC_FP_CONTEXT_OFFSET_FSR 128

#if ( SPARC_HAS_FPU == 1 )
  #define CPU_PER_CPU_CONTROL_SIZE 8
#else
  #define CPU_PER_CPU_CONTROL_SIZE 0
#endif

#if ( SPARC_HAS_FPU == 1 )
  /**
   * @brief Offset of the CPU_Per_CPU_control::fsr field relative to the
   * Per_CPU_Control begin.
   */
  #define SPARC_PER_CPU_FSR_OFFSET 0

  #if defined(SPARC_USE_LAZY_FP_SWITCH)
    /**
     * @brief Offset of the CPU_Per_CPU_control::fp_owner field relative to the
     * Per_CPU_Control begin.
     */
    #define SPARC_PER_CPU_FP_OWNER_OFFSET 4
  #endif
#endif

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
#if ( SPARC_HAS_FPU == 1 )
  /**
   * @brief Memory location to store the FSR register during interrupt
   * processing.
   *
   * This is a write-only field.  The FSR is written to force a completion of
   * floating point operations in progress.
   */
  uint32_t fsr;

#if defined(SPARC_USE_LAZY_FP_SWITCH)
  /**
   * @brief The current floating point owner.
   */
  struct _Thread_Control *fp_owner;
#else
  /* See Per_CPU_Control::Interrupt_frame */
  uint32_t reserved_for_alignment_of_interrupt_frame;
#endif
#endif
} CPU_Per_CPU_control;

/**
 * @brief The pointer to the current per-CPU control is available via register
 * g6.
 */
register struct Per_CPU_Control *_SPARC_Per_CPU_current __asm__( "g6" );

#define _CPU_Get_current_per_CPU_control() _SPARC_Per_CPU_current

#define _CPU_Get_thread_executing() ( _SPARC_Per_CPU_current->executing )

void _CPU_Context_volatile_clobber( uintptr_t pattern );

void _CPU_Context_validate( uintptr_t pattern );

RTEMS_INLINE_ROUTINE void _CPU_Instruction_illegal( void )
{
  __asm__ volatile ( "unimp 0" );
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
