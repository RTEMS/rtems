/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * @brief This header file provides defines derived from ARM multilib defines.
 */

/*
 *  COPYRIGHT (c) 2000 Canon Research Centre France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  Copyright (c) 2002 Advent Networks, Inc.
 *       Jay Monkman <jmonkman@adventnetworks.com>
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef _RTEMS_SCORE_ARM_H
#define _RTEMS_SCORE_ARM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreCPUARM
 *
 * @{
 */

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

#if __ARM_ARCH >= 7
  #define ARM_MULTILIB_HAS_WFI
  #define ARM_MULTILIB_HAS_LOAD_STORE_EXCLUSIVE
  #define ARM_MULTILIB_HAS_BARRIER_INSTRUCTIONS
  #define ARM_MULTILIB_HAS_STORE_RETURN_STATE
#endif

#ifndef ARM_DISABLE_THREAD_ID_REGISTER_USE
  #if defined(__ARM_ARCH_7A__) \
    || defined(__ARM_ARCH_7R__) \
    || __ARM_ARCH >= 8 \
    || __ARM_ARCH == 6
    #define ARM_MULTILIB_HAS_THREAD_ID_REGISTER
  #endif
#endif

#if defined(__ARM_ARCH_7A__) || __ARM_ARCH >= 8
  #define ARM_MULTILIB_CACHE_LINE_MAX_64
#endif

#if __ARM_ARCH >= 7
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
