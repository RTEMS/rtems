/*
 * Copyright (c) 2010, 2016 embedded brains GmbH.  All rights reserved.
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

#include <bsp/u-boot.h>

#include <string.h>
#include <rtems.h>

#ifndef U_BOOT_BOARD_INFO_DATA_SECTION
#define U_BOOT_BOARD_INFO_DATA_SECTION
#endif

U_BOOT_BOARD_INFO_DATA_SECTION bd_t bsp_uboot_board_info;

void bsp_uboot_copy_board_info(const bd_t *src)
{
  bd_t *dst = &bsp_uboot_board_info;

  dst = memcpy(dst, src, sizeof(*dst));
  rtems_cache_flush_multiple_data_lines(dst, sizeof(*dst));
}
