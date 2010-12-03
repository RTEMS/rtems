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

#include <assert.h>

#include <bsp/lpc24xx.h>
#include <bsp/io.h>
#include <bsp/lcd.h>
#include <bsp/utility.h>
#include <bsp/system-clocks.h>

#define LCD_ENABLE BSP_BIT32(0)

rtems_status_code lpc24xx_lcd_set_mode(lpc24xx_lcd_mode mode, unsigned pin_config)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  bool enable = false;

  switch (mode) {
    case LCD_MODE_STN_4_BIT:
    case LCD_MODE_STN_8_BIT:
    case LCD_MODE_STN_DUAL_PANEL_4_BIT:
    case LCD_MODE_STN_DUAL_PANEL_8_BIT:
    case LCD_MODE_TFT_12_BIT_4_4_4:
    case LCD_MODE_TFT_16_BIT_5_6_5:
    case LCD_MODE_TFT_16_BIT_1_5_5_5:
    case LCD_MODE_TFT_24_BIT:
      enable = true;
      break;
    case LCD_MODE_DISABLED:
      enable = false;
      break;
    default:
      sc = RTEMS_IO_ERROR;
      break;
  }

  if (sc == RTEMS_SUCCESSFUL) {
    if (enable) {
      sc = lpc24xx_module_enable(LPC24XX_MODULE_LCD, LPC24XX_MODULE_PCLK_DEFAULT);
      assert(sc == RTEMS_SUCCESSFUL);

      PINSEL11 = BSP_FLD32(mode, 1, 3) | LCD_ENABLE;

      sc = lpc24xx_io_config(LPC24XX_MODULE_LCD, pin_config);
      assert(sc == RTEMS_SUCCESSFUL);
    } else {
      if (lpc24xx_lcd_current_mode() != LCD_MODE_DISABLED) {
        uint32_t lcd_ctrl = LCD_CTRL;

        /* Disable power */
        lcd_ctrl &= ~BSP_BIT32(11);
        LCD_CTRL = lcd_ctrl;

        lpc24xx_micro_seconds_delay(100000);

        /* Disable all signals */
        lcd_ctrl &= ~BSP_BIT32(0);
        LCD_CTRL = lcd_ctrl;
      }

      sc = lpc24xx_io_release(LPC24XX_MODULE_LCD, pin_config);
      assert(sc == RTEMS_SUCCESSFUL);

      PINSEL11 = 0;

      sc = lpc24xx_module_disable(LPC24XX_MODULE_LCD);
      assert(sc == RTEMS_SUCCESSFUL);
    }
  }

  return sc;
}

lpc24xx_lcd_mode lpc24xx_lcd_current_mode(void)
{
  uint32_t pinsel11 = PINSEL11;

  if ((PCONP & BSP_BIT32(20)) != 0 && (pinsel11 & LCD_ENABLE) != 0) {
    return BSP_FLD32GET(pinsel11, 1, 3);
  } else {
    return LCD_MODE_DISABLED;
  }
}
