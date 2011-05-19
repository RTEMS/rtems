/**
 * @file
 *
 * @ingroup lpc_lcd
 *
 * @brief LCD support API.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_ARM_SHARED_LPC_LCD_H
#define LIBBSP_ARM_SHARED_LPC_LCD_H

#include <bsp/utility.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup lpc_lcd LCD Support
 *
 * @ingroup lpc
 * @ingroup lpc32xx
 *
 * @brief LCD support.
 *
 * @{
 */

typedef struct {
  uint8_t img [1024];
  uint32_t ctrl;
  uint32_t cfg;
  uint32_t pal0;
  uint32_t pal1;
  uint32_t xy;
  uint32_t clip;
  uint32_t intmsk;
  uint32_t intclr;
  uint32_t intraw;
  uint32_t intstat;
} lpc_cursor;

typedef struct {
  uint32_t timh;
  uint32_t timv;
  uint32_t pol;
  uint32_t le;
  uint32_t upbase;
  uint32_t lpbase;
  uint32_t ctrl;
  uint32_t intmsk;
  uint32_t intraw;
  uint32_t intstat;
  uint32_t intclr;
  uint32_t upcurr;
  uint32_t lpcurr;
  uint8_t reserved_0 [0x200 - 0x034];
  uint16_t pal [256];
  uint8_t reserved_1 [0x800 - 0x400];
  lpc_cursor crsr;
} lpc_lcd;

/**
 * @name LCD Configuration Register
 *
 * @{
 */

#define LCD_CFG_CLKDIV(val) BSP_FLD32(val, 0, 4)
#define LCD_CFG_HCLK_ENABLE BSP_BIT32(5)
#define LCD_CFG_MODE_SELECT(val) BSP_FLD32(val, 6, 7)
#define LCD_CFG_DISPLAY_TYPE BSP_BIT32(8)

/** @} */

/**
 * @name LCD Horizontal Timing Register
 *
 * @{
 */

#define LCD_TIMH_PPL(val) BSP_FLD32(val, 2, 7)
#define LCD_TIMH_PPL_GET(reg) BSP_FLD32GET(reg, 2, 7)
#define LCD_TIMH_HSW(val) BSP_FLD32(val, 8, 15)
#define LCD_TIMH_HSW_GET(reg) BSP_FLD32GET(reg, 8, 15)
#define LCD_TIMH_HFP(val) BSP_FLD32(val, 16, 23)
#define LCD_TIMH_HFP_GET(reg) BSP_FLD32GET(reg, 16, 23)
#define LCD_TIMH_HBP(val) BSP_FLD32(val, 24, 31)
#define LCD_TIMH_HBP_GET(reg) BSP_FLD32GET(reg, 24, 31)

/** @} */

/**
 * @name LCD Vertical Timing Register
 *
 * @{
 */

#define LCD_TIMV_LPP(val) BSP_FLD32(val, 0, 9)
#define LCD_TIMV_LPP_GET(reg) BSP_FLD32GET(reg, 0, 9)
#define LCD_TIMV_VSW(val) BSP_FLD32(val, 10, 15)
#define LCD_TIMV_VSW_GET(reg) BSP_FLD32GET(reg, 10, 15)
#define LCD_TIMV_VFP(val) BSP_FLD32(val, 16, 23)
#define LCD_TIMV_VFP_GET(reg) BSP_FLD32GET(reg, 16, 23)
#define LCD_TIMV_VBP(val) BSP_FLD32(val, 24, 31)
#define LCD_TIMV_VBP_GET(reg) BSP_FLD32GET(reg, 24, 31)

/** @} */

/**
 * @name LCD Clock and Signal Polarity Register
 *
 * @{
 */

#define LCD_POL_PCD_LO(val) BSP_FLD32(val, 0, 4)
#define LCD_POL_PCD_LO_GET(reg) BSP_FLD32GET(reg, 0, 4)
#define LCD_POL_CLKSEL BSP_BIT32(5)
#define LCD_POL_ACB(val) BSP_FLD32(val, 6, 10)
#define LCD_POL_ACB_GET(reg) BSP_FLD32GET(reg, 6, 10)
#define LCD_POL_IVS BSP_BIT32(11)
#define LCD_POL_IHS BSP_BIT32(12)
#define LCD_POL_IPC BSP_BIT32(13)
#define LCD_POL_IOE BSP_BIT32(14)
#define LCD_POL_CPL(val) BSP_FLD32(val, 16, 25)
#define LCD_POL_CPL_GET(reg) BSP_FLD32GET(reg, 16, 25)
#define LCD_POL_BCD BSP_BIT32(26)
#define LCD_POL_PCD_HI(val) BSP_FLD32(val, 27, 31)
#define LCD_POL_PCD_HI_GET(reg) BSP_FLD32GET(reg, 27, 31)

/** @} */

/**
 * @name LCD Line End Control Register
 *
 * @{
 */

#define LCD_LE_LED(val) BSP_FLD32(val, 0, 6)
#define LCD_LE_LEE BSP_BIT32(16)

/** @} */

/**
 * @name LCD Control Register
 *
 * @{
 */

#define LCD_CTRL_LCDEN BSP_BIT32(0)
#define LCD_CTRL_LCDBPP(val) BSP_FLD32(val, 1, 3)
#define LCD_CTRL_LCDBPP_GET(reg) BSP_FLD32GET(reg, 1, 3)
#define LCD_CTRL_LCDBW BSP_BIT32(4)
#define LCD_CTRL_LCDTFT BSP_BIT32(5)
#define LCD_CTRL_LCDMONO8 BSP_BIT32(6)
#define LCD_CTRL_LCDDUAL BSP_BIT32(7)
#define LCD_CTRL_BGR BSP_BIT32(8)
#define LCD_CTRL_BEBO BSP_BIT32(9)
#define LCD_CTRL_BEPO BSP_BIT32(10)
#define LCD_CTRL_LCDPWR BSP_BIT32(11)
#define LCD_CTRL_LCDVCOMP(val) BSP_FLD32(val, 12, 13)
#define LCD_CTRL_LCDVCOMP_GET(reg) BSP_FLD32GET(reg, 12, 13)
#define LCD_CTRL_WATERMARK BSP_BIT32(16)

/** @} */

/**
 * @name LCD Interrupt Registers
 *
 * @{
 */

#define LCD_INT_FUF BSP_BIT32(1)
#define LCD_INT_LNBU BSP_BIT32(2)
#define LCD_INT_VCOMP BSP_BIT32(3)
#define LCD_INT_BER BSP_BIT32(4)

/** @} */

/**
 * @name LCD Color Palette Register
 *
 * @{
 */

#define LCD_PAL_R(val) BSP_FLD16(val, 0, 4)
#define LCD_PAL_G(val) BSP_FLD16(val, 5, 9)
#define LCD_PAL_B(val) BSP_FLD16(val, 10, 14)
#define LCD_PAL_I BSP_BIT16(15)

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_LPC_LCD_H */
