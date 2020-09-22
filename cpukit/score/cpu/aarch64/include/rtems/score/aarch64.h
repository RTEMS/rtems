/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPU
 *
 * @brief ARM AArch64 Assembler Support API
 */

/*
 * Copyright (C) 2020 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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
 */

#ifndef _RTEMS_SCORE_AARCH64_H
#define _RTEMS_SCORE_AARCH64_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreCPUAArch64
 */
/**@{**/

#if defined(__LP64__)
  #define CPU_MODEL_NAME "AArch64-LP64"
  #define AARCH64_MULTILIB_ARCH_V8
#elif defined(__ILP32__)
  #define CPU_MODEL_NAME "AArch64-ILP32"
  #define AARCH64_MULTILIB_ARCH_V8_ILP32
#endif

#define AARCH64_MULTILIB_HAS_WFI
#define AARCH64_MULTILIB_HAS_LOAD_STORE_EXCLUSIVE
#define AARCH64_MULTILIB_HAS_BARRIER_INSTRUCTIONS
#define AARCH64_MULTILIB_HAS_CPACR

#define AARCH64_MULTILIB_CACHE_LINE_MAX_64

#if defined(__ARM_NEON)
  #define AARCH64_MULTILIB_VFP
#else
  #error "FPU support not implemented"
#endif


/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "AArch64"

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_AARCH64_H */
