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

#if defined(__ARM_ARCH_7A__) \
  || defined(__ARM_ARCH_7R__) \
  || defined(__ARM_ARCH_7M__)
  #define ARM_MULTILIB_HAS_WFI
#endif

#if defined(__ARM_NEON__)
  #define ARM_MULTILIB_VFP_D32
#elif !defined(__SOFTFP__)
  #error "FPU support not implemented"
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
