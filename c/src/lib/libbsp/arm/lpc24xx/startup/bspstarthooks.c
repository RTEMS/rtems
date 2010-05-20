/**
 * @file
 *
 * @ingroup lpc24xx
 *
 * @brief Startup code.
 */

/*
 * Copyright (c) 2008, 2009
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
#include <bsp/lpc24xx.h>
#include <bsp/linker-symbols.h>

#if defined(LPC24XX_EMC_MICRON) || defined(LPC24XX_EMC_NUMONYX)
  #define LPC24XX_EMC_INIT
#endif

#ifdef LPC24XX_EMC_MICRON
  static void BSP_START_SECTION lpc24xx_ram_test_32(void)
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

  static void BSP_START_SECTION lpc24xx_cpu_delay(
    unsigned ticks
  )
  {
    unsigned i = 0;

    /* One loop execution needs four instructions */
    ticks /= 4;

    for (i = 0; i <= ticks; ++i) {
      asm volatile ("nop");
    }
  }
#endif

/**
 * @brief EMC initialization hook 0.
 */
static void BSP_START_SECTION lpc24xx_init_emc_0(void)
{
  #ifdef LPC24XX_EMC_NUMONYX
    /*
     * Static Memory 1: Numonyx M29W160EB
     *
     * 1 clock cycle = 1/72MHz = 13.9ns
     *
     * We cannot use an initializer since this will result in the usage of the
     * read-only data section which is not available here.
     */
    lpc24xx_emc_static numonyx;

    /*
     * 16 bit, page mode disabled, active LOW chip select, extended wait
     * disabled, writes not protected, byte lane state LOW/LOW (!).
     */
    numonyx.cfg = 0x81;

    /* 1 clock cycles delay from the chip select 1 to the write enable */
    numonyx.waitwen = 0;

    /*
     * 0 clock cycles delay from the chip select 1 or address change
     * (whichever is later) to the output enable
     */
    numonyx.waitoen = 0;

    /* 7 clock cycles delay from the chip select 1 to the read access */
    numonyx.waitrd = 0x6;

    /*
     * 32 clock cycles delay for asynchronous page mode sequential accesses
     */
    numonyx.waitpage = 0x1f;

    /* 5 clock cycles delay from the chip select 1 to the write access */
    numonyx.waitwr = 0x3;

    /* 16 bus turnaround cycles */
    numonyx.waitrun = 0xf;
  #endif

  #ifdef LPC24XX_EMC_INIT
    /* Set pin functions for EMC */
    PINSEL5 = (PINSEL5 & 0xf000f000) | 0x05550555;
    PINSEL6 = 0x55555555;
    PINSEL8 = 0x55555555;
    PINSEL9 = (PINSEL9 & 0x0f000000) | 0x50555555;
  #endif

  #ifdef LPC24XX_EMC_NUMONYX
    /* Static Memory 1 settings */
    bsp_start_memcpy(
      (int *) EMC_STA_BASE_1,
      (const int *) &numonyx,
      sizeof(numonyx)
    );
  #endif
}

/**
 * @brief EMC initialization hook 1.
 */
