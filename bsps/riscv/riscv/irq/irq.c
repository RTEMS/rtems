/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup riscv_interrupt
 *
 * @brief Interrupt support.
 */

/*
 * Copyright (C) 2018, 2022 embedded brains GmbH & Co. KG
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
#include <rtems/score/scheduler.h>
#include <rtems/score/smpimpl.h>

#include <libfdt.h>

static volatile RISCV_PLIC_regs *riscv_plic;

volatile RISCV_CLINT_regs *riscv_clint;

#ifdef RTEMS_SMP
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
#endif

RTEMS_INTERRUPT_LOCK_DEFINE(static, riscv_plic_lock, "PLIC")

void _RISCV_Interrupt_dispatch(uintptr_t mcause, Per_CPU_Control *cpu_self)
{
  /*
   * Get rid of the most significant bit which indicates if the exception was
   * caused by an interrupt or not.
   */
  mcause <<= 1;

  if (mcause == (RISCV_INTERRUPT_TIMER_MACHINE << 1)) {
    bsp_interrupt_handler_dispatch_unchecked(RISCV_INTERRUPT_VECTOR_TIMER);
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
    /*
     * Clear the software interrupt on this processor.  Synchronization of
     * inter-processor interrupts is done via Per_CPU_Control::message in
     * _SMP_Inter_processor_interrupt_handler().
     */
    *cpu_self->cpu_per_cpu.clint_msip = 0;

#ifdef RTEMS_SMP
    _SMP_Inter_processor_interrupt_handler(cpu_self);
    bsp_interrupt_handler_dispatch_unlikely(RISCV_INTERRUPT_VECTOR_SOFTWARE);
#else
    bsp_interrupt_handler_dispatch_unchecked(RISCV_INTERRUPT_VECTOR_SOFTWARE);
#endif
  } else {
    bsp_fatal(RISCV_FATAL_UNEXPECTED_INTERRUPT_EXCEPTION);
  }
}

static void riscv_clint_per_cpu_init(
  volatile RISCV_CLINT_regs *clint,
  Per_CPU_Control *cpu,
  uint32_t index
)
{
  cpu->cpu_per_cpu.clint_msip = &clint->msip[index];
  cpu->cpu_per_cpu.clint_mtimecmp = &clint->mtimecmp[index];
}

static void riscv_plic_per_cpu_init(
  volatile RISCV_PLIC_regs *plic,
  uint32_t enable_register_count,
  Per_CPU_Control *cpu,
  uint32_t index
)
{
  volatile uint32_t *enable;
  uint32_t i;

  plic->harts[index].priority_threshold = 0;

  enable = &plic->enable[index][0];
  cpu->cpu_per_cpu.plic_m_ie = enable;
  cpu->cpu_per_cpu.plic_hart_regs = &plic->harts[index];

  for (i = 0; i < enable_register_count; ++i) {
    enable[i] = 0;
  }
}

