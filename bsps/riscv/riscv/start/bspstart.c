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
#include <bsp/fatal.h>
#include <bsp/fdt.h>
#include <bsp/irq-generic.h>
#include <bsp/riscv.h>

#include <libfdt.h>
#include <string.h>

static uint32_t riscv_core_freq;

void *riscv_fdt_get_address(const void *fdt, int node)
{
  int parent;
  int ac;
  int len;
  const uint32_t *reg;
  uint64_t addr;

  parent = fdt_parent_offset(fdt, node);
  if (parent < 0) {
    return NULL;
  }

  ac = fdt_address_cells(fdt, parent);
  if (ac != 1 && ac != 2) {
    return NULL;
  }

  reg = fdt_getprop(fdt, node, "reg", &len);
  if (reg == NULL || len < ac) {
    return NULL;
  }

  addr = 0;

  while (ac > 0) {
    addr = (addr << 32) | fdt32_to_cpu(*reg);
    ++reg;
    --ac;
  }

#if __riscv_xlen < 64
  if (addr > 0xffffffff) {
    return NULL;
  }
#endif

  return (void *)(uintptr_t) addr;
}

#ifdef RTEMS_SMP
uint32_t riscv_hart_count;

static uint32_t riscv_hart_phandles[CPU_MAXIMUM_PROCESSORS];
#else
static uint32_t riscv_hart_phandles[1];
#endif

static void riscv_find_harts(void)
{
  const void *fdt;
  int node;
  uint32_t max_hart_index;

  fdt = bsp_fdt_get();
  max_hart_index = 0;
  node = -1;

  while ((node = fdt_node_offset_by_compatible(fdt, node, "riscv")) >= 0) {
    int subnode;
    const uint32_t *val;
    int len;
    uint32_t phandle;
    uint32_t hart_index;

    val = fdt_getprop(fdt, node, "reg", &len);
    if (val == NULL || len != 4) {
      bsp_fatal(RISCV_FATAL_INVALID_HART_REG_IN_DEVICE_TREE);
    }

    hart_index = fdt32_to_cpu(val[0]);

    if (hart_index >= RTEMS_ARRAY_SIZE(riscv_hart_phandles)) {
      continue;
    }

    if (hart_index > max_hart_index) {
      max_hart_index = hart_index;
    }

    phandle = 0;

    fdt_for_each_subnode(subnode, fdt, node) {
      int propoff;
      bool interrupt_controller;
      uint32_t potential_phandle;

      interrupt_controller = false;
      potential_phandle = 0;

      fdt_for_each_property_offset(propoff, fdt, subnode) {
        const char *name;

        val = fdt_getprop_by_offset(fdt, propoff, &name, &len);
        if (val != NULL) {
          if (strcmp(name, "interrupt-controller") == 0) {
            interrupt_controller = true;
          } else if (len == 4 && strcmp(name, "phandle") == 0) {
            potential_phandle = fdt32_to_cpu(val[0]);
          }
        }
      }

      if (interrupt_controller) {
        phandle = potential_phandle;
        break;
      }
    }

    riscv_hart_phandles[hart_index] = phandle;
  }

#ifdef RTEMS_SMP
  riscv_hart_count = max_hart_index + 1;
#endif
}

uint32_t riscv_get_hart_index_by_phandle(uint32_t phandle)
{
  uint32_t hart_index;

  for (hart_index = 0; hart_index < riscv_hart_count; ++hart_index) {
    if (riscv_hart_phandles[hart_index] == phandle) {
      return hart_index;
    }
  }

  return UINT32_MAX;
}

static uint32_t get_core_frequency(void)
{
#if RISCV_ENABLE_FRDME310ARTY_SUPPORT != 0
  uint32_t node;
  const char *fdt;
  const char *tlclk;
  int len;
  const fdt32_t *val;

  fdt = bsp_fdt_get();
  node = fdt_node_offset_by_compatible(fdt, -1,"fixed-clock");
  tlclk = fdt_getprop(fdt, node, "clock-output-names", &len);

  if (strcmp(tlclk,"tlclk") != 0) {
    bsp_fatal(RISCV_FATAL_NO_TLCLOCK_FREQUENCY_IN_DEVICE_TREE);
  }

  val = fdt_getprop(fdt, node, "clock-frequency", &len);
  if (val != NULL && len == 4) {
    return fdt32_to_cpu(*val);
  }
#endif
  return 0;
}

uint32_t riscv_get_core_frequency(void)
{
  return riscv_core_freq;
}

void bsp_start(void)
{
  riscv_find_harts();
  bsp_interrupt_initialize();
  riscv_core_freq = get_core_frequency();
}
