/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @ingroup shared_uboot
 *
 *  @brief U_BOOT Support
 */

/*
 * Copyright (C) 2010, 2014 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
