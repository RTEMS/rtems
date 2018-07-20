/**
 * @file
 *
 * @ingroup riscv_interrupt
 *
 * @brief Interrupt support.
 */

/*
 * Copyright (c) 2018 embedded brains GmbH
 *
 * Copyright (c) 2015 University of York.
 * Hesham Almatary <hesham@alumni.york.ac.uk>
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

#include <bsp/irq.h>
#include <bsp/fatal.h>
#include <bsp/fdt.h>
#include <bsp/irq-generic.h>
#include <bsp/riscv.h>

#include <rtems/score/percpu.h>
#include <rtems/score/riscv-utility.h>
#include <rtems/score/smpimpl.h>

#include <libfdt.h>

volatile RISCV_CLINT_regs *riscv_clint;

void _RISCV_Interrupt_dispatch(uintptr_t mcause, Per_CPU_Control *cpu_self)
{
  /*
   * Get rid of the most significant bit which indicates if the exception was
   * caused by an interrupt or not.
   */
  mcause <<= 1;

  if (mcause == (RISCV_INTERRUPT_TIMER_MACHINE << 1)) {
    bsp_interrupt_handler_dispatch(RISCV_INTERRUPT_VECTOR_TIMER);
  } else if (mcause == (RISCV_INTERRUPT_EXTERNAL_MACHINE << 1)) {
    /* TODO: Handle PLIC interrupt */
  } else if (mcause == (RISCV_INTERRUPT_SOFTWARE_MACHINE << 1)) {
#ifdef RTEMS_SMP
    clear_csr(mip, MIP_MSIP);
    _SMP_Inter_processor_interrupt_handler(cpu_self);
#else
    bsp_interrupt_handler_dispatch(RISCV_INTERRUPT_VECTOR_SOFTWARE);
#endif
  } else {
    bsp_fatal(RISCV_FATAL_UNEXPECTED_INTERRUPT_EXCEPTION);
  }
}

static void riscv_clint_init(const void *fdt)
{
  volatile RISCV_CLINT_regs *clint;
  int node;
#ifdef RTEMS_SMP
  const uint32_t *val;
  int len;
  int i;
#endif

  node = fdt_node_offset_by_compatible(fdt, -1, "riscv,clint0");

  clint = riscv_fdt_get_address(fdt, node);
  if (clint == NULL) {
    bsp_fatal(RISCV_FATAL_NO_CLINT_REG_IN_DEVICE_TREE);
  }

  riscv_clint = clint;

#ifdef RTEMS_SMP
  val = fdt_getprop(fdt, node, "interrupts-extended", &len);

  for (i = 0; i < len; i += 16) {
    uint32_t hart_index;
    Per_CPU_Control *cpu;

    hart_index = riscv_get_hart_index_by_phandle(fdt32_to_cpu(val[i / 4]));
    if (hart_index >= rtems_configuration_get_maximum_processors()) {
      continue;
    }

    cpu = _Per_CPU_Get_by_index(hart_index);
    cpu->cpu_per_cpu.clint_msip = &clint->msip[i / 16];
    cpu->cpu_per_cpu.clint_mtimecmp = &clint->mtimecmp[i / 16];
  }
#endif
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
  const void *fdt;

  fdt = bsp_fdt_get();
  riscv_clint_init(fdt);

  return RTEMS_SUCCESSFUL;
}

void bsp_interrupt_vector_enable(rtems_vector_number vector)
{
}

void bsp_interrupt_vector_disable(rtems_vector_number vector)
{
}
