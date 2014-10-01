/**
 * @file
 *
 * @ingroup beagle
 *
 * @brief Startup code.
 */

/*
 * Copyright (c) 2012 Claas Ziemke. All rights reserved.
 *
 *  Claas Ziemke
 *  Kernerstrasse 11
 *  70182 Stuttgart
 *  Germany
 *  <claas.ziemke@gmx.net>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/start.h>
#include <bsp/beagle.h>
#include <bsp/linker-symbols.h>
#include <bsp/uart-output-char.h>
#include <libcpu/arm-cp15.h>

#ifdef BEAGLE_DISABLE_READ_WRITE_DATA_CACHE
  #define BEAGLE_MMU_READ_WRITE_DATA BEAGLE_MMU_READ_WRITE
#else
  #define BEAGLE_MMU_READ_WRITE_DATA BEAGLE_MMU_READ_WRITE_CACHED
#endif

#ifdef BEAGLE_DISABLE_READ_ONLY_PROTECTION
  #define BEAGLE_MMU_READ_ONLY_DATA BEAGLE_MMU_READ_WRITE_CACHED
  #define BEAGLE_MMU_CODE BEAGLE_MMU_READ_WRITE_CACHED
#else
  #define BEAGLE_MMU_READ_ONLY_DATA BEAGLE_MMU_READ_ONLY_CACHED
  #define BEAGLE_MMU_CODE BEAGLE_MMU_READ_ONLY_CACHED
#endif

//LINKER_SYMBOL(beagle_translation_table_base);

static BSP_START_TEXT_SECTION void clear_bss(void)
{
  const int *end = (const int *) bsp_section_bss_end;
  int *out = (int *) bsp_section_bss_begin;

  /* Clear BSS */
  while (out != end) {
    *out = 0;
    ++out;
  }
}

/*#ifndef BEAGLE_DISABLE_MMU
  typedef struct {
    uint32_t begin;
    uint32_t end;
    uint32_t flags;
  } beagle_mmu_config;

  static const BSP_START_DATA_SECTION beagle_mmu_config
    beagle_mmu_config_table [] = {
    {
      .begin = (uint32_t) bsp_section_fast_text_begin,
      .end = (uint32_t) bsp_section_fast_text_end,
      .flags = BEAGLE_MMU_CODE
    }, {
      .begin = (uint32_t) bsp_section_fast_data_begin,
      .end = (uint32_t) bsp_section_fast_data_end,
      .flags = BEAGLE_MMU_READ_WRITE_DATA
#ifdef BEAGLE_SCRATCH_AREA_SIZE
    }, {
      .begin = (uint32_t) &beagle_scratch_area [0],
      .end = (uint32_t) &beagle_scratch_area [BEAGLE_SCRATCH_AREA_SIZE],
      .flags = BEAGLE_MMU_READ_ONLY_DATA
#endif
    }, {
      .begin = (uint32_t) bsp_section_start_begin,
      .end = (uint32_t) bsp_section_start_end,
      .flags = BEAGLE_MMU_CODE
    }, {
      .begin = (uint32_t) bsp_section_vector_begin,
      .end = (uint32_t) bsp_section_vector_end,
      .flags = BEAGLE_MMU_READ_WRITE_CACHED
    }, {
      .begin = (uint32_t) bsp_section_text_begin,
      .end = (uint32_t) bsp_section_text_end,
      .flags = BEAGLE_MMU_CODE
    }, {
      .begin = (uint32_t) bsp_section_rodata_begin,
      .end = (uint32_t) bsp_section_rodata_end,
      .flags = BEAGLE_MMU_READ_ONLY_DATA
    }, {
      .begin = (uint32_t) bsp_section_data_begin,
      .end = (uint32_t) bsp_section_data_end,
      .flags = BEAGLE_MMU_READ_WRITE_DATA
    }, {
      .begin = (uint32_t) bsp_section_bss_begin,
      .end = (uint32_t) bsp_section_bss_end,
      .flags = BEAGLE_MMU_READ_WRITE_DATA
    }, {
      .begin = (uint32_t) bsp_section_work_begin,
      .end = (uint32_t) bsp_section_work_end,
      .flags = BEAGLE_MMU_READ_WRITE_DATA
    }, {
      .begin = (uint32_t) bsp_section_stack_begin,
      .end = (uint32_t) bsp_section_stack_end,
      .flags = BEAGLE_MMU_READ_WRITE_DATA
    }, {
      .begin = 0x0U,
      .end = 0x100000U,
      .flags = BEAGLE_MMU_READ_ONLY_CACHED
    }, {
      .begin = 0x20000000U,
      .end = 0x200c0000U,
      .flags = BEAGLE_MMU_READ_WRITE
    }, {
      .begin = 0x30000000U,
      .end = 0x32000000U,
      .flags = BEAGLE_MMU_READ_WRITE
    }, {
      .begin = 0x40000000U,
      .end = 0x40100000U,
      .flags = BEAGLE_MMU_READ_WRITE
    }, {
      .begin = (uint32_t) beagle_magic_zero_begin,
      .end = (uint32_t) beagle_magic_zero_end,
      .flags = BEAGLE_MMU_READ_WRITE_DATA
    }
  };

  static BSP_START_TEXT_SECTION void set_translation_table_entries(
    uint32_t *ttb,
    const beagle_mmu_config *config
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

  static BSP_START_TEXT_SECTION void
    setup_translation_table_and_enable_mmu(uint32_t ctrl)
  {
    uint32_t const dac =
      ARM_CP15_DAC_DOMAIN(BEAGLE_MMU_CLIENT_DOMAIN, ARM_CP15_DAC_CLIENT);
    uint32_t *const ttb = (uint32_t *) beagle_translation_table_base;
    size_t const config_entry_count =
      sizeof(beagle_mmu_config_table) / sizeof(beagle_mmu_config_table [0]);
    size_t i = 0;

    arm_cp15_set_domain_access_control(dac);
    arm_cp15_set_translation_table_base(ttb);

    // Initialize translation table with invalid entries
    for (i = 0; i < ARM_MMU_TRANSLATION_TABLE_ENTRY_COUNT; ++i) {
      ttb [i] = 0;
    }

    for (i = 0; i < config_entry_count; ++i) {
      set_translation_table_entries(ttb, &beagle_mmu_config_table [i]);
    }

    // Enable MMU and cache
    ctrl |= ARM_CP15_CTRL_I | ARM_CP15_CTRL_C | ARM_CP15_CTRL_M;
    arm_cp15_set_control(ctrl);
  }
#endif*/

