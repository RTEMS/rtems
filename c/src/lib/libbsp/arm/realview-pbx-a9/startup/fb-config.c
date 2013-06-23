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

#include <bsp/arm-pl111-fb.h>
#include <bsp.h>

static const pl111_fb_config fb_config = {
  .regs = (volatile pl111 *) 0x10020000,

  /*
   * Values obtained from "RealView Platform Baseboard Explore for Cortex-A9
   * User Guide" section 4.6.1 "Display resolutions and display memory
   * organization".
   */
  .timing0 = 0x3f1f3f9c,
  .timing1 = 0x090b61df,
  .timing2 = 0x067f1800,

  .timing3 = 0x0,
  .control = PL111_LCD_CONTROL_LCD_TFT
    | PL111_LCD_CONTROL_LCD_BPP(PL111_LCD_CONTROL_LCD_BPP_16),
  .power_delay_in_us = 100000
};

const pl111_fb_config *arm_pl111_fb_get_config(void)
{
  return &fb_config;
}

void arm_pl111_fb_pins_set_up(const pl111_fb_config *cfg)
{
  /* TODO */
}

void arm_pl111_fb_pins_tear_down(const pl111_fb_config *cfg)
{
  /* TODO */
}
