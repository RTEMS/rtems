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
    return UINT32_MAX;
  }

  return fdt32_to_cpu(val[index * 3 + 1]) + MAGIC_IRQ_OFFSET;
}

uint32_t bsp_fdt_map_intr(const uint32_t *intr, size_t icells)
{
  return intr[1] + MAGIC_IRQ_OFFSET;
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
    bsp_fatal(IMX_FATAL_GENERIC_TIMER_FREQUENCY);
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
