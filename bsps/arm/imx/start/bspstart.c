/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/fatal.h>
#include <bsp/fdt.h>
#include <bsp/irq-generic.h>
#include <bsp/linker-symbols.h>
#include <dev/clock/arm-generic-timer.h>
#include <libcpu/arm-cp15.h>

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
}

void bsp_start(void)
{
  imx_find_gic(bsp_fdt_get());
  bsp_interrupt_initialize();
  rtems_cache_coherent_add_area(
    bsp_section_nocacheheap_begin,
    (uintptr_t) bsp_section_nocacheheap_size
  );
}
