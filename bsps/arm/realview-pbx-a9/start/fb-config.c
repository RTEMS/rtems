/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2013 embedded brains GmbH & Co. KG
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

#include <bsp/arm-pl111-fb.h>
#include <bsp.h>

static void fb_set_up(const pl111_fb_config *cfg)
{
  /* TODO */
}

static void fb_pins_set_up(const pl111_fb_config *cfg)
{
  /* TODO */
}

static void fb_pins_tear_down(const pl111_fb_config *cfg)
{
  /* TODO */
}

static void fb_tear_down(const pl111_fb_config *cfg)
{
  /* TODO */
}

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
    | PL111_LCD_CONTROL_LCD_BPP(PL111_LCD_CONTROL_LCD_BPP_16)
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
