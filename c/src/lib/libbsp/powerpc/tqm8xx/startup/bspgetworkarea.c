/**
 * @file
 *
 * @ingroup tqm8xx
 *
 * @brief Source for BSP Get Work Area Memory
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

#include <libcpu/powerpc-utility.h>

#include <bsp.h>
#include <bsp/bootcard.h>

void bsp_get_work_area(
  void      **work_area_start,
  uintptr_t  *work_area_size,
  void      **heap_start,
  uintptr_t  *heap_size
)
{
  char *ram_end = (char *) (TQM_BD_INFO.sdram_size - (uint32_t)TopRamReserved);

  *work_area_start = bsp_work_area_start;
  *work_area_size = ram_end - bsp_work_area_start;
  *heap_start = BSP_BOOTCARD_HEAP_USES_WORK_AREA;
  *heap_size = BSP_BOOTCARD_HEAP_SIZE_DEFAULT;
}
