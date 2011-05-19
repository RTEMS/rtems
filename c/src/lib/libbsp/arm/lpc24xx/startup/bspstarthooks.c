/**
 * @file
 *
 * @ingroup lpc24xx
 *
 * @brief Startup code.
 */

/*
 * Copyright (c) 2008-2011 embedded brains GmbH.  All rights reserved.
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

#include <stdbool.h>

#include <bspopts.h>
#include <bsp/start.h>
#include <bsp/linker-symbols.h>
#include <bsp/lpc24xx.h>
#include <bsp/lpc-emc.h>

#if defined(LPC24XX_EMC_MICRON) || defined(LPC24XX_EMC_NUMONYX)
  #define LPC24XX_EMC_INIT
#endif

static volatile lpc_emc *const emc = (lpc_emc *) EMC_BASE_ADDR;

typedef struct {
  uint32_t refresh;
  uint32_t readconfig;
  uint32_t trp;
  uint32_t tras;
  uint32_t tsrex;
  uint32_t tapr;
  uint32_t tdal;
  uint32_t twr;
  uint32_t trc;
  uint32_t trfc;
  uint32_t txsr;
  uint32_t trrd;
  uint32_t tmrd;
} lpc24xx_emc_dynamic_config;

typedef struct {
  uint32_t config;
  uint32_t rascas;
  uint32_t mode;
} lpc24xx_emc_dynamic_chip_config;

typedef struct {
  uint32_t config;
  uint32_t waitwen;
  uint32_t waitoen;
  uint32_t waitrd;
  uint32_t waitpage;
  uint32_t waitwr;
  uint32_t waitrun;
} lpc24xx_emc_static_chip_config;

#ifdef LPC24XX_EMC_MICRON
  static void BSP_START_TEXT_SECTION lpc24xx_ram_test_32(void)
  {
    #ifdef LPC24XX_EMC_TEST
      int *begin = (int *) 0xa0000000;
      const int *end = (const int *) 0xa0800000;
      int *out = begin;

      while (out != end) {
        *out = (int) out;
        ++out;
      }

      out = begin;
      while (out != end) {
        if (*out != (int) out) {
          while (true) {
            /* Do nothing */
          }
        }
        ++out;
      }
    #endif
  }

  static void BSP_START_TEXT_SECTION lpc24xx_cpu_delay(unsigned ticks)
  {
    unsigned i = 0;

    /* One loop execution needs four instructions */
    ticks /= 4;

    for (i = 0; i <= ticks; ++i) {
      __asm__ volatile ("nop");
    }
  }

  static void BSP_START_TEXT_SECTION lpc24xx_udelay(unsigned us)
  {
    lpc24xx_cpu_delay(us * (LPC24XX_CCLK / 1000000));
  }
#endif

static void BSP_START_TEXT_SECTION lpc24xx_init_emc_pinsel(void)
{
  #ifdef LPC24XX_EMC_INIT
    static const BSP_START_DATA_SECTION uint32_t pinsel_5_9 [5] = {
      0x05010115,
      0x55555555,
      0x0,
      0x55555555,
      0x40050155
    };

    bsp_start_memcpy(
      (int *) &PINSEL5,
      (const int *) &pinsel_5_9,
      sizeof(pinsel_5_9)
    );
  #endif
}

static void BSP_START_TEXT_SECTION lpc24xx_init_emc_static(void)
{
  #ifdef LPC24XX_EMC_NUMONYX
    /*
     * Static Memory 1: Numonyx M29W160EB
     *
     * 1 clock cycle = 1/72MHz = 13.9ns
     */
    static const BSP_START_DATA_SECTION lpc24xx_emc_static_chip_config chip_config = {
      /*
       * 16 bit, page mode disabled, active LOW chip select, extended wait
       * disabled, writes not protected, byte lane state LOW/LOW (!).
       */
      .config = 0x81,

      /* 1 clock cycles delay from the chip select 1 to the write enable */
      .waitwen = 0,

      /*
       * 0 clock cycles delay from the chip select 1 or address change
       * (whichever is later) to the output enable
       */
      .waitoen = 0,

      /* 7 clock cycles delay from the chip select 1 to the read access */
      .waitrd = 0x6,

      /*
       * 32 clock cycles delay for asynchronous page mode sequential accesses
       */
      .waitpage = 0x1f,

      /* 5 clock cycles delay from the chip select 1 to the write access */
      .waitwr = 0x3,

      /* 16 bus turnaround cycles */
      .waitrun = 0xf
    };
    lpc24xx_emc_static_chip_config chip_config_on_stack;

    bsp_start_memcpy(
      (int *) &chip_config_on_stack,
      (const int *) &chip_config,
      sizeof(chip_config_on_stack)
    );
    bsp_start_memcpy(
      (int *) EMC_STA_BASE_1,
      (const int *) &chip_config_on_stack,
      sizeof(chip_config_on_stack)
    );
  #endif
}

