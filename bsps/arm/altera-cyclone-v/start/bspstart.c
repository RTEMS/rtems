/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMCycV
 */

/*
 * Copyright (C) 2013, 2018 embedded brains GmbH & Co. KG
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
#include <bsp/irq-generic.h>
#include <bsp/linker-symbols.h>

#include <bsp/alt_clock_manager.h>

#include <libfdt.h>

#ifdef BSP_FDT_IS_SUPPORTED
uint32_t bsp_fdt_map_intr(const uint32_t *intr, size_t icells)
{
  return intr[1] + 32;
}

static void set_clock(
  const void *fdt,
  int parent,
  ALT_CLK_t clk,
  const char *name
)
{
  int node;
  int len;
  const uint32_t *val;

  node = fdt_subnode_offset(fdt, parent, name);
  val = fdt_getprop(fdt, node, "clock-frequency", &len);

  if (val != NULL && len >= 4) {
    alt_clk_ext_clk_freq_set(clk, fdt32_to_cpu(val[0]));
  }
}

static void set_clock_by_output_name(
  const void *fdt,
  ALT_CLK_t clk,
  const char *clock_output_name
)
{
  int node;
  int len;
  const uint32_t *val;

  node = fdt_node_offset_by_prop_value(
    fdt,
    -1,
    "clock-output-names",
    clock_output_name,
    strlen(clock_output_name) + 1
  );
  val = fdt_getprop(fdt, node, "clock-frequency", &len);

  if (val != NULL && len >= 4) {
    alt_clk_ext_clk_freq_set(clk, fdt32_to_cpu(val[0]));
  }
}

static void update_clocks(void)
{
  const void *fdt;
  int parent;

  fdt = bsp_fdt_get();

  /* Try to set by node name */
  parent = fdt_node_offset_by_compatible(fdt, -1, "altr,clk-mgr");
  parent = fdt_subnode_offset(fdt, parent, "clocks");
  set_clock(fdt, parent, ALT_CLK_OSC1, "osc1");
  set_clock(fdt, parent, ALT_CLK_IN_PIN_OSC2, "osc2");
  set_clock(fdt, parent, ALT_CLK_F2H_PERIPH_REF, "f2s_periph_ref_clk");
  set_clock(fdt, parent, ALT_CLK_F2H_SDRAM_REF, "f2s_sdram_ref_clk");

  /* Try to set by "clock-output-names" property value */
  set_clock_by_output_name(fdt, ALT_CLK_OSC1, "hps_0_eosc1-clk");
  set_clock_by_output_name(fdt, ALT_CLK_IN_PIN_OSC2, "hps_0_eosc2-clk");
  set_clock_by_output_name(fdt, ALT_CLK_F2H_PERIPH_REF, "hps_0_f2s_periph_ref_clk-clk");
  set_clock_by_output_name(fdt, ALT_CLK_F2H_SDRAM_REF, "hps_0_f2s_sdram_ref_clk-clk");
}
#endif

#ifdef ALTERA_CYCLONE_V_NEED_A9MPCORE_PERIPHCLK
uint32_t altera_cyclone_v_a9mpcore_periphclk;
#endif

void bsp_start(void)
{
#ifdef BSP_FDT_IS_SUPPORTED
  update_clocks();
#endif
#ifdef ALTERA_CYCLONE_V_NEED_A9MPCORE_PERIPHCLK
  alt_clk_freq_get(ALT_CLK_MPU_PERIPH, &altera_cyclone_v_a9mpcore_periphclk);
#endif
  bsp_interrupt_initialize();
  rtems_cache_coherent_add_area(
    bsp_section_nocacheheap_begin,
    (uintptr_t) bsp_section_nocacheheap_size
  );
}
