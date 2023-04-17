/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64Shared
 *
 * @brief AArch64 MMU implementation.
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

#include <bsp/aarch64-mmu.h>
#include <rtems/score/cpu.h>

/*
 * This must have a non-header implementation because it is used by libdebugger.
 */
rtems_status_code aarch64_mmu_map(
  uintptr_t addr,
  uint64_t size,
  uint64_t flags
)
{
  rtems_status_code sc;
  ISR_Level        level;
  uint64_t max_mappable = 1LLU << aarch64_mmu_get_cpu_pa_bits();

  if ( addr >= max_mappable || (addr + size) > max_mappable ) {
    return RTEMS_INVALID_ADDRESS;
  }

  /*
   * Disable interrupts so they don't run while the MMU tables are being
   * modified.
   */
  _ISR_Local_disable( level );

  sc = aarch64_mmu_map_block(
    (uint64_t *) bsp_translation_table_base,
    0x0,
    addr,
    size,
    -1,
    flags
  );
  _AARCH64_Data_synchronization_barrier();
  __asm__ volatile(
    "tlbi vmalle1\n"
  );
  _AARCH64_Data_synchronization_barrier();
  _AARCH64_Instruction_synchronization_barrier();

  _ISR_Local_enable( level );

  return sc;
}
