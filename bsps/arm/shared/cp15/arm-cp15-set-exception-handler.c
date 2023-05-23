/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMShared
 *
 * @brief This source file contains the implementation of
 *   arm_cp15_set_exception_handler().
 */

/*
 * Copyright (c) 2013 embedded brains GmbH & Co. KG
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

#include <libcpu/arm-cp15.h>

#include <bsp/linker-symbols.h>

void* arm_cp15_set_exception_handler(
  Arm_symbolic_exception_name exception,
  void (*handler)(void)
)
{
  uint32_t current_handler = 0;

  if ((unsigned) exception < MAX_EXCEPTIONS) {
    uint32_t *cpu_table = (uint32_t *) 0 + MAX_EXCEPTIONS;
    uint32_t *mirror_table = (uint32_t *) bsp_vector_table_begin + MAX_EXCEPTIONS;

    current_handler = mirror_table[exception];

    if (current_handler != (uint32_t) handler) {
      size_t table_size = MAX_EXCEPTIONS * sizeof(uint32_t);
      uint32_t cls = arm_cp15_get_min_cache_line_size();
      uint32_t ctrl;
      rtems_interrupt_level level;

      rtems_interrupt_local_disable(level);

      ctrl = arm_cp15_mmu_disable(cls);

      mirror_table[exception] = (uint32_t) handler;

      rtems_cache_flush_multiple_data_lines(mirror_table, table_size);

      /*
       * On ARMv7 processors with the Security Extension the mirror table might
       * be the actual table used by the processor.
       */
      rtems_cache_invalidate_multiple_instruction_lines(mirror_table, table_size);

      rtems_cache_invalidate_multiple_instruction_lines(cpu_table, table_size);

      arm_cp15_set_control(ctrl);

      rtems_interrupt_local_enable(level);
    }
  }

  return (void*) current_handler;
}
