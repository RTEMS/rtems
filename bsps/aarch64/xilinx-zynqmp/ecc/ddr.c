/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64XilinxZynqMP
 *
 * @brief This source file contains the implementation of DDR ECC support.
 */

/*
 * Copyright (C) 2023 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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

#include <bsp.h>
#include <bsp/ecc_priv.h>
#include <bsp/irq.h>
#include <bsp/utility.h>
#include <rtems/rtems/intr.h>

static uintptr_t ddrc_base = 0xFD070000;

/*
 * The upper value expressable by the bits in a field is sometimes used to
 * indicate different things
 */
#define DDRC_ADDRMAP_4BIT_SPECIAL 15
#define DDRC_ADDRMAP_5BIT_SPECIAL 31

#define DDRC_MSTR_OFFSET 0x0
#define DDRC_MSTR_BURST_RDWR(val) BSP_FLD32(val, 16, 19)
#define DDRC_MSTR_BURST_RDWR_GET(reg) BSP_FLD32GET(reg, 16, 19)
#define DDRC_MSTR_BURST_RDWR_SET(reg, val) BSP_FLD32SET(reg, val, 16, 19)
#define DDRC_MSTR_BURST_RDWR_4 0x2
#define DDRC_MSTR_BURST_RDWR_8 0x4
#define DDRC_MSTR_BURST_RDWR_16 0x8
#define DDRC_MSTR_DATA_BUS_WIDTH(val) BSP_FLD32(val, 12, 13)
#define DDRC_MSTR_DATA_BUS_WIDTH_GET(reg) BSP_FLD32GET(reg, 12, 13)
#define DDRC_MSTR_DATA_BUS_WIDTH_SET(reg, val) BSP_FLD32SET(reg, val, 12, 13)
#define DDRC_MSTR_DATA_BUS_WIDTH_FULL 0x0
#define DDRC_MSTR_DATA_BUS_WIDTH_HALF 0x1
#define DDRC_MSTR_DATA_BUS_WIDTH_QUARTER 0x2
#define DDRC_MSTR_LPDDR4 BSP_BIT32(5)
#define DDRC_MSTR_DDR4 BSP_BIT32(4)
#define DDRC_MSTR_LPDDR3 BSP_BIT32(3)
#define DDRC_MSTR_DDR3 BSP_BIT32(0)

/* Address map definitions, DDR4 variant with full bus width expected */
#define DDRC_ADDRMAP0_OFFSET 0x200
#define DDRC_ADDRMAP0_RANK_B0_BASE 6
#define DDRC_ADDRMAP0_RANK_B0_TARGET_BIT(bw, lp3) 0
#define DDRC_ADDRMAP0_RANK_B0_TARGET rank
#define DDRC_ADDRMAP0_RANK_B0_SPECIAL DDRC_ADDRMAP_5BIT_SPECIAL
#define DDRC_ADDRMAP0_RANK_B0(val) BSP_FLD32(val, 0, 4)
#define DDRC_ADDRMAP0_RANK_B0_GET(reg) BSP_FLD32GET(reg, 0, 4)
#define DDRC_ADDRMAP0_RANK_B0_SET(reg, val) BSP_FLD32SET(reg, val, 0, 4)

#define DDRC_ADDRMAP1_OFFSET 0x204
#define DDRC_ADDRMAP1_BANK_B2_BASE 4
#define DDRC_ADDRMAP1_BANK_B2_TARGET_BIT(bw, lp3) 2
#define DDRC_ADDRMAP1_BANK_B2_TARGET bank
#define DDRC_ADDRMAP1_BANK_B2_SPECIAL DDRC_ADDRMAP_5BIT_SPECIAL
#define DDRC_ADDRMAP1_BANK_B2(val) BSP_FLD32(val, 16, 20)
#define DDRC_ADDRMAP1_BANK_B2_GET(reg) BSP_FLD32GET(reg, 16, 20)
#define DDRC_ADDRMAP1_BANK_B2_SET(reg, val) BSP_FLD32SET(reg, val, 16, 20)
#define DDRC_ADDRMAP1_BANK_B1_BASE 3
#define DDRC_ADDRMAP1_BANK_B1_TARGET_BIT(bw, lp3) 1
#define DDRC_ADDRMAP1_BANK_B1_TARGET bank
#define DDRC_ADDRMAP1_BANK_B1_SPECIAL DDRC_ADDRMAP_5BIT_SPECIAL
#define DDRC_ADDRMAP1_BANK_B1(val) BSP_FLD32(val, 8, 12)
#define DDRC_ADDRMAP1_BANK_B1_GET(reg) BSP_FLD32GET(reg, 8, 12)
#define DDRC_ADDRMAP1_BANK_B1_SET(reg, val) BSP_FLD32SET(reg, val, 8, 12)
#define DDRC_ADDRMAP1_BANK_B0_BASE 2
#define DDRC_ADDRMAP1_BANK_B0_TARGET_BIT(bw, lp3) 0
#define DDRC_ADDRMAP1_BANK_B0_TARGET bank
#define DDRC_ADDRMAP1_BANK_B0_SPECIAL DDRC_ADDRMAP_5BIT_SPECIAL
#define DDRC_ADDRMAP1_BANK_B0(val) BSP_FLD32(val, 0, 4)
#define DDRC_ADDRMAP1_BANK_B0_GET(reg) BSP_FLD32GET(reg, 0, 4)
#define DDRC_ADDRMAP1_BANK_B0_SET(reg, val) BSP_FLD32SET(reg, val, 0, 4)

#define DDRC_ADDRMAP2_OFFSET 0x208
#define DDRC_ADDRMAP2_COL_B5_BASE 5
#define DDRC_ADDRMAP2_COL_B5_TARGET_BIT(bw, lp3) \
  ((bw == DDRC_MSTR_DATA_BUS_WIDTH_FULL) ? \
    5 : ((bw == DDRC_MSTR_DATA_BUS_WIDTH_HALF) ? 6 : 7))
#define DDRC_ADDRMAP2_COL_B5_TARGET column
#define DDRC_ADDRMAP2_COL_B5_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP2_COL_B5(val) BSP_FLD32(val, 24, 27)
#define DDRC_ADDRMAP2_COL_B5_GET(reg) BSP_FLD32GET(reg, 24, 27)
#define DDRC_ADDRMAP2_COL_B5_SET(reg, val) BSP_FLD32SET(reg, val, 24, 27)
#define DDRC_ADDRMAP2_COL_B4_BASE 4
#define DDRC_ADDRMAP2_COL_B4_TARGET_BIT(bw, lp3) \
  ((bw == DDRC_MSTR_DATA_BUS_WIDTH_FULL) ? \
    4 : ((bw == DDRC_MSTR_DATA_BUS_WIDTH_HALF) ? 5 : 6))
#define DDRC_ADDRMAP2_COL_B4_TARGET column
#define DDRC_ADDRMAP2_COL_B4_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP2_COL_B4(val) BSP_FLD32(val, 16, 19)
#define DDRC_ADDRMAP2_COL_B4_GET(reg) BSP_FLD32GET(reg, 16, 19)
#define DDRC_ADDRMAP2_COL_B4_SET(reg, val) BSP_FLD32SET(reg, val, 16, 19)
#define DDRC_ADDRMAP2_COL_B3_BASE 3
#define DDRC_ADDRMAP2_COL_B3_TARGET_BIT(bw, lp3) \
  ((bw == DDRC_MSTR_DATA_BUS_WIDTH_FULL) ? \
    3 : ((bw == DDRC_MSTR_DATA_BUS_WIDTH_HALF) ? 4 : 5 ))
