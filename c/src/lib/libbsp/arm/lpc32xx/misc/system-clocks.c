/**
 * @file
 *
 * @ingroup lpc32xx
 *
 * @brief System clocks.
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

#include <bsp.h>
#include <bsp/lpc32xx.h>

uint32_t lpc32xx_sysclk(void)
{
  uint32_t sysclk_ctrl = LPC32XX_SYSCLK_CTRL;

  return (sysclk_ctrl & 0x1) == 0 ?
    LPC32XX_OSCILLATOR_MAIN
      : (397 * LPC32XX_OSCILLATOR_RTC);
}

uint32_t lpc32xx_hclkpll_clk(void)
{
  uint32_t sysclk = lpc32xx_sysclk();
  uint32_t hclkpll_ctrl = LPC32XX_HCLKPLL_CTRL;
  uint32_t m = HCLK_PLL_M_GET(hclkpll_ctrl) + 1;
  uint32_t n = HCLK_PLL_N_GET(hclkpll_ctrl) + 1;
  uint32_t p = 1U << HCLK_PLL_P_GET(hclkpll_ctrl);
  uint32_t hclkpll_clk = 0;

  if ((hclkpll_ctrl & HCLK_PLL_BYPASS) != 0) {
    if ((hclkpll_ctrl & HCLK_PLL_DIRECT) != 0) {
      hclkpll_clk = sysclk;
    } else {
      hclkpll_clk = sysclk / (2 * p);
    }
  } else {
    if ((hclkpll_ctrl & HCLK_PLL_DIRECT) != 0) {
      hclkpll_clk = (m * sysclk) / n;
    } else {
      if ((hclkpll_ctrl & HCLK_PLL_FBD_FCLKOUT) != 0) {
        hclkpll_clk = m * (sysclk / n);
      } else {
        hclkpll_clk = (m / (2 * p)) * (sysclk / n);
      }
    }
  }

  return hclkpll_clk;
}

uint32_t lpc32xx_periph_clk(void)
{
  uint32_t pwr_ctrl = LPC32XX_PWR_CTRL;
  uint32_t periph_clk = 0;

  if ((pwr_ctrl & PWR_NORMAL_RUN_MODE) != 0) {
    uint32_t hclkdiv_ctrl = LPC32XX_HCLKDIV_CTRL;
    uint32_t div = HCLK_DIV_PERIPH_CLK_GET(hclkdiv_ctrl) + 1;

    periph_clk = lpc32xx_hclkpll_clk() / div;
  } else {
    periph_clk = lpc32xx_sysclk();
  }

  return periph_clk;
}

uint32_t lpc32xx_hclk(void)
{
  uint32_t pwr_ctrl = LPC32XX_PWR_CTRL;
  uint32_t hclk = 0;

  if ((pwr_ctrl & PWR_HCLK_USES_PERIPH_CLK) != 0) {
    hclk = lpc32xx_periph_clk();
  } else {
    if ((pwr_ctrl & PWR_NORMAL_RUN_MODE) != 0) {
      uint32_t hclkdiv_ctrl = LPC32XX_HCLKDIV_CTRL;
      uint32_t div = 1U << HCLK_DIV_HCLK_GET(hclkdiv_ctrl);

      hclk = lpc32xx_hclkpll_clk() / div;
    } else {
      hclk = lpc32xx_sysclk();
    }
  }

  return hclk;
}

uint32_t lpc32xx_arm_clk(void)
{
  uint32_t pwr_ctrl = LPC32XX_PWR_CTRL;
  uint32_t arm_clk = 0;

  if ((pwr_ctrl & PWR_HCLK_USES_PERIPH_CLK) != 0) {
    arm_clk = lpc32xx_periph_clk();
  } else {
    if ((pwr_ctrl & PWR_NORMAL_RUN_MODE) != 0) {
      arm_clk = lpc32xx_hclkpll_clk();
    } else {
      arm_clk = lpc32xx_sysclk();
    }
  }

  return arm_clk;
}

uint32_t lpc32xx_ddram_clk(void)
{
  uint32_t hclkdiv_ctrl = LPC32XX_HCLKDIV_CTRL;
  uint32_t div = HCLK_DIV_DDRAM_CLK_GET(hclkdiv_ctrl);
  uint32_t ddram_clk = 0;

  if (div != 0) {
    uint32_t pwr_ctrl = LPC32XX_PWR_CTRL;

    if ((pwr_ctrl & PWR_NORMAL_RUN_MODE) != 0) {
      ddram_clk = lpc32xx_hclkpll_clk();
    } else {
      ddram_clk = lpc32xx_sysclk();
    }

    ddram_clk /= div;
  }

  return ddram_clk;
}
