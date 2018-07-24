/*
 * Copyright (c) 2018 embedded brains GmbH
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <bsp/bootcard.h>
#include <bsp/irq.h>
#include <bsp/riscv.h>

#include <rtems/score/riscv-utility.h>
#include <rtems/score/smpimpl.h>

void bsp_start_on_secondary_processor(Per_CPU_Control *cpu_self)
{
  uint32_t cpu_index_self;

  cpu_index_self = _Per_CPU_Get_index(cpu_self);

  if (
    cpu_index_self < rtems_configuration_get_maximum_processors()
      && _SMP_Should_start_processor(cpu_index_self)
  ) {
    set_csr(mie, MIP_MSIP | MIP_MEIP);
    _SMP_Start_multitasking_on_secondary_processor(cpu_self);
  } else {
    _CPU_Thread_Idle_body(0);
  }
}

uint32_t _CPU_SMP_Initialize(void)
{
  return riscv_hart_count;
}

bool _CPU_SMP_Start_processor(uint32_t cpu_index)
{
  return true;
}

void _CPU_SMP_Finalize_initialization(uint32_t cpu_count)
{
  (void) cpu_count;
  set_csr(mie, MIP_MSIP);
}

void _CPU_SMP_Prepare_start_multitasking(void)
{
  /* Do nothing */
}

void _CPU_SMP_Send_interrupt(uint32_t target_processor_index)
{
  Per_CPU_Control *cpu;

  cpu = _Per_CPU_Get_by_index(target_processor_index);
  *cpu->cpu_per_cpu.clint_msip = 0x1;
}