#define DDRC_ADDRMAP2_COL_B3_TARGET column
#define DDRC_ADDRMAP2_COL_B3_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP2_COL_B3(val) BSP_FLD32(val, 8, 11)
#define DDRC_ADDRMAP2_COL_B3_GET(reg) BSP_FLD32GET(reg, 8, 11)
#define DDRC_ADDRMAP2_COL_B3_SET(reg, val) BSP_FLD32SET(reg, val, 8, 11)
#define DDRC_ADDRMAP2_COL_B2_BASE 2
#define DDRC_ADDRMAP2_COL_B2_TARGET_BIT(bw, lp3) \
  ((bw == DDRC_MSTR_DATA_BUS_WIDTH_FULL) ? \
    2 : ((bw == DDRC_MSTR_DATA_BUS_WIDTH_HALF) ? 3 : 4))
#define DDRC_ADDRMAP2_COL_B2_TARGET column
#define DDRC_ADDRMAP2_COL_B2_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP2_COL_B2(val) BSP_FLD32(val, 0, 3)
#define DDRC_ADDRMAP2_COL_B2_GET(reg) BSP_FLD32GET(reg, 0, 3)
#define DDRC_ADDRMAP2_COL_B2_SET(reg, val) BSP_FLD32SET(reg, val, 0, 3)

#define DDRC_ADDRMAP3_OFFSET 0x20c
#define DDRC_ADDRMAP3_COL_B9_BASE 9
static uint32_t map3_col_b9_target_bit(uint32_t bw, bool lp3)
{
  if (bw == DDRC_MSTR_DATA_BUS_WIDTH_FULL) {
    return 9;
  }

  if (bw == DDRC_MSTR_DATA_BUS_WIDTH_QUARTER) {
    return 13;
  }

  if (lp3) {
    return 10;
  }

  return 11;
}
#define DDRC_ADDRMAP3_COL_B9_TARGET_BIT(bw, lp3) \
  map3_col_b9_target_bit(bw, lp3)
#define DDRC_ADDRMAP3_COL_B9_TARGET column
#define DDRC_ADDRMAP3_COL_B9_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP3_COL_B9(val) BSP_FLD32(val, 24, 27)
#define DDRC_ADDRMAP3_COL_B9_GET(reg) BSP_FLD32GET(reg, 24, 27)
#define DDRC_ADDRMAP3_COL_B9_SET(reg, val) BSP_FLD32SET(reg, val, 24, 27)
#define DDRC_ADDRMAP3_COL_B8_BASE 8
#define DDRC_ADDRMAP3_COL_B8_TARGET_BIT(bw, lp3) \
  ((bw == DDRC_MSTR_DATA_BUS_WIDTH_FULL) ? \
    8 : ((bw == DDRC_MSTR_DATA_BUS_WIDTH_HALF) ? 9 : 11))
#define DDRC_ADDRMAP3_COL_B8_TARGET column
#define DDRC_ADDRMAP3_COL_B8_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP3_COL_B8(val) BSP_FLD32(val, 16, 19)
#define DDRC_ADDRMAP3_COL_B8_GET(reg) BSP_FLD32GET(reg, 16, 19)
#define DDRC_ADDRMAP3_COL_B8_SET(reg, val) BSP_FLD32SET(reg, val, 16, 19)
#define DDRC_ADDRMAP3_COL_B7_BASE 7
#define DDRC_ADDRMAP3_COL_B7_TARGET_BIT(bw, lp3) \
  ((bw == DDRC_MSTR_DATA_BUS_WIDTH_FULL) ? \
    7 : ((bw == DDRC_MSTR_DATA_BUS_WIDTH_HALF) ? 8 : 9))
#define DDRC_ADDRMAP3_COL_B7_TARGET column
#define DDRC_ADDRMAP3_COL_B7_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP3_COL_B7(val) BSP_FLD32(val, 8, 11)
#define DDRC_ADDRMAP3_COL_B7_GET(reg) BSP_FLD32GET(reg, 8, 11)
#define DDRC_ADDRMAP3_COL_B7_SET(reg, val) BSP_FLD32SET(reg, val, 8, 11)
#define DDRC_ADDRMAP3_COL_B6_BASE 6
#define DDRC_ADDRMAP3_COL_B6_TARGET_BIT(bw, lp3) \
  ((bw == DDRC_MSTR_DATA_BUS_WIDTH_FULL) ? \
    6 : ((bw == DDRC_MSTR_DATA_BUS_WIDTH_HALF) ? 7 : 8))
#define DDRC_ADDRMAP3_COL_B6_TARGET column
#define DDRC_ADDRMAP3_COL_B6_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP3_COL_B6(val) BSP_FLD32(val, 0, 3)
#define DDRC_ADDRMAP3_COL_B6_GET(reg) BSP_FLD32GET(reg, 0, 3)
#define DDRC_ADDRMAP3_COL_B6_SET(reg, val) BSP_FLD32SET(reg, val, 0, 3)

#define DDRC_ADDRMAP4_OFFSET 0x210
#define DDRC_ADDRMAP4_COL_B11_BASE 11
#define DDRC_ADDRMAP4_COL_B11_TARGET_BIT(bw, lp3) \
  (lp3?11:13)
#define DDRC_ADDRMAP4_COL_B11_TARGET column
#define DDRC_ADDRMAP4_COL_B11_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP4_COL_B11(val) BSP_FLD32(val, 8, 11)
#define DDRC_ADDRMAP4_COL_B11_GET(reg) BSP_FLD32GET(reg, 8, 11)
#define DDRC_ADDRMAP4_COL_B11_SET(reg, val) BSP_FLD32SET(reg, val, 8, 11)
#define DDRC_ADDRMAP4_COL_B10_BASE 10
static uint32_t map4_col_b10_target_bit(uint32_t bw, bool lp3)
{
  if (bw == DDRC_MSTR_DATA_BUS_WIDTH_FULL) {
    if (lp3) {
      return 10;
    }
    return 11;
  }

  /* QUARTER bus mode not valid */
  if (lp3) {
    return 11;
  }

  return 13;
}
#define DDRC_ADDRMAP4_COL_B10_TARGET_BIT(bw, lp3) \
  map4_col_b10_target_bit(bw, lp3)
#define DDRC_ADDRMAP4_COL_B10_TARGET column
#define DDRC_ADDRMAP4_COL_B10_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP4_COL_B10(val) BSP_FLD32(val, 0, 3)
#define DDRC_ADDRMAP4_COL_B10_GET(reg) BSP_FLD32GET(reg, 0, 3)
#define DDRC_ADDRMAP4_COL_B10_SET(reg, val) BSP_FLD32SET(reg, val, 0, 3)

