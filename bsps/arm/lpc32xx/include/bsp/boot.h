/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup lpc32xx_boot
 *
 * @brief Boot support API.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_ARM_LPC32XX_BOOT_H
#define LIBBSP_ARM_LPC32XX_BOOT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup lpc32xx_boot Boot Support
 *
 * @ingroup RTEMSBSPsARMLPC32XX
 *
 * @brief Boot support.
 *
 * The NXP internal boot program shall be the "stage-0 program".
 *
 * The boot program within the first page of the first or second block shall be
 * "stage-1 program".  It will be invoked by the stage-0 program from NXP.
 *
 * The program loaded by the stage-1 program will be the "stage-2 program" or the
 * "boot loader".
 *
 * The program loaded by the stage-2 program will be the "stage-3 program" or the
 * "application".
 *
 * The stage-1 program image must have a format specified by NXP.
 *
 * The stage-2 and stage-3 program images may have any format.
 *
 * @{
 */

#define LPC32XX_BOOT_BLOCK_0 0
#define LPC32XX_BOOT_BLOCK_1 1

#define LPC32XX_BOOT_ICR_SP_3AC_8IF 0xf0
#define LPC32XX_BOOT_ICR_SP_4AC_8IF 0xd2
#define LPC32XX_BOOT_ICR_LP_4AC_8IF 0xb4
#define LPC32XX_BOOT_ICR_LP_5AC_8IF 0x96

typedef union {
  struct {
    uint8_t d0;
    uint8_t reserved_0 [3];
    uint8_t d1;
    uint8_t reserved_1 [3];
    uint8_t d2;
    uint8_t reserved_2 [3];
    uint8_t d3;
    uint8_t reserved_3 [3];
    uint8_t d4;
    uint8_t reserved_4 [3];
    uint8_t d5;
    uint8_t reserved_5 [3];
    uint8_t d6;
    uint8_t reserved_6 [3];
    uint8_t d7;
    uint8_t reserved_7 [3];
    uint8_t d8;
    uint8_t reserved_8 [3];
    uint8_t d9;
    uint8_t reserved_9 [3];
    uint8_t d10;
    uint8_t reserved_10 [3];
    uint8_t d11;
    uint8_t reserved_11 [3];
    uint8_t d12;
    uint8_t reserved_12 [463];
  } field;
  uint32_t data [128];
} lpc32xx_boot_block;

void lpc32xx_setup_boot_block(
  lpc32xx_boot_block *boot_block,
  uint8_t icr,
  uint8_t page_count
);

void lpc32xx_set_boot_block_bad(
  lpc32xx_boot_block *boot_block
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC32XX_BOOT_H */
