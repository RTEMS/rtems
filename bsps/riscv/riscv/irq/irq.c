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

/*
 * The lovely PLIC has an interrupt enable bit per hart for each interrupt
 * source.  This makes the interrupt enable/disable a bit difficult.  We have
 * to store the interrupt distribution in software.  To keep it simple, we
 * support only a one-to-one and one-to-all interrupt to processor
 * distribution.  For a one-to-one distribution, the array member must point to
 * the enable register block of the corresponding.  For a one-to-all
 * distribution, the array member must be NULL.  The array index is the
 * external interrupt index minus one (external interrupt index zero is a
 * special value, see PLIC documentation).
 */
static volatile uint32_t *
riscv_plic_irq_to_cpu[RISCV_MAXIMUM_EXTERNAL_INTERRUPTS];

RTEMS_INTERRUPT_LOCK_DEFINE(static, riscv_plic_lock, "PLIC")

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
    volatile RISCV_PLIC_hart_regs *plic_hart_regs;
    uint32_t interrupt_index;

    plic_hart_regs = cpu_self->cpu_per_cpu.plic_hart_regs;

    while ((interrupt_index = plic_hart_regs->claim_complete) != 0) {
      bsp_interrupt_handler_dispatch(
        RISCV_INTERRUPT_VECTOR_EXTERNAL(interrupt_index)
      );

      plic_hart_regs->claim_complete = interrupt_index;

      /*
       * FIXME: It is not clear which fence is necessary here or if a fence is
       * necessary at all.  The goal is that the complete signal is somehow
       * recognized by the PLIC before the next claim is issued.
       */
      __asm__ volatile ("fence o, i" : : : "memory");
    }
  } else if (mcause == (RISCV_INTERRUPT_SOFTWARE_MACHINE << 1)) {
#ifdef RTEMS_SMP
    /*
     * Clear the software interrupt on this processor.  Synchronization of
     * inter-processor interrupts is done via Per_CPU_Control::message in
     * _SMP_Inter_processor_interrupt_handler().
     */
    *cpu_self->cpu_per_cpu.clint_msip = 0;

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
  const uint32_t *val;
  int len;
  int i;

  node = fdt_node_offset_by_compatible(fdt, -1, "riscv,clint0");

  clint = riscv_fdt_get_address(fdt, node);
  if (clint == NULL) {
    bsp_fatal(RISCV_FATAL_NO_CLINT_REG_IN_DEVICE_TREE);
  }

  riscv_clint = clint;

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
}

static void riscv_plic_init(const void *fdt)
{
  volatile RISCV_PLIC_regs *plic;
  int node;
  int i;
  const uint32_t *val;
  int len;
  uint32_t interrupt_index;
  uint32_t ndev;
  Per_CPU_Control *cpu;

  node = fdt_node_offset_by_compatible(fdt, -1, "riscv,plic0");

  plic = riscv_fdt_get_address(fdt, node);
  if (plic == NULL) {
#if RISCV_ENABLE_HTIF_SUPPORT != 0
    /* Spike platform has HTIF and does not have a PLIC */
    return;
#else
    bsp_fatal(RISCV_FATAL_NO_PLIC_REG_IN_DEVICE_TREE);
#endif
  }

  val = fdt_getprop(fdt, node, "riscv,ndev", &len);
  if (val == NULL || len != 4) {
    bsp_fatal(RISCV_FATAL_INVALID_PLIC_NDEV_IN_DEVICE_TREE);
  }

  ndev = fdt32_to_cpu(val[0]);
  if (ndev > RISCV_MAXIMUM_EXTERNAL_INTERRUPTS) {
    bsp_fatal(RISCV_FATAL_TOO_LARGE_PLIC_NDEV_IN_DEVICE_TREE);
  }

  val = fdt_getprop(fdt, node, "interrupts-extended", &len);

  for (i = 0; i < len; i += 8) {
    uint32_t hart_index;

    hart_index = riscv_get_hart_index_by_phandle(fdt32_to_cpu(val[i / 4]));
    if (hart_index >= rtems_configuration_get_maximum_processors()) {
      continue;
    }

    interrupt_index = fdt32_to_cpu(val[i / 4 + 1]);
    if (interrupt_index != RISCV_INTERRUPT_EXTERNAL_MACHINE) {
      continue;
    }

    plic->harts[i / 8].priority_threshold = 0;

    cpu = _Per_CPU_Get_by_index(hart_index);
    cpu->cpu_per_cpu.plic_hart_regs = &plic->harts[i / 8];
    cpu->cpu_per_cpu.plic_m_ie = &plic->enable[i / 8][0];
  }

  cpu = _Per_CPU_Get_by_index(0);

  for (interrupt_index = 1; interrupt_index <= ndev; ++interrupt_index) {
    plic->priority[interrupt_index] = 1;
    riscv_plic_irq_to_cpu[interrupt_index - 1] = cpu->cpu_per_cpu.plic_m_ie;
  }

  /*
   * External M-mode interrupts on secondary processors are enabled in
   * bsp_start_on_secondary_processor().
   */
  set_csr(mie, MIP_MEIP);
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
  const void *fdt;

  fdt = bsp_fdt_get();
  riscv_clint_init(fdt);
  riscv_plic_init(fdt);

  return RTEMS_SUCCESSFUL;
}

void bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (RISCV_INTERRUPT_VECTOR_IS_EXTERNAL(vector)) {
    uint32_t interrupt_index;
    volatile uint32_t *enable;
    uint32_t group;
    uint32_t bit;
    rtems_interrupt_lock_context lock_context;

    interrupt_index = RISCV_INTERRUPT_VECTOR_EXTERNAL_TO_INDEX(vector);
    enable = riscv_plic_irq_to_cpu[interrupt_index - 1];
    group = interrupt_index / 32;
    bit = UINT32_C(1) << (interrupt_index % 32);

    rtems_interrupt_lock_acquire(&riscv_plic_lock, &lock_context);

    if (enable != NULL) {
      enable[group] |= bit;
    } else {
      uint32_t cpu_max;
      uint32_t cpu_index;

      cpu_max = _SMP_Get_processor_maximum();

      for (cpu_index = 0; cpu_index < cpu_max; ++cpu_index) {
        Per_CPU_Control *cpu;

        cpu = _Per_CPU_Get_by_index(cpu_index);
        enable = cpu->cpu_per_cpu.plic_m_ie;

        if (enable != NULL) {
          enable[group] |= bit;
        }
      }
    }

    rtems_interrupt_lock_release(&riscv_plic_lock, &lock_context);
  }
}

void bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (RISCV_INTERRUPT_VECTOR_IS_EXTERNAL(vector)) {
    uint32_t interrupt_index;
    volatile uint32_t *enable;
    uint32_t group;
    uint32_t bit;
    rtems_interrupt_lock_context lock_context;

    interrupt_index = RISCV_INTERRUPT_VECTOR_EXTERNAL_TO_INDEX(vector);
    enable = riscv_plic_irq_to_cpu[interrupt_index - 1];
    group = interrupt_index / 32;
    bit = UINT32_C(1) << (interrupt_index % 32);

    rtems_interrupt_lock_acquire(&riscv_plic_lock, &lock_context);

    if (enable != NULL) {
      enable[group] &= ~bit;
    } else {
      uint32_t cpu_max;
      uint32_t cpu_index;

      cpu_max = _SMP_Get_processor_maximum();

      for (cpu_index = 0; cpu_index < cpu_max; ++cpu_index) {
        Per_CPU_Control *cpu;

        cpu = _Per_CPU_Get_by_index(cpu_index);
        enable = cpu->cpu_per_cpu.plic_m_ie;

        if (enable != NULL) {
          enable[group] &= ~bit;
        }
      }
    }

    rtems_interrupt_lock_release(&riscv_plic_lock, &lock_context);
  }
}

void bsp_interrupt_set_affinity(
  rtems_vector_number vector,
  const Processor_mask *affinity
)
{
  if (RISCV_INTERRUPT_VECTOR_IS_EXTERNAL(vector)) {
    uint32_t interrupt_index;
    Processor_mask mask;

    interrupt_index = RISCV_INTERRUPT_VECTOR_EXTERNAL_TO_INDEX(vector);

    _Processor_mask_And(&mask, affinity, _SMP_Get_online_processors());

    if (_Processor_mask_Is_equal(&mask, _SMP_Get_online_processors())) {
      riscv_plic_irq_to_cpu[interrupt_index - 1] = NULL;
      return;
    }

    if (_Processor_mask_Count(&mask) == 1) {
      uint32_t cpu_index;
      Per_CPU_Control *cpu;

      cpu_index = _Processor_mask_Find_last_set(&mask) - 1;
      cpu = _Per_CPU_Get_by_index(cpu_index);
      riscv_plic_irq_to_cpu[interrupt_index - 1] = cpu->cpu_per_cpu.plic_m_ie;
      return;
    }

    bsp_fatal(RISCV_FATAL_INVALID_INTERRUPT_AFFINITY);
  }
}

void bsp_interrupt_get_affinity(
  rtems_vector_number vector,
  Processor_mask *affinity
)
{
  _Processor_mask_Zero(affinity);

  if (RISCV_INTERRUPT_VECTOR_IS_EXTERNAL(vector)) {
    uint32_t interrupt_index;
    volatile uint32_t *enable;

    interrupt_index = RISCV_INTERRUPT_VECTOR_EXTERNAL_TO_INDEX(vector);
    enable = riscv_plic_irq_to_cpu[interrupt_index - 1];

    if (enable != NULL) {
      uint32_t cpu_max;
      uint32_t cpu_index;

      cpu_max = _SMP_Get_processor_maximum();

      for (cpu_index = 0; cpu_index < cpu_max; ++cpu_index) {
        Per_CPU_Control *cpu;

        cpu = _Per_CPU_Get_by_index(cpu_index);

        if (enable == cpu->cpu_per_cpu.plic_m_ie) {
          _Processor_mask_Set(affinity, cpu_index);
          break;
        }
      }
    } else {
      _Processor_mask_Assign(affinity, _SMP_Get_online_processors());
    }
  }
}