#define DDRC_ADDRMAP5_OFFSET 0x214
#define DDRC_ADDRMAP5_ROW_B11_BASE 17
#define DDRC_ADDRMAP5_ROW_B11_TARGET_BIT(bw, lp3) 11
#define DDRC_ADDRMAP5_ROW_B11_TARGET row
#define DDRC_ADDRMAP5_ROW_B11_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP5_ROW_B11(val) BSP_FLD32(val, 24, 27)
#define DDRC_ADDRMAP5_ROW_B11_GET(reg) BSP_FLD32GET(reg, 24, 27)
#define DDRC_ADDRMAP5_ROW_B11_SET(reg, val) BSP_FLD32SET(reg, val, 24, 27)
/* This gets mapped into ADDRMAP[9,10,11] */
#define DDRC_ADDRMAP5_ROW_B2_10(val) BSP_FLD32(val, 16, 19)
#define DDRC_ADDRMAP5_ROW_B2_10_GET(reg) BSP_FLD32GET(reg, 16, 19)
#define DDRC_ADDRMAP5_ROW_B2_10_SET(reg, val) BSP_FLD32SET(reg, val, 16, 19)
#define DDRC_ADDRMAP5_ROW_B1_BASE 7
#define DDRC_ADDRMAP5_ROW_B1_TARGET_BIT(bw, lp3) 1
#define DDRC_ADDRMAP5_ROW_B1_TARGET row
#define DDRC_ADDRMAP5_ROW_B1_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP5_ROW_B1(val) BSP_FLD32(val, 8, 11)
#define DDRC_ADDRMAP5_ROW_B1_GET(reg) BSP_FLD32GET(reg, 8, 11)
#define DDRC_ADDRMAP5_ROW_B1_SET(reg, val) BSP_FLD32SET(reg, val, 8, 11)
#define DDRC_ADDRMAP5_ROW_B0_BASE 6
#define DDRC_ADDRMAP5_ROW_B0_TARGET_BIT(bw, lp3) 0
#define DDRC_ADDRMAP5_ROW_B0_TARGET row
#define DDRC_ADDRMAP5_ROW_B0_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP5_ROW_B0(val) BSP_FLD32(val, 0, 3)
#define DDRC_ADDRMAP5_ROW_B0_GET(reg) BSP_FLD32GET(reg, 0, 3)
#define DDRC_ADDRMAP5_ROW_B0_SET(reg, val) BSP_FLD32SET(reg, val, 0, 3)

#define DDRC_ADDRMAP6_OFFSET 0x218
#define DDRC_ADDRMAP6_LPDDR3_6_12 BSP_BIT(bw, lp3)32(31)
#define DDRC_ADDRMAP6_ROW_B15_BASE 21
#define DDRC_ADDRMAP6_ROW_B15_TARGET_BIT(bw, lp3) 15
#define DDRC_ADDRMAP6_ROW_B15_TARGET row
#define DDRC_ADDRMAP6_ROW_B15_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP6_ROW_B15(val) BSP_FLD32(val, 24, 27)
#define DDRC_ADDRMAP6_ROW_B15_GET(reg) BSP_FLD32GET(reg, 24, 27)
#define DDRC_ADDRMAP6_ROW_B15_SET(reg, val) BSP_FLD32SET(reg, val, 24, 27)
#define DDRC_ADDRMAP6_ROW_B14_BASE 20
#define DDRC_ADDRMAP6_ROW_B14_TARGET_BIT(bw, lp3) 14
#define DDRC_ADDRMAP6_ROW_B14_TARGET row
#define DDRC_ADDRMAP6_ROW_B14_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP6_ROW_B14(val) BSP_FLD32(val, 16, 19)
#define DDRC_ADDRMAP6_ROW_B14_GET(reg) BSP_FLD32GET(reg, 16, 19)
#define DDRC_ADDRMAP6_ROW_B14_SET(reg, val) BSP_FLD32SET(reg, val, 16, 19)
#define DDRC_ADDRMAP6_ROW_B13_BASE 19
#define DDRC_ADDRMAP6_ROW_B13_TARGET_BIT(bw, lp3) 13
#define DDRC_ADDRMAP6_ROW_B13_TARGET row
#define DDRC_ADDRMAP6_ROW_B13_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP6_ROW_B13(val) BSP_FLD32(val, 8, 11)
#define DDRC_ADDRMAP6_ROW_B13_GET(reg) BSP_FLD32GET(reg, 8, 11)
#define DDRC_ADDRMAP6_ROW_B13_SET(reg, val) BSP_FLD32SET(reg, val, 8, 11)
#define DDRC_ADDRMAP6_ROW_B12_BASE 18
#define DDRC_ADDRMAP6_ROW_B12_TARGET_BIT(bw, lp3) 12
#define DDRC_ADDRMAP6_ROW_B12_TARGET row
#define DDRC_ADDRMAP6_ROW_B12_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP6_ROW_B12(val) BSP_FLD32(val, 0, 3)
#define DDRC_ADDRMAP6_ROW_B12_GET(reg) BSP_FLD32GET(reg, 0, 3)
#define DDRC_ADDRMAP6_ROW_B12_SET(reg, val) BSP_FLD32SET(reg, val, 0, 3)

#define DDRC_ADDRMAP7_OFFSET 0x21c
#define DDRC_ADDRMAP7_ROW_B17_BASE 23
#define DDRC_ADDRMAP7_ROW_B17_TARGET_BIT(bw, lp3) 17
#define DDRC_ADDRMAP7_ROW_B17_TARGET row
#define DDRC_ADDRMAP7_ROW_B17_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP7_ROW_B17(val) BSP_FLD32(val, 8, 11)
#define DDRC_ADDRMAP7_ROW_B17_GET(reg) BSP_FLD32GET(reg, 8, 11)
#define DDRC_ADDRMAP7_ROW_B17_SET(reg, val) BSP_FLD32SET(reg, val, 8, 11)
#define DDRC_ADDRMAP7_ROW_B16_BASE 22
#define DDRC_ADDRMAP7_ROW_B16_TARGET_BIT(bw, lp3) 16
#define DDRC_ADDRMAP7_ROW_B16_TARGET row
#define DDRC_ADDRMAP7_ROW_B16_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP7_ROW_B16(val) BSP_FLD32(val, 0, 3)
#define DDRC_ADDRMAP7_ROW_B16_GET(reg) BSP_FLD32GET(reg, 0, 3)
#define DDRC_ADDRMAP7_ROW_B16_SET(reg, val) BSP_FLD32SET(reg, val, 0, 3)

#define DDRC_ADDRMAP8_OFFSET 0x220
#define DDRC_ADDRMAP8_BG_B1_BASE 3
#define DDRC_ADDRMAP8_BG_B1_TARGET_BIT(bw, lp3) 1
#define DDRC_ADDRMAP8_BG_B1_TARGET bank_group
#define DDRC_ADDRMAP8_BG_B1_SPECIAL DDRC_ADDRMAP_5BIT_SPECIAL
#define DDRC_ADDRMAP8_BG_B1(val) BSP_FLD32(val, 8, 12)
#define DDRC_ADDRMAP8_BG_B1_GET(reg) BSP_FLD32GET(reg, 8, 12)
#define DDRC_ADDRMAP8_BG_B1_SET(reg, val) BSP_FLD32SET(reg, val, 8, 12)
#define DDRC_ADDRMAP8_BG_B0_BASE 2
#define DDRC_ADDRMAP8_BG_B0_TARGET_BIT(bw, lp3) 0
#define DDRC_ADDRMAP8_BG_B0_TARGET bank_group
#define DDRC_ADDRMAP8_BG_B0_SPECIAL DDRC_ADDRMAP_5BIT_SPECIAL
#define DDRC_ADDRMAP8_BG_B0(val) BSP_FLD32(val, 0, 4)
#define DDRC_ADDRMAP8_BG_B0_GET(reg) BSP_FLD32GET(reg, 0, 4)
#define DDRC_ADDRMAP8_BG_B0_SET(reg, val) BSP_FLD32SET(reg, val, 0, 4)

