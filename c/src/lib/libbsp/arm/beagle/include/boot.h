/**
 * @file
 *
 * @ingroup beagle_boot
 *
 * @brief Boot support API.
 */

/*
 * Copyright (c) 2012 Claas Ziemke. All rights reserved.
 *
 *  Claas Ziemke
 *  Kernerstrasse 11
 *  70182 Stuttgart
 *  Germany
 *  <claas.ziemke@gmx.net>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_BEAGLE_BOOT_H
#define LIBBSP_ARM_BEAGLE_BOOT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup beagle_boot Boot Support
 *
 * @ingroup beagle
 *
 * @brief Boot support.
 *
 * The NXP internal boot program shall be the "stage-0 program".
 *
 * The boot program within the first page of the first or second block shall be
 * "stage-1 program".  It will be invoked by the stage-0 program from NXP.
 *
 * The program loaded by the stage-1 program will be the "stage-2 program" or
 * the "boot loader".
 *
 * The program loaded by the stage-2 program will be the "stage-3 program" or
 * the "application".
 *
 * The stage-1 program image must have a format specified by NXP.
 *
 * The stage-2 and stage-3 program images may have any format.
 *
 * @{
 */


#define MLC_SMALL_DATA_SIZE 512
#define MLC_SMALL_DATA_WORD_COUNT (MLC_SMALL_DATA_SIZE / 4)

#define BEAGLE_BOOT_BLOCK_0 0
#define BEAGLE_BOOT_BLOCK_1 1

#define BEAGLE_BOOT_ICR_SP_3AC_8IF 0xf0
#define BEAGLE_BOOT_ICR_SP_4AC_8IF 0xd2
#define BEAGLE_BOOT_ICR_LP_4AC_8IF 0xb4
#define BEAGLE_BOOT_ICR_LP_5AC_8IF 0x96

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
  uint32_t data [MLC_SMALL_DATA_WORD_COUNT];
} beagle_boot_block;

void beagle_setup_boot_block(
  beagle_boot_block *boot_block,
  uint8_t icr,
  uint8_t page_count
);

void beagle_set_boot_block_bad(
  beagle_boot_block *boot_block
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_BEAGLE_BOOT_H */