static void riscv_plic_cpu_0_init(
  volatile RISCV_PLIC_regs *plic,
  uint32_t interrupt_last
)
{
#ifdef RTEMS_SMP
  Per_CPU_Control *cpu;
#endif
  uint32_t i;

#ifdef RTEMS_SMP
  cpu = _Per_CPU_Get_by_index(0);
#endif

  for (i = 1; i <= interrupt_last; ++i) {
    plic->priority[i] = 1;
#ifdef RTEMS_SMP
    riscv_plic_irq_to_cpu[i - 1] = cpu->cpu_per_cpu.plic_m_ie;
#endif
  }

  /*
   * External M-mode interrupts on secondary processors are enabled in
   * bsp_start_on_secondary_processor().
   */
  set_csr(mie, MIP_MEIP);
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
    uint32_t cpu_index;

    hart_index = riscv_get_hart_index_by_phandle(fdt32_to_cpu(val[i / 4]));

#ifdef RTEMS_SMP
    cpu_index = _RISCV_Map_hardid_to_cpu_index(hart_index);
    if (cpu_index >= rtems_configuration_get_maximum_processors()) {
      continue;
    }

    if ( _Scheduler_Initial_assignments[ cpu_index ].scheduler == NULL ) {
      /* Skip not configured processor */
      continue;
    }
#else
    if (hart_index != RISCV_BOOT_HARTID) {
      continue;
    }

    cpu_index = 0;
#endif

    riscv_clint_per_cpu_init(
      clint,
      _Per_CPU_Get_by_index(cpu_index),
      (uint32_t) (i / 16)
    );

#ifndef RTEMS_SMP
    break;
#endif
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

  node = fdt_node_offset_by_compatible(fdt, -1, "riscv,plic0");

  plic = riscv_fdt_get_address(fdt, node);

  if (plic == NULL) {
#ifdef RISCV_ENABLE_HTIF_SUPPORT
    node = fdt_node_offset_by_compatible(fdt, -1, "ucb,htif0");

    /* Spike platform has HTIF and does not have a PLIC */
    if (node >= 0) {
      return;
    } else {
      bsp_fatal(RISCV_FATAL_NO_PLIC_REG_IN_DEVICE_TREE);
    }
#else
    bsp_fatal(RISCV_FATAL_NO_PLIC_REG_IN_DEVICE_TREE);
#endif
  }

  riscv_plic = plic;

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
    uint32_t enable_register_count;
    uint32_t cpu_index;

    /*
     * Each interrupt enable register contains exactly 32 enable bits.
     * Calculate the enable register count based on the number of interrupts
     * supported by the PLIC.  Take the reserved interrupt ID zero into
     * account.
     */
    enable_register_count = RTEMS_ALIGN_UP(ndev + 1, 32) / 32;

    hart_index = riscv_get_hart_index_by_phandle(fdt32_to_cpu(val[i / 4]));

#ifdef RTEMS_SMP
    cpu_index = _RISCV_Map_hardid_to_cpu_index(hart_index);
    if (cpu_index >= rtems_configuration_get_maximum_processors()) {
      continue;
    }

    if ( _Scheduler_Initial_assignments[ cpu_index ].scheduler == NULL ) {
      /* Skip not configured processor */
      continue;
    }
#else
    if (hart_index != RISCV_BOOT_HARTID) {
      continue;
    }

    cpu_index = 0;
#endif

    interrupt_index = fdt32_to_cpu(val[i / 4 + 1]);
    if (interrupt_index != RISCV_INTERRUPT_EXTERNAL_MACHINE) {
      continue;
    }

    riscv_plic_per_cpu_init(
      plic,
      enable_register_count,
      _Per_CPU_Get_by_index(cpu_index),
      (uint32_t) (i / 8)
    );

#ifndef RTEMS_SMP
    break;
#endif
  }

  riscv_plic_cpu_0_init(plic, ndev);
}

void bsp_interrupt_facility_initialize(void)
{
  const void *fdt;

  fdt = bsp_fdt_get();
  riscv_clint_init(fdt);
  riscv_plic_init(fdt);
}

bool bsp_interrupt_is_valid_vector(rtems_vector_number vector)
{
  /*
   * The PLIC interrupt ID of zero is reserved.  For example, this ID is used
   * to indicate that no interrupt was claimed.
   */
  if (vector == RISCV_INTERRUPT_VECTOR_EXTERNAL(0)) {
    return false;
  }

  return vector < (rtems_vector_number) BSP_INTERRUPT_VECTOR_COUNT;
}

rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  attributes->is_maskable = true;
  attributes->can_enable = true;
  attributes->maybe_enable = true;
  attributes->can_disable = true;
  attributes->maybe_disable = true;
  attributes->can_raise = (vector == RISCV_INTERRUPT_VECTOR_SOFTWARE);
  attributes->can_raise_on = attributes->can_raise;
  attributes->cleared_by_acknowledge = true;
  attributes->can_get_affinity = RISCV_INTERRUPT_VECTOR_IS_EXTERNAL(vector);
  attributes->can_set_affinity = attributes->can_get_affinity;

  if (vector == RISCV_INTERRUPT_VECTOR_SOFTWARE) {
    attributes->trigger_signal = RTEMS_INTERRUPT_NO_SIGNAL;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(pending != NULL);

  if (RISCV_INTERRUPT_VECTOR_IS_EXTERNAL(vector)) {
    uint32_t interrupt_index;
    uint32_t group;
    uint32_t bit;

    interrupt_index = RISCV_INTERRUPT_VECTOR_EXTERNAL_TO_INDEX(vector);
    group = interrupt_index / 32;
    bit = UINT32_C(1) << (interrupt_index % 32);
    *pending = ((riscv_plic->pending[group] & bit) != 0);
    return RTEMS_SUCCESSFUL;
  }

  if (vector == RISCV_INTERRUPT_VECTOR_TIMER) {
    *pending = (read_csr(mip) & MIP_MTIP) != 0;
    return RTEMS_SUCCESSFUL;
  }

  _Assert(vector == RISCV_INTERRUPT_VECTOR_SOFTWARE);
  *pending = (read_csr(mip) & MIP_MSIP) != 0;
  return RTEMS_SUCCESSFUL;
}