#define DDRC_ADDRMAP9_OFFSET 0x224
#define DDRC_ADDRMAP9_ROW_B5_BASE 11
#define DDRC_ADDRMAP9_ROW_B5_TARGET_BIT(bw, lp3) 5
#define DDRC_ADDRMAP9_ROW_B5_TARGET row
#define DDRC_ADDRMAP9_ROW_B5_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP9_ROW_B5(val) BSP_FLD32(val, 24, 27)
#define DDRC_ADDRMAP9_ROW_B5_GET(reg) BSP_FLD32GET(reg, 24, 27)
#define DDRC_ADDRMAP9_ROW_B5_SET(reg, val) BSP_FLD32SET(reg, val, 24, 27)
#define DDRC_ADDRMAP9_ROW_B4_BASE 10
#define DDRC_ADDRMAP9_ROW_B4_TARGET_BIT(bw, lp3) 4
#define DDRC_ADDRMAP9_ROW_B4_TARGET row
#define DDRC_ADDRMAP9_ROW_B4_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP9_ROW_B4(val) BSP_FLD32(val, 16, 19)
#define DDRC_ADDRMAP9_ROW_B4_GET(reg) BSP_FLD32GET(reg, 16, 19)
#define DDRC_ADDRMAP9_ROW_B4_SET(reg, val) BSP_FLD32SET(reg, val, 16, 19)
#define DDRC_ADDRMAP9_ROW_B3_BASE 9
#define DDRC_ADDRMAP9_ROW_B3_TARGET_BIT(bw, lp3) 3
#define DDRC_ADDRMAP9_ROW_B3_TARGET row
#define DDRC_ADDRMAP9_ROW_B3_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP9_ROW_B3(val) BSP_FLD32(val, 8, 11)
#define DDRC_ADDRMAP9_ROW_B3_GET(reg) BSP_FLD32GET(reg, 8, 11)
#define DDRC_ADDRMAP9_ROW_B3_SET(reg, val) BSP_FLD32SET(reg, val, 8, 11)
#define DDRC_ADDRMAP9_ROW_B2_BASE 8
#define DDRC_ADDRMAP9_ROW_B2_TARGET_BIT(bw, lp3) 2
#define DDRC_ADDRMAP9_ROW_B2_TARGET row
#define DDRC_ADDRMAP9_ROW_B2_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP9_ROW_B2(val) BSP_FLD32(val, 0, 3)
#define DDRC_ADDRMAP9_ROW_B2_GET(reg) BSP_FLD32GET(reg, 0, 3)
#define DDRC_ADDRMAP9_ROW_B2_SET(reg, val) BSP_FLD32SET(reg, val, 0, 3)

#define DDRC_ADDRMAP10_OFFSET 0x228
#define DDRC_ADDRMAP10_ROW_B9_BASE 15
#define DDRC_ADDRMAP10_ROW_B9_TARGET_BIT(bw, lp3) 9
#define DDRC_ADDRMAP10_ROW_B9_TARGET row
#define DDRC_ADDRMAP10_ROW_B9_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP10_ROW_B9(val) BSP_FLD32(val, 24, 27)
#define DDRC_ADDRMAP10_ROW_B9_GET(reg) BSP_FLD32GET(reg, 24, 27)
#define DDRC_ADDRMAP10_ROW_B9_SET(reg, val) BSP_FLD32SET(reg, val, 24, 27)
#define DDRC_ADDRMAP10_ROW_B8_BASE 14
#define DDRC_ADDRMAP10_ROW_B8_TARGET_BIT(bw, lp3) 8
#define DDRC_ADDRMAP10_ROW_B8_TARGET row
#define DDRC_ADDRMAP10_ROW_B8_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP10_ROW_B8(val) BSP_FLD32(val, 16, 19)
#define DDRC_ADDRMAP10_ROW_B8_GET(reg) BSP_FLD32GET(reg, 16, 19)
#define DDRC_ADDRMAP10_ROW_B8_SET(reg, val) BSP_FLD32SET(reg, val, 16, 19)
#define DDRC_ADDRMAP10_ROW_B7_BASE 13
#define DDRC_ADDRMAP10_ROW_B7_TARGET_BIT(bw, lp3) 7
#define DDRC_ADDRMAP10_ROW_B7_TARGET row
#define DDRC_ADDRMAP10_ROW_B7_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP10_ROW_B7(val) BSP_FLD32(val, 8, 11)
#define DDRC_ADDRMAP10_ROW_B7_GET(reg) BSP_FLD32GET(reg, 8, 11)
#define DDRC_ADDRMAP10_ROW_B7_SET(reg, val) BSP_FLD32SET(reg, val, 8, 11)
#define DDRC_ADDRMAP10_ROW_B6_BASE 12
#define DDRC_ADDRMAP10_ROW_B6_TARGET_BIT(bw, lp3) 6
#define DDRC_ADDRMAP10_ROW_B6_TARGET row
#define DDRC_ADDRMAP10_ROW_B6_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP10_ROW_B6(val) BSP_FLD32(val, 0, 3)
#define DDRC_ADDRMAP10_ROW_B6_GET(reg) BSP_FLD32GET(reg, 0, 3)
#define DDRC_ADDRMAP10_ROW_B6_SET(reg, val) BSP_FLD32SET(reg, val, 0, 3)

#define DDRC_ADDRMAP11_OFFSET 0x22c
#define DDRC_ADDRMAP11_ROW_B10_BASE 16
#define DDRC_ADDRMAP11_ROW_B10_TARGET_BIT(bw, lp3) 10
#define DDRC_ADDRMAP11_ROW_B10_TARGET row
#define DDRC_ADDRMAP11_ROW_B10_SPECIAL DDRC_ADDRMAP_4BIT_SPECIAL
#define DDRC_ADDRMAP11_ROW_B10(val) BSP_FLD32(val, 0, 3)
#define DDRC_ADDRMAP11_ROW_B10_GET(reg) BSP_FLD32GET(reg, 0, 3)
#define DDRC_ADDRMAP11_ROW_B10_SET(reg, val) BSP_FLD32SET(reg, val, 0, 3)

#define DDRC_ECCPOISONADDR0_OFFSET 0xB8
#define DDRC_ECCPOISONADDR0_RANK BSP_BIT32(24)
#define DDRC_ECCPOISONADDR0_COL(val) BSP_FLD32(val, 0, 11)
#define DDRC_ECCPOISONADDR0_COL_GET(reg) BSP_FLD32GET(reg, 0, 11)
#define DDRC_ECCPOISONADDR0_COL_SET(reg, val) BSP_FLD32SET(reg, val, 0, 11)

