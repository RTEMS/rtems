/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQ
 *
 * @brief BSP reset.
 */

/*
 * Copyright (c) 2010, 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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
