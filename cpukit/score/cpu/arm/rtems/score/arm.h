/**
 * @file
 *
 * @brief ARM Assembler Support API
 */

/*
 *  COPYRIGHT (c) 2000 Canon Research Centre France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  Copyright (c) 2002 Advent Networks, Inc.
 *       Jay Monkman <jmonkman@adventnetworks.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#ifndef _RTEMS_SCORE_ARM_H
#define _RTEMS_SCORE_ARM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreCPU
 */
/**@{**/

#if defined(__ARM_ARCH_7M__)
  #define CPU_MODEL_NAME "ARMv7M"
  #define ARM_MULTILIB_ARCH_V7M
#else
  #define CPU_MODEL_NAME "ARMv4"
  #define ARM_MULTILIB_ARCH_V4
#endif

/* All ARM CPUs are assumed to not have floating point units */
#if defined(__SOFTFP__)
#define ARM_HAS_FPU     0
#else
#define ARM_HAS_FPU	1
#warning "FPU-support not yet implemented for the arm"
#endif


/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "ARM"

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_ARM_H */
