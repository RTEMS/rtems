/**
 * @file
 *
 * @ingroup lpc32xx
 *
 * @brief Startup code.
 */

/*
 * Copyright (c) 2009-2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/start.h>
#include <bsp/lpc32xx.h>
#include <bsp/mmu.h>
#include <bsp/arm-cp15-start.h>
#include <bsp/linker-symbols.h>
#include <bsp/uart-output-char.h>

#ifdef LPC32XX_DISABLE_READ_WRITE_DATA_CACHE
  #define LPC32XX_MMU_READ_WRITE_DATA LPC32XX_MMU_READ_WRITE
#else
  #define LPC32XX_MMU_READ_WRITE_DATA LPC32XX_MMU_READ_WRITE_CACHED
#endif

#ifdef LPC32XX_DISABLE_READ_ONLY_PROTECTION
  #define LPC32XX_MMU_READ_ONLY_DATA LPC32XX_MMU_READ_WRITE_CACHED
  #define LPC32XX_MMU_CODE LPC32XX_MMU_READ_WRITE_CACHED
#else
  #define LPC32XX_MMU_READ_ONLY_DATA LPC32XX_MMU_READ_ONLY_CACHED
  #define LPC32XX_MMU_CODE LPC32XX_MMU_READ_ONLY_CACHED
#endif

#ifndef LPC32XX_DISABLE_MMU
  static const BSP_START_DATA_SECTION arm_cp15_start_section_config
    lpc32xx_mmu_config_table [] = {
    {
      .begin = (uint32_t) bsp_section_fast_text_begin,
      .end = (uint32_t) bsp_section_fast_text_end,
      .flags = LPC32XX_MMU_CODE
    }, {
      .begin = (uint32_t) bsp_section_fast_data_begin,
      .end = (uint32_t) bsp_section_fast_data_end,
      .flags = LPC32XX_MMU_READ_WRITE_DATA
#ifdef LPC32XX_SCRATCH_AREA_SIZE
    }, {
      .begin = (uint32_t) &lpc32xx_scratch_area [0],
      .end = (uint32_t) &lpc32xx_scratch_area [LPC32XX_SCRATCH_AREA_SIZE],
      .flags = LPC32XX_MMU_READ_ONLY_DATA
#endif
    }, {
      .begin = (uint32_t) bsp_section_start_begin,
      .end = (uint32_t) bsp_section_start_end,
      .flags = LPC32XX_MMU_CODE
    }, {
      .begin = (uint32_t) bsp_section_vector_begin,
      .end = (uint32_t) bsp_section_vector_end,
      .flags = LPC32XX_MMU_READ_WRITE_CACHED
    }, {
      .begin = (uint32_t) bsp_section_text_begin,
      .end = (uint32_t) bsp_section_text_end,
      .flags = LPC32XX_MMU_CODE
    }, {
      .begin = (uint32_t) bsp_section_rodata_begin,
      .end = (uint32_t) bsp_section_rodata_end,
      .flags = LPC32XX_MMU_READ_ONLY_DATA
    }, {
      .begin = (uint32_t) bsp_section_data_begin,
      .end = (uint32_t) bsp_section_data_end,
      .flags = LPC32XX_MMU_READ_WRITE_DATA
    }, {
      .begin = (uint32_t) bsp_section_bss_begin,
      .end = (uint32_t) bsp_section_bss_end,
      .flags = LPC32XX_MMU_READ_WRITE_DATA
    }, {
      .begin = (uint32_t) bsp_section_work_begin,
      .end = (uint32_t) bsp_section_work_end,
      .flags = LPC32XX_MMU_READ_WRITE_DATA
    }, {
      .begin = (uint32_t) bsp_section_stack_begin,
      .end = (uint32_t) bsp_section_stack_end,
      .flags = LPC32XX_MMU_READ_WRITE_DATA
    }, {
      .begin = 0x0U,
      .end = 0x100000U,
      .flags = LPC32XX_MMU_READ_ONLY_CACHED
    }, {
      .begin = 0x20000000U,
      .end = 0x200c0000U,
      .flags = LPC32XX_MMU_READ_WRITE
    }, {
      .begin = 0x30000000U,
      .end = 0x32000000U,
      .flags = LPC32XX_MMU_READ_WRITE
    }, {
      .begin = 0x40000000U,
      .end = 0x40100000U,
      .flags = LPC32XX_MMU_READ_WRITE
    }, {
      .begin = (uint32_t) lpc32xx_magic_zero_begin,
      .end = (uint32_t) lpc32xx_magic_zero_end,
      .flags = LPC32XX_MMU_READ_WRITE_DATA
    }
  };
#endif

static BSP_START_TEXT_SECTION void setup_mmu_and_cache(void)
{
  uint32_t ctrl = arm_cp15_start_setup_mmu_and_cache(
    ARM_CP15_CTRL_I | ARM_CP15_CTRL_R | ARM_CP15_CTRL_C
      | ARM_CP15_CTRL_V | ARM_CP15_CTRL_M,
    ARM_CP15_CTRL_S | ARM_CP15_CTRL_A
  );

  arm_cp15_cache_invalidate();

  #ifndef LPC32XX_DISABLE_MMU
    arm_cp15_start_setup_translation_table_and_enable_mmu_and_cache(
      ctrl,
      (uint32_t *) bsp_translation_table_base,
      LPC32XX_MMU_CLIENT_DOMAIN,
      &lpc32xx_mmu_config_table [0],
      RTEMS_ARRAY_SIZE(lpc32xx_mmu_config_table)
    );
  #endif
}

BSP_START_TEXT_SECTION bool lpc32xx_start_pll_setup(
  uint32_t hclkpll_ctrl,
  uint32_t hclkdiv_ctrl,
  bool force
)
{
  uint32_t pwr_ctrl = LPC32XX_PWR_CTRL;
  bool settings_ok =
    ((LPC32XX_HCLKPLL_CTRL ^ hclkpll_ctrl) & BSP_MSK32(1, 16)) == 0
      && ((LPC32XX_HCLKDIV_CTRL ^ hclkdiv_ctrl) & BSP_MSK32(0, 8)) == 0;

  if ((pwr_ctrl & PWR_NORMAL_RUN_MODE) == 0 || (!settings_ok && force)) {
    /* Disable HCLK PLL output */
    LPC32XX_PWR_CTRL = pwr_ctrl & ~PWR_NORMAL_RUN_MODE;

    /* Configure HCLK PLL */
    LPC32XX_HCLKPLL_CTRL = hclkpll_ctrl;
    while ((LPC32XX_HCLKPLL_CTRL & HCLK_PLL_LOCK) == 0) {
      /* Wait */
    }

    /* Setup HCLK divider */
    LPC32XX_HCLKDIV_CTRL = hclkdiv_ctrl;

    /* Enable HCLK PLL output */
    LPC32XX_PWR_CTRL = pwr_ctrl | PWR_NORMAL_RUN_MODE;
  }

  return settings_ok;
}