static void BSP_START_TEXT_SECTION lpc24xx_init_emc_memory_map(void)
{
  #ifdef LPC24XX_EMC_INIT
    /* Use normal memory map */
    EMC_CTRL &= ~0x2U;
  #endif
}

static void BSP_START_TEXT_SECTION lpc24xx_init_emc_dynamic(void)
{
  #ifdef LPC24XX_EMC_MICRON
    /* Dynamic Memory 0: Micron M T48LC 4M16 A2 P 75 IT */

    static const BSP_START_DATA_SECTION lpc24xx_emc_dynamic_config dynamic_config = {
      /* Auto-refresh command every 15.6 us */
      .refresh = 0x46,

      /* Use command delayed strategy */
      .readconfig = 1,

      /* Precharge command period 20 ns */
      .trp = 1,

      /* Active to precharge command period 44 ns */
      .tras = 3,

      /* FIXME */
      .tsrex = 5,

      /* FIXME */
      .tapr = 2,

      /* Data-in to active command period tWR + tRP */
      .tdal = 4,

      /* Write recovery time 15 ns */
      .twr = 1,

      /* Active to active command period 66 ns */
      .trc = 4,

      /* Auto refresh period 66 ns */
      .trfc = 4,

      /* Exit self refresh to active command period 75 ns */
      .txsr = 5,

      /* Active bank a to active bank b command period 15 ns */
      .trrd = 1,

      /* Load mode register to active or refresh command period 2 tCK */
      .tmrd = 1,
    };
    static const BSP_START_DATA_SECTION lpc24xx_emc_dynamic_chip_config chip_config = {
      /*
       * Use SDRAM, 0 0 001 01 address mapping, disabled buffer, unprotected writes
       */
      .config = 0x280,

      .rascas = EMC_DYN_RASCAS_RAS(2) | EMC_DYN_RASCAS_CAS(2, 0),
      .mode = 0xa0000000 | (0x23 << (1 + 2 + 8))
    };

    volatile lpc_emc_dynamic *chip = &emc->dynamic [0];
    uint32_t dynamiccontrol = EMC_DYN_CTRL_CE | EMC_DYN_CTRL_CS;

    /* Check if we need to initialize it */
    if ((chip->config & EMC_DYN_CFG_B) == 0) {
      /*
       * The buffer enable bit is not set.  Now we assume that the controller
       * is not properly initialized.
       */

      /* Global dynamic settings */
      emc->dynamicreadconfig = dynamic_config.readconfig;
      emc->dynamictrp = dynamic_config.trp;
      emc->dynamictras = dynamic_config.tras;
      emc->dynamictsrex = dynamic_config.tsrex;
      emc->dynamictapr = dynamic_config.tapr;
      emc->dynamictdal = dynamic_config.tdal;
      emc->dynamictwr = dynamic_config.twr;
      emc->dynamictrc = dynamic_config.trc;
      emc->dynamictrfc = dynamic_config.trfc;
      emc->dynamictxsr = dynamic_config.txsr;
      emc->dynamictrrd = dynamic_config.trrd;
      emc->dynamictmrd = dynamic_config.tmrd;

      /* Wait 100us after the power is applied and the clocks have stabilized */
      lpc24xx_udelay(100);

      /* NOP period, disable self-refresh */
      emc->dynamiccontrol = dynamiccontrol | EMC_DYN_CTRL_I_NOP;
      lpc24xx_udelay(200);

      /* Precharge all */
      emc->dynamiccontrol = dynamiccontrol | EMC_DYN_CTRL_I_PALL;

      /*
       * Perform several refresh cycles with a memory refresh every 16 AHB
       * clock cycles.  Wait until eight SDRAM refresh cycles have occurred
       * (128 AHB clock cycles).
       */
      emc->dynamicrefresh = 1;
      lpc24xx_cpu_delay(128);

      /* Set refresh period */
      emc->dynamicrefresh = dynamic_config.refresh;

      /* Operational values for the chip */
      chip->rascas = chip_config.rascas;
      chip->config = chip_config.config;

      /* Mode */
      emc->dynamiccontrol = dynamiccontrol | EMC_DYN_CTRL_I_MODE;
      *((volatile uint32_t *) chip_config.mode);

      /* Normal operation */
      emc->dynamiccontrol = 0;

      /* Enable buffer */
      chip->config |= EMC_DYN_CFG_B;

      /* Test RAM */
      lpc24xx_ram_test_32();
    }
  #endif
}