/*static BSP_START_TEXT_SECTION void setup_mmu_and_cache(void)
{
  uint32_t ctrl = 0;

  // Disable MMU and cache, basic settings
  ctrl = arm_cp15_get_control();
  ctrl &= ~(ARM_CP15_CTRL_I | ARM_CP15_CTRL_R | ARM_CP15_CTRL_C
    | ARM_CP15_CTRL_V | ARM_CP15_CTRL_M);
  ctrl |= ARM_CP15_CTRL_S | ARM_CP15_CTRL_A;
  arm_cp15_set_control(ctrl);

  arm_cp15_cache_invalidate();
  arm_cp15_tlb_invalidate();

  #ifndef BEAGLE_DISABLE_MMU
    //setup_translation_table_and_enable_mmu(ctrl);
  #endif
}*/

/*BSP_START_TEXT_SECTION bool beagle_start_pll_setup(
  uint32_t hclkpll_ctrl,
  uint32_t hclkdiv_ctrl,
  bool force
)
{
  uint32_t pwr_ctrl = BEAGLE_PWR_CTRL;
  bool settings_ok =
    ((BEAGLE_HCLKPLL_CTRL ^ hclkpll_ctrl) & BSP_MSK32(1, 16)) == 0
      && ((BEAGLE_HCLKDIV_CTRL ^ hclkdiv_ctrl) & BSP_MSK32(0, 8)) == 0;

  if ((pwr_ctrl & PWR_NORMAL_RUN_MODE) == 0 || (!settings_ok && force)) {
    // Disable HCLK PLL output
    BEAGLE_PWR_CTRL = pwr_ctrl & ~PWR_NORMAL_RUN_MODE;

    // Configure HCLK PLL
    BEAGLE_HCLKPLL_CTRL = hclkpll_ctrl;
    while ((BEAGLE_HCLKPLL_CTRL & HCLK_PLL_LOCK) == 0) {
      // Wait
    }

    // Setup HCLK divider
    BEAGLE_HCLKDIV_CTRL = hclkdiv_ctrl;

    // Enable HCLK PLL output
    BEAGLE_PWR_CTRL = pwr_ctrl | PWR_NORMAL_RUN_MODE;
  }

  return settings_ok;
}*/

#if BEAGLE_OSCILLATOR_MAIN != 13000000U
  #error "unexpected main oscillator frequency"
#endif

