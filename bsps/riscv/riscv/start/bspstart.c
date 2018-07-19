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
#include <bsp/irq-generic.h>
#include <bsp/riscv.h>

#include <libfdt.h>

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

void bsp_start(void)
{
  bsp_interrupt_initialize();
}
