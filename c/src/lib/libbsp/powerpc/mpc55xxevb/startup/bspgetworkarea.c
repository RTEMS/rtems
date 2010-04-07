/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief BSP Get Work Area of Memory.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <libcpu/powerpc-utility.h>

#define MPC55XX_INTERRUPT_STACK_SIZE 0x1000

/* Symbols defined in linker command file */
LINKER_SYMBOL(bsp_workspace_start);
LINKER_SYMBOL(bsp_workspace_end);
LINKER_SYMBOL(bsp_external_ram_end);

void bsp_get_work_area(
  void      **work_area_start,
  uintptr_t   *work_area_size,
  void      **heap_start,
  uintptr_t  *heap_size
)
{
  size_t free_ram_size;
  *work_area_start = bsp_workspace_start;

  free_ram_size = (uint8_t *)bsp_external_ram_end - (uint8_t *)*work_area_start;
  *work_area_size = (free_ram_size / 2);
  *heap_start = (void *)((uint8_t *)*work_area_start + *work_area_size);
  *heap_size = (free_ram_size / 2);
}
