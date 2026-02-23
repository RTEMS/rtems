/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsShared
 *
 * @brief This source file contains the default implementation for the
 *   _AArch_Get_PSCI_target_cpu() function.
 */

/*
 * Copyright (C) 2026 Preetam Das <riki10112001@gmail.com>
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

#include <bsp/start.h>
#include <bsp/bspsmp-arm-psci.h>
#include <bsp.h>

#if defined( AARCH64_MULTILIB_ARCH_V8 ) || \
 defined( AARCH64_MULTILIB_ARCH_V8_ILP32 )
#include <rtems/score/aarch64-system-registers.h>
#else
#include <rtems/score/aarch32-system-registers.h>
#endif

uintptr_t _AArch_Get_PSCI_target_cpu( uint32_t cpu_index )
{
  uintptr_t target_cpu;

#if defined( AARCH64_MULTILIB_ARCH_V8 ) || \
  defined( AARCH64_MULTILIB_ARCH_V8_ILP32 )
  target_cpu = _AArch64_Read_mpidr_el1();
  target_cpu &= ( 0x00ff00ffff00ULL );
#else
  target_cpu = _AArch32_Read_mpidr();
  target_cpu &= ( 0x00ffff00UL );
#endif

  target_cpu |= cpu_index;

  return target_cpu;
}
