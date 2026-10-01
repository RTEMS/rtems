/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsRISCVShared
 *
 * @brief This source file contains the implementation of
 *   riscv_fdt_irq_initialize().
 */

/*
 * Copyright (C) 2018, 2026 embedded brains GmbH & Co. KG
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

#include <bsp/fatal.h>
#include <bsp/riscv-fdt-irq.h>
#include <bsp/riscv.h>

#include <rtems/score/riscv-utility.h>
#include <rtems/score/smpimpl.h>

#include <libfdt.h>

#ifndef RISCV_USE_S_MODE
volatile RISCV_CLINT_regs *riscv_clint;

uint32_t riscv_fdt_clint_index[RISCV_FDT_PROCESSOR_MAXIMUM];
#endif

volatile RISCV_PLIC_regs *riscv_fdt_plic;

uint32_t riscv_fdt_plic_context[RISCV_FDT_PROCESSOR_MAXIMUM];

uint32_t riscv_fdt_plic_interrupt_count;

#ifdef RISCV_USE_S_MODE
bool riscv_fdt_has_sstc;
#endif

#ifndef RISCV_USE_S_MODE

static void riscv_fdt_find_clint(const void *fdt)
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
    cpu_index = _RISCV_Map_hartid_to_cpu_index(hart_index);
    if (cpu_index >= rtems_configuration_get_maximum_processors()) {
      continue;
    }
#else
    if (hart_index != RISCV_BOOT_HARTID) {
      continue;
    }

    cpu_index = 0;
#endif

    riscv_fdt_clint_index[cpu_index] = (uint32_t) (i / 16);

#ifndef RTEMS_SMP
    break;
#endif
  }
}

#endif

static void riscv_fdt_find_plic(const void *fdt)
{
  volatile RISCV_PLIC_regs *plic;
  int node;
  int i;
  const uint32_t *val;
  int len;
  uint32_t interrupt_index;

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

  riscv_fdt_plic = plic;

  val = fdt_getprop(fdt, node, "riscv,ndev", &len);
  if (val == NULL || len != 4) {
    bsp_fatal(RISCV_FATAL_INVALID_PLIC_NDEV_IN_DEVICE_TREE);
  }

  riscv_fdt_plic_interrupt_count = fdt32_to_cpu(val[0]);
  if (riscv_fdt_plic_interrupt_count == 0) {
    bsp_fatal(RISCV_FATAL_INVALID_PLIC_NDEV_IN_DEVICE_TREE);
  }

  if (riscv_fdt_plic_interrupt_count > RISCV_MAXIMUM_EXTERNAL_INTERRUPTS) {
    bsp_fatal(RISCV_FATAL_TOO_LARGE_PLIC_NDEV_IN_DEVICE_TREE);
  }

  val = fdt_getprop(fdt, node, "interrupts-extended", &len);

  for (i = 0; i < len; i += 8) {
    uint32_t hart_index;
    uint32_t cpu_index;

    hart_index = riscv_get_hart_index_by_phandle(fdt32_to_cpu(val[i / 4]));

#ifdef RTEMS_SMP
    cpu_index = _RISCV_Map_hartid_to_cpu_index(hart_index);
    if (cpu_index >= rtems_configuration_get_maximum_processors()) {
      continue;
    }
#else
    if (hart_index != RISCV_BOOT_HARTID) {
      continue;
    }

    cpu_index = 0;
#endif

    interrupt_index = fdt32_to_cpu(val[i / 4 + 1]);
#ifdef RISCV_USE_S_MODE
    if (interrupt_index != RISCV_INTERRUPT_EXTERNAL_SUPERVISOR) {
      continue;
    }
#else
    if (interrupt_index != RISCV_INTERRUPT_EXTERNAL_MACHINE) {
      continue;
    }
#endif

    riscv_fdt_plic_context[cpu_index] = (uint32_t) (i / 8);

#ifndef RTEMS_SMP
    break;
#endif
  }
}

void riscv_fdt_irq_initialize(void)
{
  const void *fdt;
  uint32_t cpu_index;

  for (cpu_index = 0; cpu_index < RISCV_FDT_PROCESSOR_MAXIMUM; ++cpu_index) {
#ifndef RISCV_USE_S_MODE
    riscv_fdt_clint_index[cpu_index] = UINT32_MAX;
#endif
    riscv_fdt_plic_context[cpu_index] = UINT32_MAX;
  }

  fdt = bsp_fdt_get();

#ifndef RISCV_USE_S_MODE
  riscv_fdt_find_clint(fdt);
#endif

  riscv_fdt_find_plic(fdt);

#ifdef RISCV_USE_S_MODE
  riscv_fdt_has_sstc = riscv_has_isa_extension(fdt, "sstc");
#endif
}
