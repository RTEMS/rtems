/*
 * Copyright (c) 2010-2014 embedded brains GmbH.  All rights reserved.
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

#ifndef U_BOOT_BOARD_INFO_TEXT_SECTION
#define U_BOOT_BOARD_INFO_TEXT_SECTION
#endif

#ifndef U_BOOT_BOARD_INFO_DATA_SECTION
#define U_BOOT_BOARD_INFO_DATA_SECTION
#endif

U_BOOT_BOARD_INFO_DATA_SECTION bd_t bsp_uboot_board_info;

U_BOOT_BOARD_INFO_TEXT_SECTION void bsp_uboot_copy_board_info(const bd_t *src)
{
  const int *s = (const int *) src;
  int *d = (int *) &bsp_uboot_board_info;
  int i = 0;
  int n = sizeof(*src) / sizeof(int);

  for (i = 0; i < n; ++i) {
    d [i] = s [i];
  }
}
