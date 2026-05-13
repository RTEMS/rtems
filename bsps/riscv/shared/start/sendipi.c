/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief RISC-V BSP shared implementation for sending an IPI.
 *
 */

/*
 * Copyright (C) 2026 Gedare Bloom.
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

#include <bsp.h>
#include <bsp/riscv.h>

#include <rtems/score/percpu.h>

#ifdef RISCV_USE_S_MODE
#include <machine/sbi.h>
#endif

void riscv_send_ipi(uint32_t target_processor_index)
{
#ifdef RISCV_USE_S_MODE
  uint32_t hartid;
  unsigned long hart_mask;

  hartid = _RISCV_Map_cpu_index_to_hartid(target_processor_index);
  hart_mask = 1UL << hartid;
  sbi_send_ipi(&hart_mask);
#else
  Per_CPU_Control *cpu;

  cpu = _Per_CPU_Get_by_index(target_processor_index);
  *cpu->cpu_per_cpu.clint_msip = 0x1;
#endif
}
