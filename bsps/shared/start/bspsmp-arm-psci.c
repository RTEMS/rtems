/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsShared
 *
 * @brief PSCI-based BSP CPU start.
 */

/*
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
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

#include <rtems/score/smpimpl.h>

#include <bsp/start.h>
#include <bsp.h>
#if defined( AARCH64_MULTILIB_ARCH_V8 ) || \
  defined( AARCH64_MULTILIB_ARCH_V8_ILP32 )
#include <rtems/score/aarch64-system-registers.h>
#else
#include <rtems/score/aarch32-system-registers.h>
#endif

#if defined( AARCH64_MULTILIB_ARCH_V8 ) || \
  defined( AARCH64_MULTILIB_ARCH_V8_ILP32 )
#define REGISTER_PREFIX "x"
#else
#define REGISTER_PREFIX "r"
#endif

bool _CPU_SMP_Start_processor( uint32_t cpu_index )
{
#if defined( AARCH64_MULTILIB_ARCH_V8 ) || \
  defined( AARCH64_MULTILIB_ARCH_V8_ILP32 )
  uint32_t PSCI_FN_SYSTEM_CPU_ON = 0xC4000003;
  uint64_t target_cpu = _AArch64_Read_mpidr_el1();
  uint64_t ret;
#else
  uint32_t PSCI_FN_SYSTEM_CPU_ON = 0x84000003;
  uint32_t target_cpu = _AArch32_Read_mpidr();
  uint32_t ret;
#endif
  target_cpu &= ~( 0xff0000ffUL );
  target_cpu |= cpu_index;

  __asm__ volatile (
    "mov " REGISTER_PREFIX "0, %1\n"
    "mov " REGISTER_PREFIX "1, %2\n"
    "mov " REGISTER_PREFIX "2, %3\n"
    "mov " REGISTER_PREFIX "3, #0\n"
#ifdef BSP_CPU_ON_USES_SMC
    "smc #0\n"
#else
    "hvc #0\n"
#endif
    "mov %0, " REGISTER_PREFIX "0\n"
    : "=r" ( ret ) : "r" ( PSCI_FN_SYSTEM_CPU_ON ), "r" ( target_cpu ),
    "r" ( _start )
    : REGISTER_PREFIX "0", REGISTER_PREFIX "1", REGISTER_PREFIX "2",
    REGISTER_PREFIX "3"
  );

  if ( ret != 0 ) {
    return false;
  }

  return _Per_CPU_State_wait_for_non_initial_state( cpu_index, 0 );
}
