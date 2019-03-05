/**
 *  @file
 *
 *  @ingroup shared_uboot
 *
 *  @brief U_BOOT Support
 */

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

#ifndef LIBBSP_SHARED_U_BOOT_H
#define LIBBSP_SHARED_U_BOOT_H

#include <bsp/u-boot-config.h>

#include <sys/types.h>
#include <stdint.h>

#ifdef U_BOOT_64_BIT_PHYS_SIZE
  typedef uint64_t phys_size_t;
#else
  typedef unsigned long phys_size_t;
#endif

#include <bsp/u-boot-board-info.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern bd_t bsp_uboot_board_info;

/**
 *  @defgroup shared_uboot U_BOOT Support
 *
 *  @ingroup RTEMSBSPsShared
 *
 *  @brief U_BOOT Support Package
 */

void bsp_uboot_copy_board_info(const bd_t *src);

/* FIXME: Do not use this function */
void dumpUBootBDInfo(bd_t *u);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_SHARED_U_BOOT_H */