#define DDRC_ECCPOISONADDR1_OFFSET 0xBC
#define DDRC_ECCPOISONADDR1_BG(val) BSP_FLD32(val, 28, 29)
#define DDRC_ECCPOISONADDR1_BG_GET(reg) BSP_FLD32GET(reg, 28, 29)
#define DDRC_ECCPOISONADDR1_BG_SET(reg, val) BSP_FLD32SET(reg, val, 28, 29)
#define DDRC_ECCPOISONADDR1_BANK(val) BSP_FLD32(val, 24, 26)
#define DDRC_ECCPOISONADDR1_BANK_GET(reg) BSP_FLD32GET(reg, 24, 26)
#define DDRC_ECCPOISONADDR1_BANK_SET(reg, val) BSP_FLD32SET(reg, val, 24, 26)
#define DDRC_ECCPOISONADDR1_ROW(val) BSP_FLD32(val, 0, 17)
#define DDRC_ECCPOISONADDR1_ROW_GET(reg) BSP_FLD32GET(reg, 0, 17)
#define DDRC_ECCPOISONADDR1_ROW_SET(reg, val) BSP_FLD32SET(reg, val, 0, 17)

static void homogenize_row(
  uint32_t *addrmap5,
  uint32_t *addrmap9,
  uint32_t *addrmap10,
  uint32_t *addrmap11
)
{
  uint32_t b2_10 = DDRC_ADDRMAP5_ROW_B2_10_GET(*addrmap5);
  if (b2_10 == DDRC_ADDRMAP_4BIT_SPECIAL) {
    /* ADDRMAP[9,10,11] already define row[2:10] correctly */
    return;
  }

  /* Translate b2_10 to ADDRMAP[9,10,11] to simplify future code */
  *addrmap9 = DDRC_ADDRMAP9_ROW_B5_SET(*addrmap9, b2_10);
  *addrmap9 = DDRC_ADDRMAP9_ROW_B4_SET(*addrmap9, b2_10);
  *addrmap9 = DDRC_ADDRMAP9_ROW_B3_SET(*addrmap9, b2_10);
  *addrmap9 = DDRC_ADDRMAP9_ROW_B2_SET(*addrmap9, b2_10);

  *addrmap10 = DDRC_ADDRMAP10_ROW_B9_SET(*addrmap10, b2_10);
  *addrmap10 = DDRC_ADDRMAP10_ROW_B8_SET(*addrmap10, b2_10);
  *addrmap10 = DDRC_ADDRMAP10_ROW_B7_SET(*addrmap10, b2_10);
  *addrmap10 = DDRC_ADDRMAP10_ROW_B6_SET(*addrmap10, b2_10);

  *addrmap11 = DDRC_ADDRMAP11_ROW_B10_SET(*addrmap11, b2_10);
}

#define DDRC_READ(offset) (*(uint32_t *)(ddrc_base + offset))

#define DDRC_MAP_BIT(value, source, target) ((value >> source) & 0x1) << target

#define DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, BIT_ID, info, addrmap) \
({ \
  uint32_t mapbit = DDRC_ ## BIT_ID ## _GET(addrmap); \
  uint32_t target_bit = DDRC_ ## BIT_ID ## _TARGET_BIT(bus_width, lpddr3); \
  if (mapbit != DDRC_ ## BIT_ID ## _SPECIAL) { \
    mapbit += DDRC_ ## BIT_ID ## _BASE; \
    /* account for AXI -> HIF shift */ \
    mapbit += 3; \
    info->address |= \
      DDRC_MAP_BIT(info-> DDRC_ ## BIT_ID ## _TARGET, target_bit, mapbit); \
  } \
})

/*
 * Steps in mapping an address:
 * system address -> DDRC -> AXI byte address:
 *   disjoint memory regions are mapped into a monolithic block representing
 *     total available RAM based on configured offsets
 * AXI byte address -> XPI -> HIF word address:
 *   word-sized shift of 3 bits for 8 byte (word) alignment
 * HIF word address -> DDRC -> SDRAM address:
 *   addresses are mapped into SDRAM terms by the flexible address mapper using
 *     the ADDRMAP* registers
 */
static int compose_address(DDR_Error_Info *info)
{
  uint32_t addrmap0 = DDRC_READ(DDRC_ADDRMAP0_OFFSET);
  uint32_t addrmap1 = DDRC_READ(DDRC_ADDRMAP1_OFFSET);
  uint32_t addrmap2 = DDRC_READ(DDRC_ADDRMAP2_OFFSET);
  uint32_t addrmap3 = DDRC_READ(DDRC_ADDRMAP3_OFFSET);
  uint32_t addrmap4 = DDRC_READ(DDRC_ADDRMAP4_OFFSET);
  uint32_t addrmap5 = DDRC_READ(DDRC_ADDRMAP5_OFFSET);
  uint32_t addrmap6 = DDRC_READ(DDRC_ADDRMAP6_OFFSET);
  uint32_t addrmap7 = DDRC_READ(DDRC_ADDRMAP7_OFFSET);
  uint32_t addrmap8 = DDRC_READ(DDRC_ADDRMAP8_OFFSET);
  uint32_t addrmap9 = DDRC_READ(DDRC_ADDRMAP9_OFFSET);
  uint32_t addrmap10 = DDRC_READ(DDRC_ADDRMAP10_OFFSET);
  uint32_t addrmap11 = DDRC_READ(DDRC_ADDRMAP11_OFFSET);
  uint32_t mstr = DDRC_READ(DDRC_MSTR_OFFSET);
  uint32_t bus_width = DDRC_MSTR_DATA_BUS_WIDTH_GET(mstr);
  bool lpddr3 = mstr & DDRC_MSTR_LPDDR3;

  homogenize_row(&addrmap5, &addrmap9, &addrmap10, &addrmap11);
  
  /* Clear items that will be written to */
  info->address = 0;

  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP0_RANK_B0, info, addrmap0);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP1_BANK_B2, info, addrmap1);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP1_BANK_B1, info, addrmap1);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP1_BANK_B0, info, addrmap1);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP2_COL_B5, info, addrmap2);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP2_COL_B4, info, addrmap2);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP2_COL_B3, info, addrmap2);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP2_COL_B2, info, addrmap2);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP3_COL_B9, info, addrmap3);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP3_COL_B8, info, addrmap3);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP3_COL_B7, info, addrmap3);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP3_COL_B6, info, addrmap3);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP4_COL_B11, info, addrmap4);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP4_COL_B10, info, addrmap4);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP5_ROW_B11, info, addrmap5);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP5_ROW_B1, info, addrmap5);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP5_ROW_B0, info, addrmap5);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP6_ROW_B15, info, addrmap6);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP6_ROW_B14, info, addrmap6);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP6_ROW_B13, info, addrmap6);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP6_ROW_B12, info, addrmap6);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP7_ROW_B17, info, addrmap7);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP7_ROW_B16, info, addrmap7);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP8_BG_B1, info, addrmap8);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP8_BG_B0, info, addrmap8);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP9_ROW_B5, info, addrmap9);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP9_ROW_B4, info, addrmap9);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP9_ROW_B3, info, addrmap9);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP9_ROW_B2, info, addrmap9);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP10_ROW_B9, info, addrmap10);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP10_ROW_B8, info, addrmap10);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP10_ROW_B7, info, addrmap10);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP10_ROW_B6, info, addrmap10);
  DDRC_CHECK_AND_UNMAP(bus_width, lpddr3, ADDRMAP11_ROW_B10, info, addrmap11);

  /* Column[0:1] are always statically mapped to HIF[0:1] */
  info->address |= (info->column & 0x3) << 3;
  return 0;
}

