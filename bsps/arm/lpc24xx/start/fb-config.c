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
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp/arm-pl111-fb.h>

#include <bsp.h>
#include <bsp/fatal.h>
#include <bsp/io.h>
#include <bsp/lcd.h>
#include <bsp/lpc24xx.h>

static const lpc24xx_pin_range tft_16_bit_5_6_5_pins[] = {
  LPC24XX_PIN_LCD_DCLK,
  LPC24XX_PIN_LCD_FP,
  LPC24XX_PIN_LCD_LP,
  LPC24XX_PIN_LCD_VD_3_P4_29,
  LPC24XX_PIN_LCD_VD_4_P2_6,
  LPC24XX_PIN_LCD_VD_5_P2_7,
  LPC24XX_PIN_LCD_VD_6_P2_8,
  LPC24XX_PIN_LCD_VD_7_P2_9,
  LPC24XX_PIN_LCD_VD_10_P1_20,
  LPC24XX_PIN_LCD_VD_11_P1_21,
  LPC24XX_PIN_LCD_VD_12_P1_22,
  LPC24XX_PIN_LCD_VD_13_P1_23,
  LPC24XX_PIN_LCD_VD_14_P1_24,
  LPC24XX_PIN_LCD_VD_15_P1_25,
  LPC24XX_PIN_LCD_VD_19_P2_13,
  LPC24XX_PIN_LCD_VD_20_P1_26,
  LPC24XX_PIN_LCD_VD_21_P1_27,
  LPC24XX_PIN_LCD_VD_22_P1_28,
  LPC24XX_PIN_LCD_VD_23_P1_29,
  LPC24XX_PIN_TERMINAL
};

static void fb_set_up(const pl111_fb_config *cfg)
{
  rtems_status_code sc;

  sc = lpc24xx_module_enable(LPC24XX_MODULE_LCD, LPC24XX_MODULE_PCLK_DEFAULT);
  if (sc != RTEMS_SUCCESSFUL) {
    bsp_fatal(LPC24XX_FATAL_PL111_SET_UP);
  }

  #ifdef ARM_MULTILIB_ARCH_V4
    PINSEL11 = BSP_FLD32(LCD_MODE_TFT_16_BIT_5_6_5, 1, 3) | BSP_BIT32(0);
  #endif

  #ifdef ARM_MULTILIB_ARCH_V7M
    volatile lpc17xx_scb *scb = &LPC17XX_SCB;

    scb->matrixarb = 0x0c09;
  #endif
}

static void fb_pins_set_up(const pl111_fb_config *cfg)
{
  rtems_status_code sc;

  sc = lpc24xx_pin_config(tft_16_bit_5_6_5_pins, LPC24XX_PIN_SET_FUNCTION);
  if (sc != RTEMS_SUCCESSFUL) {
    bsp_fatal(LPC24XX_FATAL_PL111_PINS_SET_UP);
  }
}

static void fb_pins_tear_down(const pl111_fb_config *cfg)
{
  rtems_status_code sc;

  sc = lpc24xx_pin_config(tft_16_bit_5_6_5_pins, LPC24XX_PIN_SET_INPUT);
  if (sc != RTEMS_SUCCESSFUL) {
    bsp_fatal(LPC24XX_FATAL_PL111_PINS_TEAR_DOWN);
  }
}

static void fb_tear_down(const pl111_fb_config *cfg)
{
  rtems_status_code sc;

  #ifdef ARM_MULTILIB_ARCH_V4
    PINSEL11 = 0;
  #endif

  sc = lpc24xx_module_disable(LPC24XX_MODULE_LCD);
  if (sc != RTEMS_SUCCESSFUL) {
    bsp_fatal(LPC24XX_FATAL_PL111_TEAR_DOWN);
  }
}

static const pl111_fb_config fb_config = {
  .regs = (volatile pl111 *) LCD_BASE_ADDR,

  .timing0 = PL111_LCD_TIMING0_PPL(640 / 16 - 1)
    | PL111_LCD_TIMING0_HSW(0x1d)
    | PL111_LCD_TIMING0_HFP(0x0f)
    | PL111_LCD_TIMING0_HBP(0x71),
  .timing1 = PL111_LCD_TIMING1_LPP(480 - 1)
    | PL111_LCD_TIMING1_VSW(0x02)
    | PL111_LCD_TIMING1_VFP(0x0a)
    | PL111_LCD_TIMING1_VBP(0x20),
  .timing2 = PL111_LCD_TIMING2_PCD_LO(0x3)
    | PL111_LCD_TIMING2_ACB(0x0)
    | PL111_LCD_TIMING2_IVS
    | PL111_LCD_TIMING2_IHS
    | PL111_LCD_TIMING2_IPC
    | PL111_LCD_TIMING2_CPL(640 - 1)
    | PL111_LCD_TIMING2_PCD_HI(0x0),
  .timing3 = 0x0,
  .control = PL111_LCD_CONTROL_LCD_TFT
    | PL111_LCD_CONTROL_LCD_BPP(PL111_LCD_CONTROL_LCD_BPP_16_5_6_5)
    | PL111_LCD_CONTROL_BGR,
  .power_delay_in_us = 100000,

  .set_up = fb_set_up,
  .pins_set_up = fb_pins_set_up,
  .pins_tear_down = fb_pins_tear_down,
  .tear_down = fb_tear_down
};

const pl111_fb_config *arm_pl111_fb_get_config(void)
{
  return &fb_config;
}