#if LPC32XX_OSCILLATOR_MAIN != 13000000U
  #error "unexpected main oscillator frequency"
#endif

static BSP_START_TEXT_SECTION void setup_pll(void)
{
  uint32_t hclkpll_ctrl = LPC32XX_HCLKPLL_CTRL_INIT_VALUE;
  uint32_t hclkdiv_ctrl = LPC32XX_HCLKDIV_CTRL_INIT_VALUE;

  lpc32xx_start_pll_setup(hclkpll_ctrl, hclkdiv_ctrl, false);
}

BSP_START_TEXT_SECTION void bsp_start_hook_0(void)
{
  setup_pll();
}

static BSP_START_TEXT_SECTION void stop_dma_activities(void)
{
  #ifdef LPC32XX_STOP_GPDMA
    LPC32XX_DO_STOP_GPDMA;
  #endif

  #ifdef LPC32XX_STOP_ETHERNET
    LPC32XX_DO_STOP_ETHERNET;
  #endif

  #ifdef LPC32XX_STOP_USB
    LPC32XX_DO_STOP_USB;
  #endif
}

static BSP_START_TEXT_SECTION void setup_uarts(void)
{
  LPC32XX_UART_CTRL = 0x0;
  LPC32XX_UART_LOOP = 0x0;

  #ifdef LPC32XX_UART_5_BAUD
    LPC32XX_UARTCLK_CTRL |= 1U << 2;
    LPC32XX_U5CLK = LPC32XX_CONFIG_U5CLK;
    LPC32XX_UART_CLKMODE = BSP_FLD32SET(LPC32XX_UART_CLKMODE, 0x2, 8, 9);
    BSP_CONSOLE_UART_INIT(0x01);
  #endif
}

static BSP_START_TEXT_SECTION void setup_timer(void)
{
  volatile lpc_timer *timer = LPC32XX_STANDARD_TIMER;

  LPC32XX_TIMCLK_CTRL1 = (1U << 2) | (1U << 3);

  timer->tcr = LPC_TIMER_TCR_RST;
  timer->ctcr = 0x0;
  timer->pr = 0x0;
  timer->ir = 0xff;
  timer->mcr = 0x0;
  timer->ccr = 0x0;
  timer->tcr = LPC_TIMER_TCR_EN;
}

BSP_START_TEXT_SECTION void bsp_start_hook_1(void)
{
  stop_dma_activities();
  bsp_start_copy_sections();
  setup_mmu_and_cache();
  setup_uarts();
  setup_timer();
  bsp_start_clear_bss();
}