static uintptr_t ddr_qos_ctrl_base = 0xFD090000;
/* DDR QoS CTRL QoS IRQ Status */
#define DDR_QIS_OFFSET 0x200
#define DDR_QIE_OFFSET 0x208
#define DDR_QID_OFFSET 0x20c
#define DDR_QI_UNCRERR BSP_BIT32(2)
#define DDR_QI_CORERR BSP_BIT32(1)

#define DDRC_ECCSTAT_OFFSET 0x78
#define DDRC_ECCSTAT_UNCR_ERR(val) BSP_FLD32(val, 16, 19)
#define DDRC_ECCSTAT_UNCR_ERR_GET(reg) BSP_FLD32GET(reg, 16, 19)
#define DDRC_ECCSTAT_UNCR_ERR_SET(reg, val) BSP_FLD32SET(reg, val, 16, 19)
#define DDRC_ECCSTAT_CORR_ERR(val) BSP_FLD32(val, 8, 11)
#define DDRC_ECCSTAT_CORR_ERR_GET(reg) BSP_FLD32GET(reg, 8, 11)
#define DDRC_ECCSTAT_CORR_ERR_SET(reg, val) BSP_FLD32SET(reg, val, 8, 11)
#define DDRC_ECCSTAT_CORR_BIT(val) BSP_FLD32(val, 0, 6)
#define DDRC_ECCSTAT_CORR_BIT_GET(reg) BSP_FLD32GET(reg, 0, 6)
#define DDRC_ECCSTAT_CORR_BIT_SET(reg, val) BSP_FLD32SET(reg, val, 0, 6)

/* Correctable and uncorrectable error address registers share encodings */
#define DDRC_ECCCADDR0_OFFSET 0x84
#define DDRC_ECCUADDR0_OFFSET 0xA4
#define DDRC_ECCXADDR0_RANK BSP_BIT32(24)
#define DDRC_ECCXADDR0_ROW(val) BSP_FLD32(val, 0, 17)
#define DDRC_ECCXADDR0_ROW_GET(reg) BSP_FLD32GET(reg, 0, 17)
#define DDRC_ECCXADDR0_ROW_SET(reg, val) BSP_FLD32SET(reg, val, 0, 17)

#define DDRC_ECCCADDR1_OFFSET 0x88
#define DDRC_ECCUADDR1_OFFSET 0xA8
#define DDRC_ECCXADDR1_BG(val) BSP_FLD32(val, 24, 25)
#define DDRC_ECCXADDR1_BG_GET(reg) BSP_FLD32GET(reg, 24, 25)
#define DDRC_ECCXADDR1_BG_SET(reg, val) BSP_FLD32SET(reg, val, 24, 25)
#define DDRC_ECCXADDR1_BANK(val) BSP_FLD32(val, 16, 18)
#define DDRC_ECCXADDR1_BANK_GET(reg) BSP_FLD32GET(reg, 16, 18)
#define DDRC_ECCXADDR1_BANK_SET(reg, val) BSP_FLD32SET(reg, val, 16, 18)
#define DDRC_ECCXADDR1_COL(val) BSP_FLD32(val, 0, 11)
#define DDRC_ECCXADDR1_COL_GET(reg) BSP_FLD32GET(reg, 0, 11)
#define DDRC_ECCXADDR1_COL_SET(reg, val) BSP_FLD32SET(reg, val, 0, 11)

static void extract_ddr_info(
  DDR_Error_Info *info,
  uint32_t addr0_val,
  uint32_t addr1_val
)
{
  info->rank = (addr0_val & DDRC_ECCXADDR0_RANK) >> 24;
  info->bank_group = DDRC_ECCXADDR1_BG_GET(addr1_val);
  info->bank = DDRC_ECCXADDR1_BANK_GET(addr1_val);
  info->row = DDRC_ECCXADDR0_ROW_GET(addr0_val);
  info->column = DDRC_ECCXADDR1_COL_GET(addr1_val);
  compose_address(info);
}


static void ddr_handler(void *arg)
{
  (void) arg;

  volatile uint32_t *qis = (uint32_t *)(ddr_qos_ctrl_base + DDR_QIS_OFFSET);
  uint32_t qis_value = *qis;
  DDR_Error_Info info;
  volatile uint32_t *addr0 = (uint32_t *)(ddrc_base + DDRC_ECCCADDR0_OFFSET);
  volatile uint32_t *addr1 = (uint32_t *)(ddrc_base + DDRC_ECCCADDR1_OFFSET);

  /* specific data is captured in DDRC.ECCSTAT[corrected_bit_num] */
  if ((qis_value & DDR_QI_CORERR) != 0) {
    /* Clear status flag */
    *qis = DDR_QI_CORERR;

    info.type = DDR_CORRECTABLE;
    extract_ddr_info(&info, *addr0, *addr1);
    zynqmp_invoke_ecc_handler(DDR_RAM, &info);
  }
  if ((qis_value & DDR_QI_UNCRERR) != 0) {
    /* Clear status flag */
    *qis = DDR_QI_UNCRERR;

    info.type = DDR_UNCORRECTABLE;
    extract_ddr_info(&info, *addr0, *addr1);
    zynqmp_invoke_ecc_handler(DDR_RAM, &info);
  }
}

static rtems_interrupt_entry zynqmp_ddr_ecc_entry;

rtems_status_code zynqmp_configure_ddr_ecc( void )
{
  volatile uint32_t *qie = (uint32_t *)(ddr_qos_ctrl_base + DDR_QIE_OFFSET);
  rtems_status_code sc;

  rtems_interrupt_entry_initialize(
    &zynqmp_ddr_ecc_entry,
    ddr_handler,
    NULL,
    "DDR RAM ECC"
  );

  sc = rtems_interrupt_entry_install(
    ZYNQMP_IRQ_DDR,
    RTEMS_INTERRUPT_SHARED,
    &zynqmp_ddr_ecc_entry
  );

  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  /* enable interrupts for ECC in QOS_IRQ_ENABLE */
  *qie |= DDR_QI_UNCRERR | DDR_QI_CORERR;
  return RTEMS_SUCCESSFUL;
}

#if RUNNING_FROM_OCM_IS_NOT_CURRENTLY_POSSIBLE
/*
 * Injecting DDR ECC faults requires RTEMS to run from OCM since the DDR will be
 * partially disabled and re-enabled during the process. RTEMS is too large to
 * run out of OCM in its current configuration and doing so would require
 * operation without MMU along with other changes to reduce the memory footprint
 * to below 256KB. It must be the whole RTEMS executable since stack accesses
 * would also present a problem.
 */

#define DDRC_PRINT_MAP(BIT_ID, addrmap) \
({ \
  uint32_t mapbit = DDRC_ ## BIT_ID ## _GET(addrmap); \
  if (mapbit != DDRC_ ## BIT_ID ## _SPECIAL) { \
    mapbit += DDRC_ ## BIT_ID ## _BASE; \
  } \
})

