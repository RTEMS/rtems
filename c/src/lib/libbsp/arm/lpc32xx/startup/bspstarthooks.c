/**
 * @file
 *
 * @ingroup lpc32xx
 *
 * @brief Startup code.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <stdbool.h>

#include <bspopts.h>
#include <bsp/start.h>
#include <bsp/lpc32xx.h>
#include <bsp/mmu.h>
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

LINKER_SYMBOL(lpc32xx_translation_table_base);

static void BSP_START_TEXT_SECTION clear_bss(void)
{
  const int *end = (const int *) bsp_section_bss_end;
  int *out = (int *) bsp_section_bss_begin;

  /* Clear BSS */
  while (out != end) {
    *out = 0;
    ++out;
  }
}

#ifndef LPC32XX_DISABLE_MMU
  typedef struct {
    uint32_t begin;
    uint32_t end;
    uint32_t flags;
  } lpc32xx_mmu_config;

  static const BSP_START_DATA_SECTION lpc32xx_mmu_config
    lpc32xx_mmu_config_table [] = {
    {
      .begin = (uint32_t) bsp_section_fast_text_begin,
      .end = (uint32_t) bsp_section_fast_text_end,
      .flags = LPC32XX_MMU_CODE
    }, {
      .begin = (uint32_t) bsp_section_fast_data_begin,
      .end = (uint32_t) bsp_section_fast_data_end,
      .flags = LPC32XX_MMU_READ_WRITE_DATA
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

  static void BSP_START_TEXT_SECTION set_translation_table_entries(
    uint32_t *ttb,
    const lpc32xx_mmu_config *config
  )
  {
    uint32_t i = ARM_MMU_SECT_GET_INDEX(config->begin);
    uint32_t iend =
      ARM_MMU_SECT_GET_INDEX(ARM_MMU_SECT_MVA_ALIGN_UP(config->end));

    if (config->begin != config->end) {
      while (i < iend) {
        ttb [i] = (i << ARM_MMU_SECT_BASE_SHIFT) | config->flags;
        ++i;
      }
    }
  }

  static void BSP_START_TEXT_SECTION
    setup_translation_table_and_enable_mmu(uint32_t ctrl)
  {
    uint32_t const dac =
      ARM_CP15_DAC_DOMAIN(LPC32XX_MMU_CLIENT_DOMAIN, ARM_CP15_DAC_CLIENT);
    uint32_t *const ttb = (uint32_t *) lpc32xx_translation_table_base;
    size_t const config_entry_count =
      sizeof(lpc32xx_mmu_config_table) / sizeof(lpc32xx_mmu_config_table [0]);
    size_t i = 0;

    arm_cp15_set_domain_access_control(dac);
    arm_cp15_set_translation_table_base(ttb);

    /* Initialize translation table with invalid entries */
    for (i = 0; i < ARM_MMU_TRANSLATION_TABLE_ENTRY_COUNT; ++i) {
      ttb [i] = 0;
    }

    for (i = 0; i < config_entry_count; ++i) {
      set_translation_table_entries(ttb, &lpc32xx_mmu_config_table [i]);
    }

    /* Enable MMU and cache */
    ctrl |= ARM_CP15_CTRL_I | ARM_CP15_CTRL_C | ARM_CP15_CTRL_M;
    arm_cp15_set_control(ctrl);
  }
#endif

static void BSP_START_TEXT_SECTION setup_mmu_and_cache(void)
{
  uint32_t ctrl = 0;

  /* Disable MMU and cache, basic settings */
  ctrl = arm_cp15_get_control();
  ctrl &= ~(ARM_CP15_CTRL_I | ARM_CP15_CTRL_R | ARM_CP15_CTRL_C
    | ARM_CP15_CTRL_V | ARM_CP15_CTRL_M);
  ctrl |= ARM_CP15_CTRL_S | ARM_CP15_CTRL_A;
  arm_cp15_set_control(ctrl);

  arm_cp15_cache_invalidate();
  arm_cp15_tlb_invalidate();

  #ifndef LPC32XX_DISABLE_MMU
    setup_translation_table_and_enable_mmu(ctrl);
  #endif
}

#if LPC32XX_OSCILLATOR_MAIN != 13000000U
  #error "unexpected main oscillator frequency"
#endif

static void BSP_START_TEXT_SECTION setup_pll(void)
{
  uint32_t pwr_ctrl = LPC32XX_PWR_CTRL;

  if ((pwr_ctrl & PWR_NORMAL_RUN_MODE) == 0) {
    /* Enable HCLK PLL */
    LPC32XX_HCLKPLL_CTRL = HCLK_PLL_POWER | HCLK_PLL_DIRECT | HCLK_PLL_M(16 - 1);
    while ((LPC32XX_HCLKPLL_CTRL & HCLK_PLL_LOCK) == 0) {
      /* Wait */
    }

    /* Setup HCLK divider */
    LPC32XX_HCLKDIV_CTRL = HCLK_DIV_HCLK(2 - 1) | HCLK_DIV_PERIPH_CLK(16 - 1);

    /* Enable HCLK PLL output */
    LPC32XX_PWR_CTRL = pwr_ctrl | PWR_NORMAL_RUN_MODE;
  }
}

void BSP_START_TEXT_SECTION bsp_start_hook_0(void)
{
  setup_pll();
  setup_mmu_and_cache();
}

static void BSP_START_TEXT_SECTION stop_dma_activities(void)
{
  #ifdef LPC32XX_STOP_GPDMA
    if ((LPC32XX_DMACLK_CTRL & 0x1) != 0) {
      if ((lpc32xx.dma.cfg & LPC_DMA_CFG_EN) != 0) {
        int i = 0;

        for (i = 0; i < 8; ++i) {
          lpc32xx.dma.channels [i].cfg = 0;
        }

        lpc32xx.dma.cfg &= ~LPC_DMA_CFG_EN;
      }
      LPC32XX_DMACLK_CTRL = 0;
    }
  #endif

  #ifdef LPC32XX_STOP_ETHERNET
    if ((LPC32XX_MAC_CLK_CTRL & 0x7) == 0x7) {
      lpc32xx.eth.command = 0x38;
      lpc32xx.eth.mac1 = 0xcf00;
      lpc32xx.eth.mac1 = 0;
      LPC32XX_MAC_CLK_CTRL = 0;
    }
  #endif

  #ifdef LPC32XX_STOP_USB
    if ((LPC32XX_USB_CTRL & 0x010e8000) != 0) {
      LPC32XX_OTG_CLK_CTRL = 0;
      LPC32XX_USB_CTRL = 0x80000;
    }
  #endif
}

static void BSP_START_TEXT_SECTION setup_uarts(void)
{
  uint32_t uartclk_ctrl = 0;

  #ifdef LPC32XX_CONFIG_U3CLK
    uartclk_ctrl |= 1U << 0;
    LPC32XX_U3CLK = LPC32XX_CONFIG_U3CLK;
  #endif
  #ifdef LPC32XX_CONFIG_U4CLK
    uartclk_ctrl |= 1U << 1;
    LPC32XX_U4CLK = LPC32XX_CONFIG_U4CLK;
  #endif
  #ifdef LPC32XX_CONFIG_U5CLK
    uartclk_ctrl |= 1U << 2;
    LPC32XX_U5CLK = LPC32XX_CONFIG_U5CLK;
  #endif
  #ifdef LPC32XX_CONFIG_U6CLK
    uartclk_ctrl |= 1U << 3;
    LPC32XX_U6CLK = LPC32XX_CONFIG_U6CLK;
  #endif

  #ifdef LPC32XX_CONFIG_UART_CLKMODE
    LPC32XX_UART_CLKMODE = LPC32XX_CONFIG_UART_CLKMODE;
  #endif

  LPC32XX_UARTCLK_CTRL = uartclk_ctrl;
  LPC32XX_UART_CTRL = 0x0;
  LPC32XX_UART_LOOP = 0x0;

  #ifdef LPC32XX_CONFIG_U5CLK
    /* Clock is already set in LPC32XX_U5CLK */
    BSP_CONSOLE_UART_INIT(0x01);
  #endif
}

static void BSP_START_TEXT_SECTION setup_timer(void)
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

void BSP_START_TEXT_SECTION bsp_start_hook_1(void)
{
  stop_dma_activities();
  setup_uarts();
  setup_timer();

  /* Copy .text section */
  arm_cp15_instruction_cache_invalidate();
  bsp_start_memcpy(
    (int *) bsp_section_text_begin,
    (const int *) bsp_section_text_load_begin,
    (size_t) bsp_section_text_size
  );

  /* Copy .rodata section */
  arm_cp15_instruction_cache_invalidate();
  bsp_start_memcpy(
    (int *) bsp_section_rodata_begin,
    (const int *) bsp_section_rodata_load_begin,
    (size_t) bsp_section_rodata_size
  );

  /* Copy .data section */
  arm_cp15_instruction_cache_invalidate();
  bsp_start_memcpy(
    (int *) bsp_section_data_begin,
    (const int *) bsp_section_data_load_begin,
    (size_t) bsp_section_data_size
  );

  /* Copy .fast_text section */
  arm_cp15_instruction_cache_invalidate();
  bsp_start_memcpy(
    (int *) bsp_section_fast_text_begin,
    (const int *) bsp_section_fast_text_load_begin,
    (size_t) bsp_section_fast_text_size
  );

  /* Copy .fast_data section */
  arm_cp15_instruction_cache_invalidate();
  bsp_start_memcpy(
    (int *) bsp_section_fast_data_begin,
    (const int *) bsp_section_fast_data_load_begin,
    (size_t) bsp_section_fast_data_size
  );

  /* Clear .bss section */
  clear_bss();

  /* At this point we can use objects outside the .start section */
}