static void BSP_START_TEXT_SECTION lpc24xx_pll_config(
  uint32_t val
)
{
  PLLCON = val;
  PLLFEED = 0xaa;
  PLLFEED = 0x55;
}

/**
 * @brief Sets the Phase Locked Loop (PLL).
 *
 * All parameter values are the actual register field values.
 *
 * @param clksrc Selects the clock source for the PLL.
 *
 * @param nsel Selects PLL pre-divider value (sometimes named psel).
 *
 * @param msel Selects PLL multiplier value.
 *
 * @param cclksel Selects the divide value for creating the CPU clock (CCLK)
 * from the PLL output.
 */
static void BSP_START_TEXT_SECTION lpc24xx_set_pll(
  unsigned clksrc,
  unsigned nsel,
  unsigned msel,
  unsigned cclksel
)
{
  uint32_t pllstat = PLLSTAT;
  uint32_t pllcfg = SET_PLLCFG_NSEL(0, nsel) | SET_PLLCFG_MSEL(0, msel);
  uint32_t clksrcsel = SET_CLKSRCSEL_CLKSRC(0, clksrc);
  uint32_t cclkcfg = SET_CCLKCFG_CCLKSEL(0, cclksel | 1);
  bool pll_enabled = (pllstat & PLLSTAT_PLLE) != 0;

  /* Disconnect PLL if necessary */
  if ((pllstat & PLLSTAT_PLLC) != 0) {
    if (pll_enabled) {
      /* Check if we run already with the desired settings */
      if (PLLCFG == pllcfg && CLKSRCSEL == clksrcsel && CCLKCFG == cclkcfg) {
        /* Nothing to do */
        return;
      }
      lpc24xx_pll_config(PLLCON_PLLE);
    } else {
      lpc24xx_pll_config(0);
    }
  }

  /* Set CPU clock divider to a reasonable save value */
  CCLKCFG = 0;

  /* Disable PLL if necessary */
  if (pll_enabled) {
    lpc24xx_pll_config(0);
  }

  /* Select clock source */
  CLKSRCSEL = clksrcsel;

  /* Set PLL Configuration Register */
  PLLCFG = pllcfg;

  /* Enable PLL */
  lpc24xx_pll_config(PLLCON_PLLE);

  /* Wait for lock */
  while ((PLLSTAT & PLLSTAT_PLOCK) == 0) {
    /* Wait */
  }

  /* Set CPU clock divider and ensure that we have an odd value */
  CCLKCFG = cclkcfg;

  /* Connect PLL */
  lpc24xx_pll_config(PLLCON_PLLE | PLLCON_PLLC);
}