static void print_addr_maps( void )
{
  uint32_t addrmap0 = DDRC_READ(DDRC_ADDRMAP0_OFFSET);
  uint32_t addrmap1 = DDRC_READ(DDRC_ADDRMAP1_OFFSET);
  uint32_t addrmap2 = DDRC_READ(DDRC_ADDRMAP2_OFFSET);
  uint32_t addrmap3 = DDRC_READ(DDRC_ADDRMAP3_OFFSET);
  uint32_t addrmap4 = DDRC_READ(DDRC_ADDRMAP4_OFFSET);
  uint32_t addrmap5 = DDRC_READ(DDRC_ADDRMAP5_OFFSET);
  uint32_t addrmap6 = DDRC_READ(DDRC_ADDRMAP6_OFFSET);
  uint32_t addrmap7 = DDRC_READ(DDRC_ADDRMAP7_OFFSET);
  uint32_t addrmap8 = DDRC_READ(DDRC_ADDRMAP8_OFFSET);
  uint32_t addrmap9 = DDRC_READ(DDRC_ADDRMAP9_OFFSET);
  uint32_t addrmap10 = DDRC_READ(DDRC_ADDRMAP10_OFFSET);
  uint32_t addrmap11 = DDRC_READ(DDRC_ADDRMAP11_OFFSET);

  homogenize_row(&addrmap5, &addrmap9, &addrmap10, &addrmap11);

  DDRC_PRINT_MAP(ADDRMAP0_RANK_B0, addrmap0);
  DDRC_PRINT_MAP(ADDRMAP1_BANK_B2, addrmap1);
  DDRC_PRINT_MAP(ADDRMAP1_BANK_B1, addrmap1);
  DDRC_PRINT_MAP(ADDRMAP1_BANK_B0, addrmap1);
  DDRC_PRINT_MAP(ADDRMAP2_COL_B5, addrmap2);
  DDRC_PRINT_MAP(ADDRMAP2_COL_B4, addrmap2);
  DDRC_PRINT_MAP(ADDRMAP2_COL_B3, addrmap2);
  DDRC_PRINT_MAP(ADDRMAP2_COL_B2, addrmap2);
  DDRC_PRINT_MAP(ADDRMAP3_COL_B9, addrmap3);
  DDRC_PRINT_MAP(ADDRMAP3_COL_B8, addrmap3);
  DDRC_PRINT_MAP(ADDRMAP3_COL_B7, addrmap3);
  DDRC_PRINT_MAP(ADDRMAP3_COL_B6, addrmap3);
  DDRC_PRINT_MAP(ADDRMAP4_COL_B11, addrmap4);
  DDRC_PRINT_MAP(ADDRMAP4_COL_B10, addrmap4);
  DDRC_PRINT_MAP(ADDRMAP5_ROW_B11, addrmap5);
  DDRC_PRINT_MAP(ADDRMAP5_ROW_B1, addrmap5);
  DDRC_PRINT_MAP(ADDRMAP5_ROW_B0, addrmap5);
  DDRC_PRINT_MAP(ADDRMAP6_ROW_B15, addrmap6);
  DDRC_PRINT_MAP(ADDRMAP6_ROW_B14, addrmap6);
  DDRC_PRINT_MAP(ADDRMAP6_ROW_B13, addrmap6);
  DDRC_PRINT_MAP(ADDRMAP6_ROW_B12, addrmap6);
  DDRC_PRINT_MAP(ADDRMAP7_ROW_B17, addrmap7);
  DDRC_PRINT_MAP(ADDRMAP7_ROW_B16, addrmap7);
  DDRC_PRINT_MAP(ADDRMAP8_BG_B1, addrmap8);
  DDRC_PRINT_MAP(ADDRMAP8_BG_B0, addrmap8);
  DDRC_PRINT_MAP(ADDRMAP9_ROW_B5, addrmap9);
  DDRC_PRINT_MAP(ADDRMAP9_ROW_B4, addrmap9);
  DDRC_PRINT_MAP(ADDRMAP9_ROW_B3, addrmap9);
  DDRC_PRINT_MAP(ADDRMAP9_ROW_B2, addrmap9);
  DDRC_PRINT_MAP(ADDRMAP10_ROW_B9, addrmap10);
  DDRC_PRINT_MAP(ADDRMAP10_ROW_B8, addrmap10);
  DDRC_PRINT_MAP(ADDRMAP10_ROW_B7, addrmap10);
  DDRC_PRINT_MAP(ADDRMAP10_ROW_B6, addrmap10);
  DDRC_PRINT_MAP(ADDRMAP11_ROW_B10, addrmap11);
}

/* Ignore the bitmap if it's the flag value, otherwise map it */
#define DDRC_CHECK_AND_MAP(bus_width, lpddr3, BIT_ID, info, addrmap) \
({ \
  uint32_t mapbit = DDRC_ ## BIT_ID ## _GET(addrmap); \
  uint32_t target_bit = DDRC_ ## BIT_ID ## _TARGET_BIT(bus_width, lpddr3); \
  if (mapbit != DDRC_ ## BIT_ID ## _SPECIAL) { \
    mapbit += DDRC_ ## BIT_ID ## _BASE; \
    /* account for AXI -> HIF shift */ \
    mapbit += 3; \
    info-> DDRC_ ## BIT_ID ## _TARGET |= \
      DDRC_MAP_BIT(info->address, mapbit, target_bit); \
  } \
})

static int decompose_address(DDR_Error_Info *info)
{
  uint32_t addrmap0 = DDRC_READ(DDRC_ADDRMAP0_OFFSET);
  uint32_t addrmap1 = DDRC_READ(DDRC_ADDRMAP1_OFFSET);
  uint32_t addrmap2 = DDRC_READ(DDRC_ADDRMAP2_OFFSET);
  uint32_t addrmap3 = DDRC_READ(DDRC_ADDRMAP3_OFFSET);
  uint32_t addrmap4 = DDRC_READ(DDRC_ADDRMAP4_OFFSET);
  uint32_t addrmap5 = DDRC_READ(DDRC_ADDRMAP5_OFFSET);
  uint32_t addrmap6 = DDRC_READ(DDRC_ADDRMAP6_OFFSET);
  uint32_t addrmap7 = DDRC_READ(DDRC_ADDRMAP7_OFFSET);
  uint32_t addrmap8 = DDRC_READ(DDRC_ADDRMAP8_OFFSET);
  uint32_t addrmap9 = DDRC_READ(DDRC_ADDRMAP9_OFFSET);
  uint32_t addrmap10 = DDRC_READ(DDRC_ADDRMAP10_OFFSET);
  uint32_t addrmap11 = DDRC_READ(DDRC_ADDRMAP11_OFFSET);
  uint32_t mstr = DDRC_READ(DDRC_MSTR_OFFSET);
  uint32_t bus_width = DDRC_MSTR_DATA_BUS_WIDTH_GET(mstr);
  bool lpddr3 = mstr & DDRC_MSTR_LPDDR3;

  homogenize_row(&addrmap5, &addrmap9, &addrmap10, &addrmap11);
  
  /* Clear items that will be written to */
  info->rank = 0;
  info->bank_group = 0;
  info->bank = 0;
  info->row = 0;
  info->column = 0;

  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP0_RANK_B0, info, addrmap0);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP1_BANK_B2, info, addrmap1);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP1_BANK_B1, info, addrmap1);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP1_BANK_B0, info, addrmap1);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP2_COL_B5, info, addrmap2);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP2_COL_B4, info, addrmap2);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP2_COL_B3, info, addrmap2);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP2_COL_B2, info, addrmap2);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP3_COL_B9, info, addrmap3);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP3_COL_B8, info, addrmap3);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP3_COL_B7, info, addrmap3);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP3_COL_B6, info, addrmap3);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP4_COL_B11, info, addrmap4);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP4_COL_B10, info, addrmap4);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP5_ROW_B11, info, addrmap5);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP5_ROW_B1, info, addrmap5);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP5_ROW_B0, info, addrmap5);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP6_ROW_B15, info, addrmap6);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP6_ROW_B14, info, addrmap6);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP6_ROW_B13, info, addrmap6);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP6_ROW_B12, info, addrmap6);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP7_ROW_B17, info, addrmap7);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP7_ROW_B16, info, addrmap7);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP8_BG_B1, info, addrmap8);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP8_BG_B0, info, addrmap8);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP9_ROW_B5, info, addrmap9);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP9_ROW_B4, info, addrmap9);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP9_ROW_B3, info, addrmap9);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP9_ROW_B2, info, addrmap9);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP10_ROW_B9, info, addrmap10);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP10_ROW_B8, info, addrmap10);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP10_ROW_B7, info, addrmap10);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP10_ROW_B6, info, addrmap10);
  DDRC_CHECK_AND_MAP(bus_width, lpddr3, ADDRMAP11_ROW_B10, info, addrmap11);

  /* Column[0:1] are always statically mapped to HIF[0:1] */
  info->column |= (info->address >> 3) & 0x3;

  /* select target address */
  uint32_t paddr0_val = DDRC_ECCPOISONADDR0_COL_SET(0, info->column);
  if (info->rank) {
    paddr0_val |= DDRC_ECCPOISONADDR0_RANK;
  }

  uint32_t paddr1_val = DDRC_ECCPOISONADDR1_BG_SET(0, info->bank_group);
  paddr1_val = DDRC_ECCPOISONADDR1_BANK_SET(paddr1_val, info->bank);
  paddr1_val = DDRC_ECCPOISONADDR1_ROW_SET(paddr1_val, info->row);
  return 0;
}

