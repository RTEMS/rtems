/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQ
 *
 * @brief BSP reset.
 */

/*
 * Copyright (C) 2010, 2017 embedded brains GmbH & Co. KG
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

#include <bsp/bootcard.h>
#include <bsp/fdt.h>
#include <bsp/qoriq.h>

#include <libfdt.h>

static int find_rstcr_node(const void *fdt, int node)
{
  return fdt_node_offset_by_prop_value(fdt, node, "fsl,has-rstcr", NULL, 0);
}

void bsp_reset(void)
{
  rtems_interrupt_level level;
  const char *fdt;

  rtems_interrupt_local_disable(level);
  (void) level;

  fdt = bsp_fdt_get();

  /* If we do not find a RSTCR, then loop forever */
  while (true) {
    int node;

    node = -1;

    while ((node = find_rstcr_node(fdt, node)) >= 0) {
      const uint32_t *reg;
      int len;

      reg = fdt_getprop(fdt, node, "reg", &len);
      if (reg != NULL && len >= 4) {
        volatile uint32_t *rstcr;

        rstcr = (volatile uint32_t *)
          ((uintptr_t) &qoriq + fdt32_to_cpu(reg[0]) + 0xb0);
        *rstcr = 0x2;
      }
    }
  }
}
