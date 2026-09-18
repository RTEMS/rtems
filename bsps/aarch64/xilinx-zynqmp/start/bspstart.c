/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64XilinxZynqMP
 *
 * @brief This source file contains the implementation of bsp_start().
 */

/*
 * Copyright (C) 2020 On-Line Applications Research Corporation (OAR)
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

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/ecc_priv.h>
#include <bsp/irq-generic.h>
#include <bsp/linker-symbols.h>
#include <bsp/aarch64-mmu.h>
#include <bsp/fdt.h>
#include <libfdt.h>

/*
 * Initialize PCIe MMU mapping using the device tree.
 *
 * Example PCIe node from the upstream Linux device tree binding:
 *
 * pcie@fd0e0000 {
 *     compatible = "xlnx,nwl-pcie-2.11";
 *     reg = <0x0 0xfd0e0000 0x0 0x1000>,
 *           <0x0 0xfd480000 0x0 0x1000>,
 *           <0x80 0x00000000 0x0 0x10000000>;
 *     reg-names = "breg", "pcireg", "cfg";
 *     ranges = <0x02000000 0x0 0xe0000000
 *               0x0 0xe0000000
 *               0x0 0x10000000>,
 *              <0x43000000 0x00000006 0x0
 *               0x00000006 0x0
 *               0x00000002 0x0>;
 *     #address-cells = <3>;
 *     #size-cells = <2>;
 *     device_type = "pci";
 * };
 */

static void zynqmp_pcie_default_mappings(void)
{
  aarch64_mmu_map(0xe0000000, 0x10000000, AARCH64_MMU_DEVICE);
  aarch64_mmu_map(0x600000000U, 0x200000000U, AARCH64_MMU_DEVICE);
  aarch64_mmu_map(0x8000000000U, 0x4000000000U, AARCH64_MMU_DEVICE);
}

static void zynqmp_pcie_init(void)
{
  const void *fdt;
  int node;
  const fdt32_t *ranges;
  const fdt32_t *reg;
  int len;
  int reg_len;
  int reg_entry_cells;
  int parent;
  int child_addr_cells;
  int parent_addr_cells;
  int size_cells;
  int entry_cells;
  int cfg_index;
  int cfg_offset;
  uintptr_t cfg_addr;
  uint64_t cfg_size;

  fdt = bsp_fdt_get();
  if (fdt == NULL) {
    return;
  }

  node = fdt_node_offset_by_compatible(
    fdt,
    -1,
    "xlnx,nwl-pcie-2.11"
  );

  if (node < 0) {
    /* Fallback to default static PCIe mappings */
    zynqmp_pcie_default_mappings();
    return;
  }

  parent = fdt_parent_offset(fdt, node);
  if (parent < 0) {
    /* Fallback to default static PCIe mappings */
    zynqmp_pcie_default_mappings();
    return;
  }

  child_addr_cells = fdt_address_cells(fdt, node);
  parent_addr_cells = fdt_address_cells(fdt, parent);
  size_cells = fdt_size_cells(fdt, node);

  if (
    child_addr_cells <= 0 ||
    parent_addr_cells <= 0 ||
    size_cells <= 0 ||
    parent_addr_cells > 2 ||
    size_cells > 2
  ) {
    /* Fallback to default static PCIe mappings */
    zynqmp_pcie_default_mappings();
    return;
  }

  reg_entry_cells = parent_addr_cells + size_cells;
  entry_cells = child_addr_cells + reg_entry_cells;
  ranges = fdt_getprop(fdt, node, "ranges", &len);
  reg = fdt_getprop(fdt, node, "reg", &reg_len);

  if (
    ranges == NULL ||
    len < entry_cells * (int)sizeof(fdt32_t) ||
    len % (entry_cells * (int)sizeof(fdt32_t)) != 0
  ) {
    /* Fallback to default static PCIe mappings */
    zynqmp_pcie_default_mappings();
    return;
  }

  if (
    reg == NULL ||
    reg_len <= 0 ||
    reg_len % (reg_entry_cells * (int)sizeof(fdt32_t)) != 0
  ) {
    /* Fallback to default static PCIe mappings */
    zynqmp_pcie_default_mappings();
    return;
  }

  cfg_index = fdt_stringlist_search(
    fdt,
    node,
    "reg-names",
    "cfg"
  );

  if (
    cfg_index < 0 ||
    cfg_index >= reg_len / (reg_entry_cells * (int)sizeof(fdt32_t))
  ) {
    /* Fallback to default static PCIe mappings */
    zynqmp_pcie_default_mappings();
    return;
  }

  cfg_offset = cfg_index * reg_entry_cells;
  cfg_addr = 0;

  for (int j = 0; j < parent_addr_cells; ++j) {
    cfg_addr = (cfg_addr << 32) | fdt32_to_cpu(reg[cfg_offset + j]);
  }

  cfg_offset += parent_addr_cells;

  cfg_size = 0;
  for (int j = 0; j < size_cells; ++j) {
    cfg_size = (cfg_size << 32) | fdt32_to_cpu(reg[cfg_offset + j]);
  }

  if (cfg_size == 0) {
    /* Fallback to default static PCIe mappings */
    zynqmp_pcie_default_mappings();
    return;
  }

  aarch64_mmu_map(
    cfg_addr,
    cfg_size,
    AARCH64_MMU_DEVICE
  );

  for (int i = 0; i < len / (entry_cells * (int)sizeof(fdt32_t)); ++i) {
    uintptr_t addr;
    uintptr_t size;
    int offset;

    offset = i * entry_cells + child_addr_cells;

    addr = 0;
    for (int j = 0; j < parent_addr_cells; ++j) {
      addr = (addr << 32) | fdt32_to_cpu(ranges[offset + j]);
    }

    offset += parent_addr_cells;

    size = 0;
    for (int j = 0; j < size_cells; ++j) {
      size = (size << 32) | fdt32_to_cpu(ranges[offset + j]);
    }

    aarch64_mmu_map(
      addr,
      size,
      AARCH64_MMU_DEVICE
    );
  }
}

void bsp_start( void )
{
  bsp_interrupt_initialize();
  rtems_cache_coherent_add_area(
    bsp_section_nocacheheap_begin,
    (uintptr_t) bsp_section_nocacheheap_size
  );
  zynqmp_ecc_init();
  zynqmp_pcie_init();
}