/* DDR Controller (DDRC) ECC Config Register 1 */
#define DDRC_ECCCFG1_OFFSET 0x74
#define DDRC_ECCCFG1_POISON_SINGLE BSP_BIT32(1)
#define DDRC_ECCCFG1_POISON_EN BSP_BIT32(0)

#define DDRC_SWCTL_OFFSET 0x74
#define DDRC_SWSTAT_OFFSET 0x324

#define DDRC_DBGCAM_OFFSET 0x308
#define DDRC_DBGCAM_WR_DATA_PIPELINE_EMPTY BSP_BIT32(28)
#define DDRC_DBGCAM_RD_DATA_PIPELINE_EMPTY BSP_BIT32(27)
#define DDRC_DBGCAM_DBG_WR_Q_EMPTY BSP_BIT32(26)
#define DDRC_DBGCAM_DBG_RD_Q_EMPTY BSP_BIT32(25)

#define DDRC_DBG1_OFFSET 0x304
#define DDRC_DBG1_DIS_DQ BSP_BIT32(0)

static void set_ecccfg1(volatile uint32_t *ecccfg1, uint32_t ecccfg1_val)
{
  uint32_t dbgcam_mask = DDRC_DBGCAM_WR_DATA_PIPELINE_EMPTY |
    DDRC_DBGCAM_RD_DATA_PIPELINE_EMPTY | DDRC_DBGCAM_DBG_WR_Q_EMPTY |
    DDRC_DBGCAM_DBG_RD_Q_EMPTY;
  volatile uint32_t *dbgcam = (uint32_t *)(ddrc_base + DDRC_DBGCAM_OFFSET);
  volatile uint32_t *dbg1 = (uint32_t *)(ddrc_base + DDRC_DBG1_OFFSET);

  /* disable dequeueing */
  *dbg1 |= DDRC_DBG1_DIS_DQ;
  /* poll for DDRC empty state */
  while((*dbgcam & dbgcam_mask) != dbgcam_mask);
  *ecccfg1 = ecccfg1_val;
  /* enable dequeueing */
  *dbg1 &= ~DDRC_DBG1_DIS_DQ;
}

void zynqmp_ddr_inject_fault( bool correctable )
{
  uint64_t poison_var;
  uint64_t read_var;
  volatile uint64_t *poison_addr = &poison_var;
  volatile uint32_t *ecccfg1 = (uint32_t *)(ddrc_base + DDRC_ECCCFG1_OFFSET);
  uint32_t ecccfg1_val = 0;
  volatile uint32_t *swctl = (uint32_t *)(ddrc_base + DDRC_SWCTL_OFFSET);
  volatile uint32_t *swstat = (uint32_t *)(ddrc_base + DDRC_SWSTAT_OFFSET);
  volatile uint32_t *poisonaddr0;
  uint32_t paddr0_val;
  volatile uint32_t *poisonaddr1;
  uint32_t paddr1_val;
  DDR_Error_Info info;
  poisonaddr0 = (uint32_t *)(ddrc_base + DDRC_ECCPOISONADDR0_OFFSET);
  poisonaddr1 = (uint32_t *)(ddrc_base + DDRC_ECCPOISONADDR1_OFFSET);

  info.address = (uint64_t)(uintptr_t)poison_addr;
  /* convert address to SDRAM address components */
  decompose_address(&info);

  /* select correctable/uncorrectable */
  ecccfg1_val &= ~DDRC_ECCCFG1_POISON_SINGLE;
  if (correctable) {
    ecccfg1_val |= DDRC_ECCCFG1_POISON_SINGLE;
  }

  /* enable poisoning */
  ecccfg1_val |= DDRC_ECCCFG1_POISON_EN;

  uint32_t isr_cookie;
  rtems_interrupt_local_disable(isr_cookie);
  /* swctl must be unset to allow modification of ecccfg1 */
  *swctl = 0;
  set_ecccfg1(ecccfg1, ecccfg1_val);
  *swctl = 1;
  rtems_interrupt_local_enable(isr_cookie);

  /* Wait for swctl propagation to swstat */
  while ((*swstat & 0x1) == 0);

  /* select target address */
  paddr0_val = DDRC_ECCPOISONADDR0_COL_SET(0, info.column);
  if (info.rank) {
    paddr0_val |= DDRC_ECCPOISONADDR0_RANK;
  }
  *poisonaddr0 = paddr0_val;

  paddr1_val = DDRC_ECCPOISONADDR1_BG_SET(0, info.bank_group);
  paddr1_val = DDRC_ECCPOISONADDR1_BANK_SET(paddr1_val, info.bank);
  paddr1_val = DDRC_ECCPOISONADDR1_ROW_SET(paddr1_val, info.row);
  *poisonaddr1 = paddr1_val;

  /* write to poison address */
  *poison_addr = 0x5555555555555555UL;

  /* flush cache to force write */
  rtems_cache_flush_multiple_data_lines(poison_addr, sizeof(*poison_addr));

  /* invalidate cache to force read */
  rtems_cache_invalidate_multiple_data_lines(poison_addr, sizeof(*poison_addr));

  /* Force a data sync barrier */
  _AARCH64_Data_synchronization_barrier();

  /* read from poison address to generate event */
  read_var = *poison_addr;
  read_var++;

  volatile uint32_t *qis = (uint32_t *)(ddr_qos_ctrl_base + DDR_QIS_OFFSET);

  /* disable poisoning */
  *swctl = 0;
  *ecccfg1 = 0;
  *swctl = 1;
}
#endif
