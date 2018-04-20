/*
 * Copyright (c) 2008-2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/u-boot.h>

#include <libcpu/powerpc-utility.h>

void bsp_restart(void *addr)
{
  rtems_interrupt_level level;
  void (*start)(void) = addr;
  #ifdef HAS_UBOOT
    const void *mem_begin = (const void *) bsp_uboot_board_info.bi_memstart;
    size_t mem_size = bsp_uboot_board_info.bi_memsize;
  #else /* HAS_UBOOT */
    const void *mem_begin = bsp_ram_start;
    size_t mem_size = (size_t) bsp_ram_size;
  #endif /* HAS_UBOOT */
  uint32_t hid0;

  rtems_interrupt_disable(level);
  (void) level; /* avoid set but not used warning */

  hid0 = PPC_SPECIAL_PURPOSE_REGISTER(HID0);

  if ((hid0 & HID0_DCE) != 0) {
    rtems_cache_flush_multiple_data_lines(mem_begin, mem_size);
  }

  hid0 &= ~(HID0_DCE | HID0_ICE);

  PPC_SET_SPECIAL_PURPOSE_REGISTER(HID0, hid0);

  (*start)();
}