/*static BSP_START_TEXT_SECTION void setup_pll(void)
{
  uint32_t hclkpll_ctrl = BEAGLE_HCLKPLL_CTRL_INIT_VALUE;
  uint32_t hclkdiv_ctrl = BEAGLE_HCLKDIV_CTRL_INIT_VALUE;

  beagle_start_pll_setup(hclkpll_ctrl, hclkdiv_ctrl, false);
}*/

BSP_START_TEXT_SECTION void bsp_start_hook_0(void)
{
  //setup_pll();
  //setup_mmu_and_cache();
}

/*static BSP_START_TEXT_SECTION void stop_dma_activities(void)
{
  #ifdef BEAGLE_STOP_GPDMA
    BEAGLE_DO_STOP_GPDMA;
  #endif

  #ifdef BEAGLE_STOP_ETHERNET
    BEAGLE_DO_STOP_ETHERNET;
  #endif

  #ifdef BEAGLE_STOP_USB
    BEAGLE_DO_STOP_USB;
  #endif
}*/

static BSP_START_TEXT_SECTION void setup_uarts(void)
{
  uint32_t uartclk_ctrl = 0;

  #ifdef BEAGLE_CONFIG_U3CLK
    uartclk_ctrl |= 1U << 0;
    BEAGLE_U3CLK = BEAGLE_CONFIG_U3CLK;
  #endif
  #ifdef BEAGLE_CONFIG_U4CLK
    uartclk_ctrl |= 1U << 1;
    BEAGLE_U4CLK = BEAGLE_CONFIG_U4CLK;
  #endif
  #ifdef BEAGLE_CONFIG_U5CLK
    uartclk_ctrl |= 1U << 2;
    BEAGLE_U5CLK = BEAGLE_CONFIG_U5CLK;
  #endif
  #ifdef BEAGLE_CONFIG_U6CLK
    uartclk_ctrl |= 1U << 3;
    BEAGLE_U6CLK = BEAGLE_CONFIG_U6CLK;
  #endif

  #ifdef BEAGLE_CONFIG_UART_CLKMODE
    BEAGLE_UART_CLKMODE = BEAGLE_CONFIG_UART_CLKMODE;
  #endif

  BEAGLE_UARTCLK_CTRL = uartclk_ctrl;
  BEAGLE_UART_CTRL = 0x0;
  BEAGLE_UART_LOOP = 0x0;

  #ifdef BEAGLE_CONFIG_U5CLK
    // Clock is already set in BEAGLE_U5CLK
    BSP_CONSOLE_UART_INIT(0x01);
  #endif
}

/*static BSP_START_TEXT_SECTION void setup_timer(void)
{
  volatile beagle_timer *timer = BEAGLE_STANDARD_TIMER;

  BEAGLE_TIMCLK_CTRL1 = (1U << 2) | (1U << 3);

  timer->tcr = BEAGLE_TIMER_TCR_RST;
  timer->ctcr = 0x0;
  timer->pr = 0x0;
  timer->ir = 0xff;
  timer->mcr = 0x0;
  timer->ccr = 0x0;
  timer->tcr = BEAGLE_TIMER_TCR_EN;
}*/

BSP_START_TEXT_SECTION void bsp_start_hook_1(void)
{
  //stop_dma_activities();
  setup_uarts();
  //setup_timer();

  // Copy .text section
  arm_cp15_instruction_cache_invalidate();
  bsp_start_memcpy(
    (int *) bsp_section_text_begin,
    (const int *) bsp_section_text_load_begin,
    (size_t) bsp_section_text_size
  );

  // Copy .rodata section
  arm_cp15_instruction_cache_invalidate();
  bsp_start_memcpy(
    (int *) bsp_section_rodata_begin,
    (const int *) bsp_section_rodata_load_begin,
    (size_t) bsp_section_rodata_size
  );

  // Copy .data section
  arm_cp15_instruction_cache_invalidate();
  bsp_start_memcpy(
    (int *) bsp_section_data_begin,
    (const int *) bsp_section_data_load_begin,
    (size_t) bsp_section_data_size
  );

  // Copy .fast_text section
  arm_cp15_instruction_cache_invalidate();
  bsp_start_memcpy(
    (int *) bsp_section_fast_text_begin,
    (const int *) bsp_section_fast_text_load_begin,
    (size_t) bsp_section_fast_text_size
  );

  // Copy .fast_data section
  arm_cp15_instruction_cache_invalidate();
  bsp_start_memcpy(
    (int *) bsp_section_fast_data_begin,
    (const int *) bsp_section_fast_data_load_begin,
    (size_t) bsp_section_fast_data_size
  );

  // Clear .bss section
  clear_bss();



  // At this point we can use objects outside the .start section
}