static inline rtems_status_code riscv_raise_on(
  rtems_vector_number vector,
  uint32_t            cpu_index
)
{
  Per_CPU_Control *cpu;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (vector != RISCV_INTERRUPT_VECTOR_SOFTWARE) {
    return RTEMS_UNSATISFIED;
  }

  cpu = _Per_CPU_Get_by_index(cpu_index);
  *cpu->cpu_per_cpu.clint_msip = 0x1;
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_raise(rtems_vector_number vector)
{
  return riscv_raise_on(vector, rtems_scheduler_get_processor());
}

#if defined(RTEMS_SMP)
rtems_status_code bsp_interrupt_raise_on(
  rtems_vector_number vector,
  uint32_t            cpu_index
)
{
  return riscv_raise_on(vector, cpu_index);
}
#endif

rtems_status_code bsp_interrupt_clear(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(enabled != NULL);

  if (RISCV_INTERRUPT_VECTOR_IS_EXTERNAL(vector)) {
    uint32_t interrupt_index;
    uint32_t group;
    uint32_t bit;
    Per_CPU_Control *cpu;
#ifdef RTEMS_SMP
    uint32_t cpu_max;
    uint32_t cpu_index;
#endif

    interrupt_index = RISCV_INTERRUPT_VECTOR_EXTERNAL_TO_INDEX(vector);
    group = interrupt_index / 32;
    bit = UINT32_C(1) << (interrupt_index % 32);

#ifdef RTEMS_SMP
    cpu_max = _SMP_Get_processor_maximum();

    for (cpu_index = 0; cpu_index < cpu_max; ++cpu_index) {
      volatile uint32_t *enable;

      cpu = _Per_CPU_Get_by_index(cpu_index);
      enable = cpu->cpu_per_cpu.plic_m_ie;

      if (enable != NULL && (enable[group] & bit) != 0) {
        *enabled = true;
        return RTEMS_SUCCESSFUL;
      }
    }

    *enabled = false;
#else
    cpu = _Per_CPU_Get_by_index(0);
    *enabled = (cpu->cpu_per_cpu.plic_m_ie[group] & bit) != 0;
#endif

    return RTEMS_SUCCESSFUL;
  }

  if (vector == RISCV_INTERRUPT_VECTOR_TIMER) {
    *enabled = (read_csr(mie) & MIP_MTIP) != 0;
    return RTEMS_SUCCESSFUL;
  }

  _Assert(vector == RISCV_INTERRUPT_VECTOR_SOFTWARE);
  *enabled = (read_csr(mie) & MIP_MSIP) != 0;
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (RISCV_INTERRUPT_VECTOR_IS_EXTERNAL(vector)) {
    uint32_t interrupt_index;
    uint32_t group;
    uint32_t bit;
    rtems_interrupt_lock_context lock_context;
    Per_CPU_Control *cpu;
#ifdef RTEMS_SMP
    volatile uint32_t *enable;
#endif

    interrupt_index = RISCV_INTERRUPT_VECTOR_EXTERNAL_TO_INDEX(vector);
    group = interrupt_index / 32;
    bit = UINT32_C(1) << (interrupt_index % 32);
#ifdef RTEMS_SMP
    enable = riscv_plic_irq_to_cpu[interrupt_index - 1];
#endif

    rtems_interrupt_lock_acquire(&riscv_plic_lock, &lock_context);

#ifdef RTEMS_SMP
    if (enable != NULL) {
      enable[group] |= bit;
    } else {
      uint32_t cpu_max;
      uint32_t cpu_index;

      cpu_max = _SMP_Get_processor_maximum();

      for (cpu_index = 0; cpu_index < cpu_max; ++cpu_index) {
        cpu = _Per_CPU_Get_by_index(cpu_index);
        enable = cpu->cpu_per_cpu.plic_m_ie;

        if (enable != NULL) {
          enable[group] |= bit;
        }
      }
    }
#else
    cpu = _Per_CPU_Get_by_index(0);
    cpu->cpu_per_cpu.plic_m_ie[group] |= bit;
#endif

    rtems_interrupt_lock_release(&riscv_plic_lock, &lock_context);
    return RTEMS_SUCCESSFUL;
  }

  if (vector == RISCV_INTERRUPT_VECTOR_TIMER) {
    set_csr(mie, MIP_MTIP);
    return RTEMS_SUCCESSFUL;
  }

  _Assert(vector == RISCV_INTERRUPT_VECTOR_SOFTWARE);
  set_csr(mie, MIP_MSIP);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (RISCV_INTERRUPT_VECTOR_IS_EXTERNAL(vector)) {
    uint32_t interrupt_index;
    uint32_t group;
    uint32_t bit;
    rtems_interrupt_lock_context lock_context;
    Per_CPU_Control *cpu;
#ifdef RTEMS_SMP
    volatile uint32_t *enable;
#endif

    interrupt_index = RISCV_INTERRUPT_VECTOR_EXTERNAL_TO_INDEX(vector);
    group = interrupt_index / 32;
    bit = UINT32_C(1) << (interrupt_index % 32);
#ifdef RTEMS_SMP
    enable = riscv_plic_irq_to_cpu[interrupt_index - 1];
#endif

    rtems_interrupt_lock_acquire(&riscv_plic_lock, &lock_context);

#ifdef RTEMS_SMP
    if (enable != NULL) {
      enable[group] &= ~bit;
    } else {
      uint32_t cpu_max;
      uint32_t cpu_index;

      cpu_max = _SMP_Get_processor_maximum();

      for (cpu_index = 0; cpu_index < cpu_max; ++cpu_index) {
        cpu = _Per_CPU_Get_by_index(cpu_index);
        enable = cpu->cpu_per_cpu.plic_m_ie;

        if (enable != NULL) {
          enable[group] &= ~bit;
        }
      }
    }
#else
    cpu = _Per_CPU_Get_by_index(0);
    cpu->cpu_per_cpu.plic_m_ie[group] &= ~bit;
#endif

    rtems_interrupt_lock_release(&riscv_plic_lock, &lock_context);
    return RTEMS_SUCCESSFUL;
  }

  if (vector == RISCV_INTERRUPT_VECTOR_TIMER) {
    clear_csr(mie, MIP_MTIP);
    return RTEMS_SUCCESSFUL;
  }

  _Assert(vector == RISCV_INTERRUPT_VECTOR_SOFTWARE);
  clear_csr(mie, MIP_MSIP);
  return RTEMS_SUCCESSFUL;
}

#ifdef RTEMS_SMP
rtems_status_code bsp_interrupt_set_affinity(
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
      return RTEMS_SUCCESSFUL;
    }

    if (_Processor_mask_Count(&mask) == 1) {
      uint32_t cpu_index;
      Per_CPU_Control *cpu;

      cpu_index = _Processor_mask_Find_last_set(&mask) - 1;
      cpu = _Per_CPU_Get_by_index(cpu_index);
      riscv_plic_irq_to_cpu[interrupt_index - 1] = cpu->cpu_per_cpu.plic_m_ie;
      return RTEMS_SUCCESSFUL;
    }

    return RTEMS_INVALID_NUMBER;
  }

  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_get_affinity(
  rtems_vector_number vector,
  Processor_mask *affinity
)
{
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

    return RTEMS_SUCCESSFUL;
  }

  return RTEMS_UNSATISFIED;
}
#endif