static void BSP_START_SECTION lpc24xx_init_emc_1(void)
{
  #ifdef LPC24XX_EMC_INIT
    /* Use normal memory map */
    EMC_CTRL &= ~0x2U;
  #endif

  #ifdef LPC24XX_EMC_MICRON
    /* Check if we need to initialize it */
    if ((EMC_DYN_CFG0 & 0x00080000) == 0) {
      /*
       * The buffer enable bit is not set.  Now we assume that the controller
       * is not properly initialized.
       */

      /* Global dynamic settings */

      /* FIXME */
      EMC_DYN_APR = 2;

      /* Data-in to active command period tWR + tRP */
      EMC_DYN_DAL = 4;

      /* Load mode register to active or refresh command period 2 tCK */
      EMC_DYN_MRD = 1;

      /* Active to precharge command period 44 ns */
      EMC_DYN_RAS = 3;

      /* Active to active command period 66 ns */
      EMC_DYN_RC = 4;

      /* Use command delayed strategy */
      EMC_DYN_RD_CFG = 1;

      /* Auto refresh period 66 ns */
      EMC_DYN_RFC = 4;

      /* Precharge command period 20 ns */
      EMC_DYN_RP = 1;

      /* Active bank a to active bank b command period 15 ns */
      EMC_DYN_RRD = 1;

      /* FIXME */
      EMC_DYN_SREX = 5;

      /* Write recovery time 15 ns */
      EMC_DYN_WR = 1;

      /* Exit self refresh to active command period 75 ns */
      EMC_DYN_XSR = 5;

      /* Dynamic Memory 0: Micron M T48LC 4M16 A2 P 75 IT */

      /*
       * Use SDRAM, 0 0 001 01 address mapping, disabled buffer, unprotected writes
       */
      EMC_DYN_CFG0 = 0x0280;

      /* CAS and RAS latency */
      EMC_DYN_RASCAS0 = 0x0202;

      /* Wait 50 micro seconds */
      lpc24xx_cpu_delay(3600);

      /* Send command: NOP */
      EMC_DYN_CTRL = EMC_DYN_CTRL_CE | EMC_DYN_CTRL_CS | EMC_DYN_CTRL_CMD_NOP;

      /* Wait 50 micro seconds */
      lpc24xx_cpu_delay(3600);

      /* Send command: PRECHARGE ALL */
      EMC_DYN_CTRL = EMC_DYN_CTRL_CE | EMC_DYN_CTRL_CS | EMC_DYN_CTRL_CMD_PALL;

      /* Shortest possible refresh period */
      EMC_DYN_RFSH = 0x01;

      /* Wait at least 128 AHB clock cycles */
      lpc24xx_cpu_delay(128);

      /* Wait 1 micro second */
      lpc24xx_cpu_delay(72);

      /* Set refresh period */
      EMC_DYN_RFSH = 0x46;

      /* Send command: MODE */
      EMC_DYN_CTRL = EMC_DYN_CTRL_CE | EMC_DYN_CTRL_CS | EMC_DYN_CTRL_CMD_MODE;

      /* Set mode register in SDRAM */
      *((volatile uint32_t *) (0xa0000000 | (0x23 << (1 + 2 + 8))));

      /* Send command: NORMAL */
      EMC_DYN_CTRL = 0;

      /* Enable buffer */
      EMC_DYN_CFG0 |= 0x00080000;

      /* Test RAM */
      lpc24xx_ram_test_32();
    }
  #endif
}

static void BSP_START_SECTION lpc24xx_pll_config(
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
static void BSP_START_SECTION lpc24xx_set_pll(
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

static void BSP_START_SECTION lpc24xx_init_pll(void)
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

static void BSP_START_SECTION lpc24xx_clear_bss(void)
{
  const int *end = (const int *) bsp_section_bss_end;
  int *out = (int *) bsp_section_bss_begin;

  /* Clear BSS */
  while (out != end) {
    *out = 0;
    ++out;
  }
}

void BSP_START_SECTION bsp_start_hook_0(void)
{
  /* Initialize PLL */
  lpc24xx_init_pll();

  /* Initialize EMC hook 0 */
  lpc24xx_init_emc_0();
}

void BSP_START_SECTION bsp_start_hook_1(void)
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

  /* Initialize EMC hook 1 */
  lpc24xx_init_emc_1();

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

  /* Copy .fast section */
  bsp_start_memcpy(
    (int *) bsp_section_fast_begin,
    (const int *) bsp_section_fast_load_begin,
    (size_t) bsp_section_fast_size
  );

  /* Clear .bss section */
  lpc24xx_clear_bss();

  /* At this point we can use objects outside the .start section */
}
