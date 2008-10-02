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
LINKER_SYMBOL(bsp_ram_start);
LINKER_SYMBOL(bsp_ram_end);
LINKER_SYMBOL(bsp_external_ram_start);
LINKER_SYMBOL(bsp_external_ram_size);
LINKER_SYMBOL(bsp_section_bss_end);

void bsp_get_work_area(
  void    **work_area_start,
  ssize_t  *work_area_size,
  void    **heap_start,
  ssize_t  *heap_size
)
{
  *work_area_start = bsp_section_bss_end;
  *work_area_size = bsp_ram_end - 2 * 
        MPC55XX_INTERRUPT_STACK_SIZE - bsp_section_bss_end;
  *heap_start = bsp_external_ram_start;
  *heap_size = (ssize_t) bsp_external_ram_size;
}