static void BSP_START_TEXT_SECTION lpc24xx_init_pll(void)
{
  /* Enable main oscillator */
  if ((SCS & 0x40) == 0) {
    SCS |= 0x20;
    while ((SCS & 0x40) == 0) {
      /* Wait */
    }
  }

  /* Set PLL */
  #if LPC24XX_OSCILLATOR_MAIN == 12000000U
    lpc24xx_set_pll(1, 0, 11, 3);
  #elif LPC24XX_OSCILLATOR_MAIN == 3686400U
    lpc24xx_set_pll(1, 0, 47, 5);
  #else
    #error "unexpected main oscillator frequency"
  #endif
}

static void BSP_START_TEXT_SECTION lpc24xx_clear_bss(void)
{
  const int *end = (const int *) bsp_section_bss_end;
  int *out = (int *) bsp_section_bss_begin;

  /* Clear BSS */
  while (out != end) {
    *out = 0;
    ++out;
  }
}

void BSP_START_TEXT_SECTION bsp_start_hook_0(void)
{
  lpc24xx_init_pll();
  lpc24xx_init_emc_pinsel();
  lpc24xx_init_emc_static();
}

void BSP_START_TEXT_SECTION bsp_start_hook_1(void)
{
  /* Re-map interrupt vectors to internal RAM */
  MEMMAP = SET_MEMMAP_MAP(MEMMAP, 2);

  /* Fully enable memory accelerator module functions (MAM) */
  MAMCR = 0;
  #if LPC24XX_CCLK <= 20000000U
    MAMTIM = 0x1;
  #elif LPC24XX_CCLK <= 40000000U
    MAMTIM = 0x2;
  #elif LPC24XX_CCLK <= 60000000U
    MAMTIM = 0x3;
  #else
    MAMTIM = 0x4;
  #endif
  MAMCR = 0x2;

  /* Enable fast IO for ports 0 and 1 */
  SCS |= 0x1;

  /* Set fast IO */
  FIO0DIR = 0;
  FIO1DIR = 0;
  FIO2DIR = 0;
  FIO3DIR = 0;
  FIO4DIR = 0;
  FIO0CLR = 0xffffffff;
  FIO1CLR = 0xffffffff;
  FIO2CLR = 0xffffffff;
  FIO3CLR = 0xffffffff;
  FIO4CLR = 0xffffffff;

  lpc24xx_init_emc_memory_map();
  lpc24xx_init_emc_dynamic();

  #ifdef LPC24XX_STOP_GPDMA
    if ((PCONP & PCONP_GPDMA) != 0) {
      GPDMA_CONFIG = 0;
      PCONP &= ~PCONP_GPDMA;
    }
  #endif

  #ifdef LPC24XX_STOP_ETHERNET
    if ((PCONP & PCONP_ETHERNET) != 0) {
      MAC_COMMAND = 0x38;
      MAC_MAC1 = 0xcf00;
      MAC_MAC1 = 0;
      PCONP &= ~PCONP_ETHERNET;
    }
  #endif

  #ifdef LPC24XX_STOP_USB
    if ((PCONP & PCONP_USB) != 0) {
      OTG_CLK_CTRL = 0;
      PCONP &= ~PCONP_USB;
    }
  #endif

  /* Copy .text section */
  bsp_start_memcpy(
    (int *) bsp_section_text_begin,
    (const int *) bsp_section_text_load_begin,
    (size_t) bsp_section_text_size
  );

  /* Copy .rodata section */
  bsp_start_memcpy(
    (int *) bsp_section_rodata_begin,
    (const int *) bsp_section_rodata_load_begin,
    (size_t) bsp_section_rodata_size
  );

  /* Copy .data section */
  bsp_start_memcpy(
    (int *) bsp_section_data_begin,
    (const int *) bsp_section_data_load_begin,
    (size_t) bsp_section_data_size
  );

  /* Copy .fast_text section */
  bsp_start_memcpy(
    (int *) bsp_section_fast_text_begin,
    (const int *) bsp_section_fast_text_load_begin,
    (size_t) bsp_section_fast_text_size
  );

  /* Copy .fast_data section */
  bsp_start_memcpy(
    (int *) bsp_section_fast_data_begin,
    (const int *) bsp_section_fast_data_load_begin,
    (size_t) bsp_section_fast_data_size
  );

  /* Clear .bss section */
  lpc24xx_clear_bss();

  /* At this point we can use objects outside the .start section */
}
