/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_SHARED_U_BOOT_H
#define LIBBSP_SHARED_U_BOOT_H

#include <bsp/u-boot-config.h>

typedef unsigned long phys_size_t;

#include <bsp/u-boot-board-info.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern bd_t bsp_uboot_board_info;

void bsp_uboot_copy_board_info(const bd_t *src);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_SHARED_U_BOOT_H */
