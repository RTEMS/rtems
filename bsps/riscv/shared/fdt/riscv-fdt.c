/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsRISCVShared
 *
 * @brief Helpers to probe the RISC-V ISA extensions. 
 *
 */

/*
 * Copyright (C) 2026 Gedare Bloom
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
#include <bsp/fatal.h>
#include <bsp/riscv-fdt.h>

#include <libfdt.h>
#include <string.h>

void *riscv_fdt_get_end_of_memory( const void *fdt )
{
  const void *val;
  int node;
  int parent;
  int ac;
  int sc;
  int len;
  uintptr_t start;
  uintptr_t size;

  node = fdt_path_offset( fdt, "/memory" );

  if (node < 0) {
    return NULL;
  }

  parent = fdt_parent_offset(fdt, node);
  if (parent < 0) {
    return NULL;
  }

  ac = fdt_address_cells(fdt, parent);
  if (ac != 1 && ac != 2) {
    return NULL;
  }

  sc = fdt_size_cells(fdt, parent);
  if (sc != 1 && sc != 2) {
    return NULL;
  }

  if (sc > ac) {
    return NULL;
  }

  val = fdt_getprop(fdt, node, "reg", &len);
  if (len < sc + ac) {
    return NULL;
  }

  if (ac == 1) {
    start = fdt32_ld(&((fdt32_t *)val)[0]);
    size = fdt32_ld(&((fdt32_t *)val)[1]);
  }

  if (ac == 2) {
    start = fdt64_ld(&((fdt64_t *)val)[0]);
    if (sc == 1) {
      size = fdt32_ld(&((fdt32_t *)(val+8))[0]);
    } else {
      size = fdt64_ld(&((fdt64_t *)val)[1]);
    }
  }

  return (void*) (start + size);
}

uint32_t riscv_clock_get_timebase_frequency(const void *fdt)
{
  int node;
  const fdt32_t *val;
  int len=0;

  node = fdt_path_offset(fdt, "/cpus");

  val = (fdt32_t *) fdt_getprop(fdt, node, "timebase-frequency", &len);

  if (val == NULL || len < 4) {
    int cpu0 = fdt_subnode_offset(fdt, node, "cpu@0");
    val = (fdt32_t *) fdt_getprop(fdt, cpu0, "timebase-frequency", &len);

    if (val == NULL || len < 4) {
      bsp_fatal(RISCV_FATAL_NO_TIMEBASE_FREQUENCY_IN_DEVICE_TREE);
    }
  }
  return fdt32_to_cpu(*val);
}

bool riscv_has_isa_extension(const void *fdt, const char *ext)
{
  int node;
  const char *val;
  int len = 0;

  node = fdt_path_offset(fdt, "/cpus");

  if ( node < 0 ) {
    return false;
  }

  val = (const char *) fdt_getprop(fdt, node, "riscv,isa", &len);

  if (val == NULL || len == 0) {
    int cpu0 = fdt_subnode_offset(fdt, node, "cpu@0");
    val = (const char *) fdt_getprop(fdt, cpu0, "riscv,isa", &len);

    if (val == NULL || len == 0) {
      return false;
    }
  }

  return strstr( val, ext ) != NULL;
}
