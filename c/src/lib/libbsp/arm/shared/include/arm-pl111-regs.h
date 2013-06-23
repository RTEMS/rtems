/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_SHARED_ARM_PL111_REGS_H
#define LIBBSP_ARM_SHARED_ARM_PL111_REGS_H

#include <bsp/utility.h>

typedef struct {
	uint32_t timing0;
#define PL111_LCD_TIMING0_PPL(val) BSP_FLD32(val, 2, 7)
#define PL111_LCD_TIMING0_PPL_GET(reg) BSP_FLD32GET(reg, 2, 7)
#define PL111_LCD_TIMING0_PPL_SET(reg, val) BSP_FLD32SET(reg, val, 2, 7)
#define PL111_LCD_TIMING0_HSW(val) BSP_FLD32(val, 8, 15)
#define PL111_LCD_TIMING0_HSW_GET(reg) BSP_FLD32GET(reg, 8, 15)
#define PL111_LCD_TIMING0_HSW_SET(reg, val) BSP_FLD32SET(reg, val, 8, 15)
#define PL111_LCD_TIMING0_HFP(val) BSP_FLD32(val, 16, 23)
#define PL111_LCD_TIMING0_HFP_GET(reg) BSP_FLD32GET(reg, 16, 23)
#define PL111_LCD_TIMING0_HFP_SET(reg, val) BSP_FLD32SET(reg, val, 16, 23)
#define PL111_LCD_TIMING0_HBP(val) BSP_FLD32(val, 24, 31)
#define PL111_LCD_TIMING0_HBP_GET(reg) BSP_FLD32GET(reg, 24, 31)
#define PL111_LCD_TIMING0_HBP_SET(reg, val) BSP_FLD32SET(reg, val, 24, 31)
	uint32_t timing1;
#define PL111_LCD_TIMING1_LPP(val) BSP_FLD32(val, 0, 9)
#define PL111_LCD_TIMING1_LPP_GET(reg) BSP_FLD32GET(reg, 0, 9)
#define PL111_LCD_TIMING1_LPP_SET(reg, val) BSP_FLD32SET(reg, val, 0, 9)
#define PL111_LCD_TIMING1_VSW(val) BSP_FLD32(val, 10, 15)
#define PL111_LCD_TIMING1_VSW_GET(reg) BSP_FLD32GET(reg, 10, 15)
#define PL111_LCD_TIMING1_VSW_SET(reg, val) BSP_FLD32SET(reg, val, 10, 15)
#define PL111_LCD_TIMING1_VFP(val) BSP_FLD32(val, 16, 23)
#define PL111_LCD_TIMING1_VFP_GET(reg) BSP_FLD32GET(reg, 16, 23)
#define PL111_LCD_TIMING1_VFP_SET(reg, val) BSP_FLD32SET(reg, val, 16, 23)
#define PL111_LCD_TIMING1_VBP(val) BSP_FLD32(val, 24, 31)
#define PL111_LCD_TIMING1_VBP_GET(reg) BSP_FLD32GET(reg, 24, 31)
#define PL111_LCD_TIMING1_VBP_SET(reg, val) BSP_FLD32SET(reg, val, 24, 31)
	uint32_t timing2;
#define PL111_LCD_TIMING2_PCD_LO(val) BSP_FLD32(val, 0, 4)
#define PL111_LCD_TIMING2_PCD_LO_GET(reg) BSP_FLD32GET(reg, 0, 4)
#define PL111_LCD_TIMING2_PCD_LO_SET(reg, val) BSP_FLD32SET(reg, val, 0, 4)
#define PL111_LCD_TIMING2_CLKSEL BSP_BIT32(5)
#define PL111_LCD_TIMING2_ACB(val) BSP_FLD32(val, 6, 10)
#define PL111_LCD_TIMING2_ACB_GET(reg) BSP_FLD32GET(reg, 6, 10)
#define PL111_LCD_TIMING2_ACB_SET(reg, val) BSP_FLD32SET(reg, val, 6, 10)
#define PL111_LCD_TIMING2_IVS BSP_BIT32(11)
#define PL111_LCD_TIMING2_IHS BSP_BIT32(12)
#define PL111_LCD_TIMING2_IPC BSP_BIT32(13)
#define PL111_LCD_TIMING2_IOE BSP_BIT32(14)
#define PL111_LCD_TIMING2_CPL(val) BSP_FLD32(val, 16, 25)
#define PL111_LCD_TIMING2_CPL_GET(reg) BSP_FLD32GET(reg, 16, 25)
#define PL111_LCD_TIMING2_CPL_SET(reg, val) BSP_FLD32SET(reg, val, 16, 25)
#define PL111_LCD_TIMING2_BCD BSP_BIT32(26)
#define PL111_LCD_TIMING2_PCD_HI(val) BSP_FLD32(val, 27, 31)
#define PL111_LCD_TIMING2_PCD_HI_GET(reg) BSP_FLD32GET(reg, 27, 31)
#define PL111_LCD_TIMING2_PCD_HI_SET(reg, val) BSP_FLD32SET(reg, val, 27, 31)
	uint32_t timing3;
#define PL111_LCD_TIMING3_LED(val) BSP_FLD32(val, 0, 6)
#define PL111_LCD_TIMING3_LED_GET(reg) BSP_FLD32GET(reg, 0, 6)
#define PL111_LCD_TIMING3_LED_SET(reg, val) BSP_FLD32SET(reg, val, 0, 6)
#define PL111_LCD_TIMING3_LEE BSP_BIT32(16)
	uint32_t upbase;
	uint32_t lpbase;
	uint32_t control;
#define PL111_LCD_CONTROL_LCD_EN BSP_BIT32(0)
#define PL111_LCD_CONTROL_LCD_BPP(val) BSP_FLD32(val, 1, 3)
#define PL111_LCD_CONTROL_LCD_BPP_GET(reg) BSP_FLD32GET(reg, 1, 3)
#define PL111_LCD_CONTROL_LCD_BPP_SET(reg, val) BSP_FLD32SET(reg, val, 1, 3)
#define PL111_LCD_CONTROL_LCD_BPP_1 0x00U
#define PL111_LCD_CONTROL_LCD_BPP_2 0x01U
#define PL111_LCD_CONTROL_LCD_BPP_4 0x02U
#define PL111_LCD_CONTROL_LCD_BPP_8 0x03U
#define PL111_LCD_CONTROL_LCD_BPP_16 0x04U
#define PL111_LCD_CONTROL_LCD_BPP_24 0x05U
#define PL111_LCD_CONTROL_LCD_BPP_16 0x06U
#define PL111_LCD_CONTROL_LCD_BPP_12 0x07U
#define PL111_LCD_CONTROL_LCD_BW BSP_BIT32(4)
#define PL111_LCD_CONTROL_LCD_TFT BSP_BIT32(5)
#define PL111_LCD_CONTROL_LCD_MONO8 BSP_BIT32(6)
#define PL111_LCD_CONTROL_LCD_DUAL BSP_BIT32(7)
#define PL111_LCD_CONTROL_BGR BSP_BIT32(8)
#define PL111_LCD_CONTROL_BEBO BSP_BIT32(9)
#define PL111_LCD_CONTROL_BEPO BSP_BIT32(10)
#define PL111_LCD_CONTROL_LCD_PWR BSP_BIT32(11)
#define PL111_LCD_CONTROL_LCD_V_COMP(val) BSP_FLD32(val, 12, 13)
#define PL111_LCD_CONTROL_LCD_V_COMP_GET(reg) BSP_FLD32GET(reg, 12, 13)
#define PL111_LCD_CONTROL_LCD_V_COMP_SET(reg, val) BSP_FLD32SET(reg, val, 12, 13)
#define PL111_LCD_CONTROL_WATERMARK BSP_BIT32(16)
	uint32_t imsc;
	uint32_t ris;
	uint32_t mis;
	uint32_t icr;
#define PL111_LCD_I_FUF BSP_BIT32(1)
#define PL111_LCD_I_LNBU BSP_BIT32(2)
#define PL111_LCD_I_VCOMP BSP_BIT32(3)
#define PL111_LCD_I_MBERROR BSP_BIT32(4)
	uint32_t upcurr;
	uint32_t lpcurr;
	uint32_t reserved_34[115];
	uint16_t pal[256];
#define PL111_LCD_PAL_R(val) BSP_FLD16(val, 0, 4)
#define PL111_LCD_PAL_R_GET(reg) BSP_FLD16GET(reg, 0, 4)
#define PL111_LCD_PAL_R_SET(reg, val) BSP_FLD16SET(reg, val, 0, 4)
#define PL111_LCD_PAL_G(val) BSP_FLD16(val, 5, 9)
#define PL111_LCD_PAL_G_GET(reg) BSP_FLD16GET(reg, 5, 9)
#define PL111_LCD_PAL_G_SET(reg, val) BSP_FLD16SET(reg, val, 5, 9)
#define PL111_LCD_PAL_B(val) BSP_FLD16(val, 10, 14)
#define PL111_LCD_PAL_B_GET(reg) BSP_FLD16GET(reg, 10, 14)
#define PL111_LCD_PAL_B_SET(reg, val) BSP_FLD16SET(reg, val, 10, 14)
#define PL111_LCD_PAL_I BSP_BIT16(15)
} pl111_lcd;

