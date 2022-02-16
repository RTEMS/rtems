/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSMP
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreSMP which are used by the implementation and the
 *   @ref RTEMSImplApplConfig.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_SCORE_SMP_H
#define _RTEMS_SCORE_SMP_H

#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScoreSMP SMP Support
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the implementation to support SMP systems.
 *
 * @{
 */

/**
 * @brief The configured processor maximum.
 *
 * In SMP configurations, this constant is defined by the application
 * configuration via <rtems/confdefs.h>, otherwise it is a compile-time
 * constant with the value one.
 */
#if defined(RTEMS_SMP)
  extern const uint32_t _SMP_Processor_configured_maximum;
#else
  #define _SMP_Processor_configured_maximum 1
#endif

#if defined( RTEMS_SMP )
  extern uint32_t _SMP_Processor_maximum;

  static inline uint32_t _SMP_Get_processor_maximum( void )
  {
    return _SMP_Processor_maximum;
  }
#else
  #define _SMP_Get_processor_maximum() UINT32_C(1)
#endif

#if defined( RTEMS_SMP )
  static inline uint32_t _SMP_Get_current_processor( void )
  {
    return _CPU_SMP_Get_current_processor();
  }
#else
  #define _SMP_Get_current_processor() UINT32_C(0)
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
