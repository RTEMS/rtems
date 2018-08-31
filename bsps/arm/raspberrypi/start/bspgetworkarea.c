/**
 * @file
 *
 * @ingroup arm_start
 *
 * @brief Raspberry pi workarea initialization.
 */

/*
 * COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * Copyright (c) 2011-2012 embedded brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 *
 * Copyright (c) 2015 YANG Qiao
 *
 * Code is based on c/src/lib/libbsp/shared/bspgetworkarea.c
 */

#include <string.h>
#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/vc.h>

#if defined(HAS_UBOOT) && !defined(BSP_DISABLE_UBOOT_WORK_AREA_CONFIG)
  #define USE_UBOOT
#endif

/*
 *  These are provided by the linkcmds for ALL of the BSPs which use this file.
 */
extern char WorkAreaBase[];

/*
 *  We may get the size information from U-Boot or the linker scripts.
 */
#ifdef USE_UBOOT
  #include <bsp/u-boot.h>
#else
  extern char RamBase[];
  extern char RamSize[];
#endif

void bsp_work_area_initialize(void)
{
  uintptr_t work_base = (uintptr_t) WorkAreaBase;
  uintptr_t ram_end;
  bcm2835_get_vc_memory_entries vc_entry;
  /*
   * bcm2835_get_arm_memory_entries arm_entry;
   * is another alternative how to obtain usable memory size
   */

  #ifdef USE_UBOOT
    ram_end = (uintptr_t) bsp_uboot_board_info.bi_memstart +
                          bsp_uboot_board_info.bi_memsize;
  #else
    ram_end = (uintptr_t)RamBase + (uintptr_t)RamSize;
  #endif

  memset( &vc_entry, 0, sizeof(vc_entry) );
  if (bcm2835_mailbox_get_vc_memory( &vc_entry ) >= 0) {
    if (vc_entry.base > 10 * 1024 *1024)
      ram_end = ram_end > vc_entry.base? vc_entry.base: ram_end;
  }
  bsp_work_area_initialize_default( (void *) work_base, ram_end - work_base );
}