typedef struct {
	uint8_t image[1024];
	uint32_t ctrl;
#define PL111_CRSR_CTRL_ON BSP_BIT32(0)
#define PL111_CRSR_CTRL_NUMBER(val) BSP_FLD32(val, 4, 5)
#define PL111_CRSR_CTRL_NUMBER_GET(reg) BSP_FLD32GET(reg, 4, 5)
#define PL111_CRSR_CTRL_NUMBER_SET(reg, val) BSP_FLD32SET(reg, val, 4, 5)
	uint32_t config;
#define PL111_CRSR_CONFIG_SIZE BSP_BIT32(0)
#define PL111_CRSR_CONFIG_FRAME_SYNC BSP_BIT32(1)
	uint32_t palette0;
	uint32_t palette1;
#define PL111_CRSR_PALETTE_RED(val) BSP_FLD32(val, 0, 7)
#define PL111_CRSR_PALETTE_RED_GET(reg) BSP_FLD32GET(reg, 0, 7)
#define PL111_CRSR_PALETTE_RED_SET(reg, val) BSP_FLD32SET(reg, val, 0, 7)
#define PL111_CRSR_PALETTE_GREEN(val) BSP_FLD32(val, 8, 15)
#define PL111_CRSR_PALETTE_GREEN_GET(reg) BSP_FLD32GET(reg, 8, 15)
#define PL111_CRSR_PALETTE_GREEN_SET(reg, val) BSP_FLD32SET(reg, val, 8, 15)
#define PL111_CRSR_PALETTE_BLUE(val) BSP_FLD32(val, 16, 23)
#define PL111_CRSR_PALETTE_BLUE_GET(reg) BSP_FLD32GET(reg, 16, 23)
#define PL111_CRSR_PALETTE_BLUE_SET(reg, val) BSP_FLD32SET(reg, val, 16, 23)
	uint32_t xy;
#define PL111_CRSR_XY_X(val) BSP_FLD32(val, 0, 9)
#define PL111_CRSR_XY_X_GET(reg) BSP_FLD32GET(reg, 0, 9)
#define PL111_CRSR_XY_X_SET(reg, val) BSP_FLD32SET(reg, val, 0, 9)
#define PL111_CRSR_XY_X_EXP(val) BSP_FLD32(val, 10, 11)
#define PL111_CRSR_XY_X_EXP_GET(reg) BSP_FLD32GET(reg, 10, 11)
#define PL111_CRSR_XY_X_EXP_SET(reg, val) BSP_FLD32SET(reg, val, 10, 11)
#define PL111_CRSR_XY_Y(val) BSP_FLD32(val, 16, 25)
#define PL111_CRSR_XY_Y_GET(reg) BSP_FLD32GET(reg, 16, 25)
#define PL111_CRSR_XY_Y_SET(reg, val) BSP_FLD32SET(reg, val, 16, 25)
#define PL111_CRSR_XY_Y_EXP(val) BSP_FLD32(val, 25, 27)
#define PL111_CRSR_XY_Y_EXP_GET(reg) BSP_FLD32GET(reg, 25, 27)
#define PL111_CRSR_XY_Y_EXP_SET(reg, val) BSP_FLD32SET(reg, val, 25, 27)
	uint32_t clip;
#define PL111_CRSR_CLIP_X(val) BSP_FLD32(val, 0, 5)
#define PL111_CRSR_CLIP_X_GET(reg) BSP_FLD32GET(reg, 0, 5)
#define PL111_CRSR_CLIP_X_SET(reg, val) BSP_FLD32SET(reg, val, 0, 5)
#define PL111_CRSR_CLIP_Y(val) BSP_FLD32(val, 8, 13)
#define PL111_CRSR_CLIP_Y_GET(reg) BSP_FLD32GET(reg, 8, 13)
#define PL111_CRSR_CLIP_Y_SET(reg, val) BSP_FLD32SET(reg, val, 8, 13)
	uint32_t imsc;
	uint32_t icr;
	uint32_t ris;
	uint32_t mis;
#define PL111_CRSR_I_CRSR BSP_BIT32(0)
} pl111_crsr;

typedef struct {
	pl111_lcd lcd;
	uint32_t reserved_400[256];
	pl111_crsr crsr;
} pl111;

#endif /* LIBBSP_ARM_SHARED_ARM_PL111_REGS_H */
