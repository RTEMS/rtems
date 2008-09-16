/**
 * @file
 *
 * @ingroup mpc83xx
 *
 * @brief Source for BSP startup code.
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
 *
 * $Id$
 */

#include <bsp.h>
#include <bsp/bootcard.h>

/*
 *  These are provided by the linkcmds for ALL of the BSPs which use this file.
 */
extern char RamBase[];
extern char WorkAreaBase[];
extern char HeapSize[];
extern char RamSize[];

#ifdef HAS_UBOOT
  extern bd_t mpc83xx_uboot_board_info;
#endif /* HAS_UBOOT */

void bsp_get_work_area(
  void   **work_area_start,
  size_t  *work_area_size,
  void   **heap_start,
  size_t  *heap_size)
{
  #ifdef HAS_UBOOT
    char *ram_end = (char *) mpc83xx_uboot_board_info.bi_memstart +
				   mpc83xx_uboot_board_info.bi_memsize;
  #else /* HAS_UBOOT */
    char *ram_end = RamBase + (uintptr_t)RamSize;
  #endif /* HAS_UBOOT */

  *work_area_start = bsp_work_area_start;
  *work_area_size = ram_end - bsp_work_area_start;
  *heap_start = BSP_BOOTCARD_HEAP_USES_WORK_AREA;
  *heap_size = HeapSize;
}
