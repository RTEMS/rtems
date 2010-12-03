/**
 * @file
 *
 * @ingroup lpc24xx_lcd
 *
 * @brief LCD support.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_ARM_LPC24XX_LCD_H
#define LIBBSP_ARM_LPC24XX_LCD_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup lpc_dma LCD Support
 *
 * @ingroup lpc24xx
 *
 * @brief LCD support.
 *
 * @{
 */

typedef enum {
  LCD_MODE_STN_4_BIT = 0,
  LCD_MODE_STN_8_BIT,
  LCD_MODE_STN_DUAL_PANEL_4_BIT,
  LCD_MODE_STN_DUAL_PANEL_8_BIT,
  LCD_MODE_TFT_12_BIT_4_4_4,
  LCD_MODE_TFT_16_BIT_5_6_5,
  LCD_MODE_TFT_16_BIT_1_5_5_5,
  LCD_MODE_TFT_24_BIT,
  LCD_MODE_DISABLED
} lpc24xx_lcd_mode;

/**
 * @brief Set the LCD @a mode.
 *
 * The pins are configured according to the @a pin_config.
 *
 * @see lpc24xx_io_config() and lpc24xx_io_release().
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_IO_ERROR Invalid mode.
 */
rtems_status_code lpc24xx_lcd_set_mode(
  lpc24xx_lcd_mode mode,
  unsigned pin_config
);

lpc24xx_lcd_mode lpc24xx_lcd_current_mode(void);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC24XX_LCD_H */
