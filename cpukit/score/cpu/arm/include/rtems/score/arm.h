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
 *  http://www.rtems.org/license/LICENSE.
 *
 */

#ifndef _RTEMS_SCORE_ARM_H
#define _RTEMS_SCORE_ARM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreCPUARM
 */
/**@{**/

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
  #define CPU_MODEL_NAME "ARMv7M"
  #define ARM_MULTILIB_ARCH_V7M
#elif defined(__ARM_ARCH_6M__)
  #define CPU_MODEL_NAME "ARMv6M"
  #define ARM_MULTILIB_ARCH_V6M
#else
  #define CPU_MODEL_NAME "ARMv4"
  #define ARM_MULTILIB_ARCH_V4
#endif

#if defined(__ARM_ARCH_7A__) \
  || defined(__ARM_ARCH_7R__) \
  || defined(__ARM_ARCH_7M__) \
  || defined(__ARM_ARCH_7EM__)
  #define ARM_MULTILIB_HAS_WFI
  #define ARM_MULTILIB_HAS_LOAD_STORE_EXCLUSIVE
  #define ARM_MULTILIB_HAS_BARRIER_INSTRUCTIONS
#endif

#ifndef ARM_DISABLE_THREAD_ID_REGISTER_USE
  #if defined(__ARM_ARCH_7A__) \
    || defined(__ARM_ARCH_7R__)
    #define ARM_MULTILIB_HAS_THREAD_ID_REGISTER
  #endif
#endif

#if defined(__ARM_ARCH_7A__)
  #define ARM_MULTILIB_CACHE_LINE_MAX_64
#endif

#if defined(__ARM_ARCH_7A__) \
  || defined(__ARM_ARCH_7M__) \
  || defined(__ARM_ARCH_7EM__)
  #define ARM_MULTILIB_HAS_CPACR
#endif

#if !defined(__SOFTFP__)
  #if defined(__ARM_NEON__)
    #define ARM_MULTILIB_VFP_D32
  #elif defined(__VFP_FP__)
    #define ARM_MULTILIB_VFP_D16
  #else
    #error "FPU support not implemented"
  #endif
#endif

#if defined(ARM_MULTILIB_VFP_D16) \
  || defined(ARM_MULTILIB_VFP_D32)
  #define ARM_MULTILIB_VFP
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
