/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
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
#include <bsp/fatal.h>
#include <bsp/fdt.h>
#include <bsp/irq-generic.h>
#include <bsp/linker-symbols.h>
#include <dev/clock/arm-generic-timer.h>
#include <libcpu/arm-cp15.h>
#include <arm/freescale/imx/imx6ul_ccmreg.h>

#include <libfdt.h>

#define MAGIC_IRQ_OFFSET 32

void *imx_get_reg_of_node(const void *fdt, int node)
{
  int len;
  const uint32_t *val;

  val = fdt_getprop(fdt, node, "reg", &len);
  if (val == NULL || len < 4) {
    return NULL;
  }

  return (void *) fdt32_to_cpu(val[0]);
}

rtems_vector_number imx_get_irq_of_node(
  const void *fdt,
  int node,
  size_t index
)
{
  int len;
  const uint32_t *val;

  val = fdt_getprop(fdt, node, "interrupts", &len);
  if (val == NULL || len < (int) ((index + 1) * 12)) {
    return BSP_INTERRUPT_VECTOR_INVALID;
  }

  return fdt32_to_cpu(val[index * 3 + 1]) + MAGIC_IRQ_OFFSET;
}

uint32_t bsp_fdt_map_intr(const uint32_t *intr, size_t icells)
{
  return intr[1] + MAGIC_IRQ_OFFSET;
}

static bool imx_is_imx6(const void *fdt)
{
  /*
   * At the moment: Check for some compatible strings that should be there
   * somewhere in every fdt.
   *
   * FIXME: It would be nice if some CPU-ID could be used instead. But I didn't
   * find one.
   */
  int node;

  node = fdt_node_offset_by_compatible(fdt, -1, "fsl,imx6ul");
  if (node >= 0) {
    return true;
  }

  node = fdt_node_offset_by_compatible(fdt, -1, "fsl,imx6ull");
  if (node >= 0) {
    return true;
  }

  return false;
}

#define SYSCNT_CNTCR          (0x0)
#define SYSCNT_CNTCR_ENABLE   (1 << 0)
#define SYSCNT_CNTCR_HDBG     (1 << 1)
#define SYSCNT_CNTCR_FCREQ(n) (1 << (8 + (n)))
#define SYSCNT_CNTFID(n)      (0x20 + 4 * (n))

static uint32_t imx_syscnt_enable_and_return_frequency(const void *fdt)
{
  uint32_t freq;
  volatile void *syscnt_base;

  /* That's not in the usual FDTs. Sorry for falling back to a magic value. */
  if (imx_is_imx6(fdt)) {
    syscnt_base = (void *)0x021dc000;
  } else {
    syscnt_base = (void *)0x306c0000;
  }

  freq = *(uint32_t *)(syscnt_base + SYSCNT_CNTFID(0));

  arm_cp15_set_counter_frequency(freq);

  *(uint32_t *)(syscnt_base + SYSCNT_CNTCR) =
    SYSCNT_CNTCR_ENABLE |
    SYSCNT_CNTCR_HDBG |
    SYSCNT_CNTCR_FCREQ(0);

  return freq;
}

void arm_generic_timer_get_config(
  uint32_t *frequency,
  uint32_t *irq
)
{
  const void *fdt;
  int node;
  int len;
  const uint32_t *val;

  fdt = bsp_fdt_get();
  node = fdt_path_offset(fdt, "/timer");

  val = fdt_getprop(fdt, node, "clock-frequency", &len);
  if (val != NULL && len >= 4) {
    *frequency = fdt32_to_cpu(val[0]);
  } else {
    /*
     * Normally clock-frequency would be provided by the boot loader. If it
     * didn't add one, we have to initialize the system counter ourself.
     */
    *frequency = imx_syscnt_enable_and_return_frequency(fdt);
  }

  /* FIXME: Figure out how Linux gets a proper IRQ number */
  *irq = imx_get_irq_of_node(fdt, node, 0) - 16;
}

uintptr_t imx_gic_dist_base;

static void imx_find_gic(const void *fdt)
{
  int node;

  node = fdt_path_offset(fdt, "/interrupt-controller");
  if (node < 0) {
    node = fdt_path_offset(fdt, "/soc/interrupt-controller");
  }
  imx_gic_dist_base = (uintptr_t) imx_get_reg_of_node(fdt, node);

#if defined(RTEMS_SMP)
  /*
   * Secondary processors start with a disabled data cache and use the GIC to
   * deterine if they can continue the initialization, see
   * arm_gic_irq_initialize_secondary_cpu().
   */
  rtems_cache_flush_multiple_data_lines(
    &imx_gic_dist_base,
    sizeof(imx_gic_dist_base)
  );
#endif
}

static void imx_ccm_enable_eth2_clk(void)
{
  const void *fdt = bsp_fdt_get();

  if (imx_is_imx6(fdt)) {
    int node;
    volatile imx6ul_ccm_analog *ccm_analog = NULL;

    node = fdt_node_offset_by_compatible(fdt, -1, "fsl,imx6ul-anatop");
    if (node >= 0) {
        ccm_analog = imx_get_reg_of_node(fdt, node);
    }
    if (ccm_analog != NULL) {
        ccm_analog->pll_enet_set = IMX6UL_CCM_ANALOG_PLL_ENET_ENET2_125M_EN;
    }
  }
}

void bsp_start(void)
{
  imx_find_gic(bsp_fdt_get());
  bsp_interrupt_initialize();
  rtems_cache_coherent_add_area(
    bsp_section_nocacheheap_begin,
    (uintptr_t) bsp_section_nocacheheap_size
  );
  imx_ccm_enable_eth2_clk();
}
