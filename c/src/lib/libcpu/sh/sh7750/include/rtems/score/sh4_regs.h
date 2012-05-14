/*
 * Bits on SH-4 registers.
 * See SH-4 Programming manual for more details.
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Alexandra Kossovsky <sasha@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __SH4_REGS_H__
#define __SH4_REGS_H__

/* SR -- Status Register */
#define SH4_SR_MD          0x40000000 /* Priveleged mode */
#define SH4_SR_RB          0x20000000 /* General register bank specifier */
#define SH4_SR_BL          0x10000000 /* Exeption/interrupt masking bit */
#define SH4_SR_FD          0x00008000 /* FPU disable bit */
#define SH4_SR_M           0x00000200 /* For signed division:
                                         divisor (module) is negative */
#define SH4_SR_Q           0x00000100 /* For signed division:
                                         dividend (and quotient) is negative */
#define SH4_SR_IMASK       0x000000f0 /* Interrupt mask level */
#define SH4_SR_IMASK_S     4
#define SH4_SR_S           0x00000002 /* Saturation for MAC instruction:
                                         if set, data in MACH/L register
                                         is restricted to 48/32 bits
                                         for MAC.W/L instructions */
#define SH4_SR_T           0x00000001 /* 1 if last condiyion was true */
#define SH4_SR_RESERV      0x8fff7d0d /* Reserved bits, read/write as 0 */

/* FPSCR -- FPU Starus/Control Register */
#define SH4_FPSCR_FR       0x00200000 /* FPU register bank specifier */
#define SH4_FPSCR_SZ       0x00100000 /* FMOV 64-bit transfer mode */
#define SH4_FPSCR_PR       0x00080000 /* Double-percision floating-point
                                         operations flag */
                                      /* SH4_FPSCR_SZ & SH4_FPSCR_PR != 1 */
#define SH4_FPSCR_DN       0x00040000 /* Treat denormalized number as zero */
#define SH4_FPSCR_CAUSE    0x0003f000 /* FPU exeption cause field */
#define SH4_FPSCR_CAUSE_S  12
#define SH4_FPSCR_ENABLE   0x00000f80 /* FPU exeption enable field */
#define SH4_FPSCR_ENABLE_s 7
#define SH4_FPSCR_FLAG     0x0000007d /* FPU exeption flag field */
#define SH4_FPSCR_FLAG_S   2
#define SH4_FPSCR_RM       0x00000001 /* Rounding mode:
                                         1/0 -- round to zero/nearest */
#define SH4_FPSCR_RESERV   0xffd00000 /* Reserved bits, read/write as 0 */

#endif
