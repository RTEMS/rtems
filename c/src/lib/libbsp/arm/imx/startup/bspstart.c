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

#include <libfdt.h>

uint32_t bsp_fdt_map_intr(const uint32_t *intr, size_t icells)
{
  return intr[1] + 32;
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

  val = fdt_getprop(fdt, node, "interrupts", &len);
  if (val != NULL && len >= 8) {
    /* FIXME: Figure out how Linux gets a proper IRQ number */
    *irq = 16 + fdt32_to_cpu(val[1]);
  } else {
    bsp_fatal(IMX_FATAL_GENERIC_TIMER_IRQ);
  }
}

void bsp_start(void)
{
  bsp_interrupt_initialize();
}
