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
  uintptr_t                      work_base;
  uintptr_t                      ram_end;
  bcm2835_get_board_spec_entries spec = { 0 };

  work_base = (uintptr_t) WorkAreaBase;

  /*
   * Get the board revision and use it to determine the size of the
   * SDRAM. Get the VC memory entry to determine the size of the VC
   * memory needed.
   */

  #ifdef USE_UBOOT
    ram_end = (uintptr_t) bsp_uboot_board_info.bi_memstart +
                          bsp_uboot_board_info.bi_memsize;
  #else
    ram_end = (uintptr_t)RamBase + (uintptr_t)RamSize;
  #endif

  if (bcm2835_mailbox_get_board_revision( &spec ) >= 0) {
    uint32_t mem = (spec.spec >> (4 + 4 + 8 + 4)) & 0xf;
    if (mem < 5) {
      bcm2835_get_vc_memory_entries vc = { 0 };
      const uint32_t rpi_mem[5] = {
        256 * 1024,
        512 * 1024,
        1 * 1024,
        2 * 1024,
        4 * 1024
      };
      ram_end = work_base + rpi_mem[mem];
      if (bcm2835_mailbox_get_vc_memory( &vc ) >= 0)
        ram_end -= vc.size;
    }
  }

  bsp_work_area_initialize_default( (void *) work_base, ram_end - work_base );
}
