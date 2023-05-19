/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC24XX_lcd
 *
 * @brief LCD support.
 */

/*
 * Copyright (C) 2010, 2012 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_ARM_LPC24XX_LCD_H
#define LIBBSP_ARM_LPC24XX_LCD_H

#include <rtems.h>

#include <bsp/io.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup lpc_dma LCD Support
 *
 * @ingroup RTEMSBSPsARMLPC24XX
 *
 * @brief LCD support.
 *
 * @{
 */

typedef enum {
  #ifdef ARM_MULTILIB_ARCH_V4
    LCD_MODE_STN_4_BIT = 0,
    LCD_MODE_STN_8_BIT,
    LCD_MODE_STN_DUAL_PANEL_4_BIT,
    LCD_MODE_STN_DUAL_PANEL_8_BIT,
    LCD_MODE_TFT_12_BIT_4_4_4,
    LCD_MODE_TFT_16_BIT_5_6_5,
    LCD_MODE_TFT_16_BIT_1_5_5_5,
    LCD_MODE_TFT_24_BIT,
    LCD_MODE_DISABLED
  #else
    LCD_MODE_STN_4_BIT = 0x4,
    LCD_MODE_STN_8_BIT = 0x6,
    LCD_MODE_STN_DUAL_PANEL_4_BIT = 0x84,
    LCD_MODE_STN_DUAL_PANEL_8_BIT = 0x86,
    LCD_MODE_TFT_12_BIT_4_4_4 = 0x2e,
    LCD_MODE_TFT_16_BIT_5_6_5 = 0x2c,
    LCD_MODE_TFT_16_BIT_1_5_5_5 = 0x28,
    LCD_MODE_TFT_24_BIT = 0x2a,
    LCD_MODE_DISABLED = 0xff
  #endif
} lpc24xx_lcd_mode;

/**
 * @brief Set the LCD @a mode.
 *
 * The pins are configured according to @a pins.
 *
 * @see lpc24xx_pin_config().
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_IO_ERROR Invalid mode.
 */
rtems_status_code lpc24xx_lcd_set_mode(
  lpc24xx_lcd_mode mode,
  const lpc24xx_pin_range *pins
);

lpc24xx_lcd_mode lpc24xx_lcd_current_mode(void);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC24XX_LCD_H */
