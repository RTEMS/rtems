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

  /* See Per_CPU_Control::Interrupt_frame */
  uint32_t reserved_for_alignment_of_interrupt_frame;
#endif
} CPU_Per_CPU_control;

/**
 * @brief The pointer to the current per-CPU control is available via register
 * g6.
 */
register struct Per_CPU_Control *_SPARC_Per_CPU_current __asm__( "g6" );

#define _CPU_Get_current_per_CPU_control() _SPARC_Per_CPU_current

#define _CPU_Get_thread_executing() ( _SPARC_Per_CPU_current->executing )

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif /* _RTEMS_SCORE_CPUIMPL_H */
