/**
 * @file
 *
 * @ingroup lpc24xx
 *
 * @brief Startup code.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#include <string.h>

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/dma.h>
#include <bsp/io.h>
#include <bsp/irq-generic.h>
#include <bsp/irq.h>
#include <bsp/linker-symbols.h>
#include <bsp/lpc24xx.h>
#include <bsp/stackalloc.h>
#include <bsp/start.h>
#include <bsp/system-clocks.h>

static void lpc24xx_fatal_error( void)
{
  while (true) {
    /* Spin forever */
  }
}

static void lpc24xx_ram_test_32( void)
{
  const unsigned *end = (const unsigned *) bsp_region_data_end;
  unsigned *begin = (unsigned *) bsp_region_data_begin;
  unsigned *out = begin;

  while (out != end) {
    *out = (unsigned) out;
    ++out;
  }

  out = begin;
  while (out != end) {
    if (*out != (unsigned) out) {
      lpc24xx_fatal_error();
    }
    ++out;
  }
}

/**
 * @brief EMC initialization.
 *
 * Dynamic Memory 0: Micron M T48LC 4M16 A2 P 75 IT
 */
static void lpc24xx_init_emc( void)
{
  #ifdef LPC24XX_EMC_MICRON
    int i = 0;
    uint32_t mode = 0;

    /* Enable module power */
    lpc24xx_module_enable( LPC24XX_MODULE_EMC, 0, LPC24XX_MODULE_PCLK_DEFAULT);

    /* IO configuration */
    lpc24xx_io_config( LPC24XX_MODULE_EMC, 0, 0);

    /* Enable module, normal memory map and normal power mode */
    EMC_CTRL = 1;

    /* Use little-endian mode and 1:1 clock ratio */
    EMC_CONFIG = 0;

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

    /* Dynamic Memory 0 settings */

    /*
     * Use SDRAM, 0 0 001 01 address mapping, disabled buffer, unprotected writes
     */
    EMC_DYN_CFG0 = 0x0280;

    /* CAS and RAS latency */
    EMC_DYN_RASCAS0 = 0x0202;

    /* Wait 50 micro seconds */
    lpc24xx_micro_seconds_delay( 50);

    /* Send command: NOP */
    EMC_DYN_CTRL = EMC_DYN_CTRL_CE | EMC_DYN_CTRL_CS | EMC_DYN_CTRL_CMD_NOP;

    /* Wait 50 micro seconds */
    lpc24xx_micro_seconds_delay( 50);

    /* Send command: PRECHARGE ALL */
    EMC_DYN_CTRL = EMC_DYN_CTRL_CE | EMC_DYN_CTRL_CS | EMC_DYN_CTRL_CMD_PALL;

    /* Shortest possible refresh period */
    EMC_DYN_RFSH = 0x01;

    /* Wait at least 128 ABH clock cycles */
    for (i = 0; i < 128; ++i) {
      asm volatile (" nop");
    }

    /* Wait 1 micro second */
    lpc24xx_micro_seconds_delay( 1);

    /* Set refresh period */
    EMC_DYN_RFSH = 0x46;

    /* Send command: MODE */
    EMC_DYN_CTRL = EMC_DYN_CTRL_CE | EMC_DYN_CTRL_CS | EMC_DYN_CTRL_CMD_MODE;

    /* Set mode registerin SDRAM */
    mode = *((volatile uint32_t *) (0xa0000000 | (0x23 << (1 + 2 + 8))));

    /* Send command: NORMAL */
    EMC_DYN_CTRL = 0;

    /* Enable buffer */
    EMC_DYN_CFG0 |= 0x00080000;

    /* Extended wait register */
    EMC_STA_EXT_WAIT = 0;

    /* Static Memory 1 settings */
    EMC_STA_WAITWEN1 = 0x02;
    EMC_STA_WAITOEN1 = 0x02;
    EMC_STA_WAITRD1 = 0x08;
    EMC_STA_WAITPAGE1 = 0x1f;
    EMC_STA_WAITWR1 = 0x08;
    EMC_STA_WAITTURN1 = 0x0f;
    EMC_STA_CFG1 = 0x81;

    /* RAM test */
    lpc24xx_ram_test_32();
  #endif
}

static void lpc24xx_init_pll( void)
{
  #ifndef LPC24XX_HAS_UBOOT
    /* Enable main oscillator */
    SCS = SET_FLAG( SCS, 0x20);
    while (IS_FLAG_CLEARED( SCS, 0x40)) {
      /* Wait */
    }

    /* Set PLL */
    lpc24xx_set_pll( 1, 0, 11, 3);
  #endif
}

void /* __attribute__ ((section (".entry"))) */ bsp_start_hook_0( void)
{
  /* Initialize PLL */
  lpc24xx_init_pll();

  #ifndef LPC24XX_HAS_UBOOT
    /* Set pin functions  */
    PINSEL0 = 0;
    PINSEL1 = 0;
    PINSEL2 = 0;
    PINSEL3 = 0;
    PINSEL4 = 0;
    PINSEL5 = 0;
    PINSEL6 = 0;
    PINSEL7 = 0;
    PINSEL8 = 0;
    PINSEL9 = 0;
    PINSEL10 = 0;
    PINSEL11 = 0;

    /* Set pin modes  */
    PINMODE0 = 0;
    PINMODE1 = 0;
    PINMODE2 = 0;
    PINMODE3 = 0;
    PINMODE4 = 0;
    PINMODE5 = 0;
    PINMODE6 = 0;
    PINMODE7 = 0;
    PINMODE8 = 0;
    PINMODE9 = 0;

    /* Set periperal clocks */
    PCLKSEL0 = 0;
    PCLKSEL1 = 0;

    /* Disable power for all modules */
    PCONP = 0;

    /* Set memory accelerator module (MAM) */
    MAMCR = 0;
    MAMTIM = 4;

    /* Enable fast IO for ports 0 and 1 */
    SCS = SET_FLAG( SCS, 0x1);

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

    /* Initialize console */
    #ifdef LPC24XX_CONFIG_CONSOLE
      lpc24xx_module_enable( LPC24XX_MODULE_UART, 0, LPC24XX_MODULE_CCLK);
      lpc24xx_io_config( LPC24XX_MODULE_UART, 0, LPC24XX_CONFIG_CONSOLE);
      U0LCR = 0;
      U0IER = 0;
      U0LCR = 0x80;
      U0DLL = lpc24xx_cclk() / 16 / LPC24XX_UART_BAUD;
      U0DLM = 0;
      U0LCR = 0x03;
      U0FCR = 0x07;
    #endif

    /* Initialize Timer 1 */
    lpc24xx_module_enable( LPC24XX_MODULE_TIMER, 1, LPC24XX_MODULE_CCLK);
  #endif
}

static void lpc24xx_copy_data( void)
{
  #ifndef LPC24XX_HAS_UBOOT
    const unsigned *end = (const unsigned *) bsp_section_data_end;
    unsigned *in = (unsigned *) bsp_section_data_load_begin;
    unsigned *out = (unsigned *) bsp_section_data_begin;

    /* Copy data */
    while (out != end) {
      *out = *in;
      ++out;
      ++in;
    }
  #endif
}

static void lpc24xx_clear_bss( void)
{
  const unsigned *end = (const unsigned *) bsp_section_bss_end;
  unsigned *out = (unsigned *) bsp_section_bss_begin;

  /* Clear BSS */
  while (out != end) {
    *out = 0;
    ++out;
  }
}

void /* __attribute__ ((section (".entry"))) */ bsp_start_hook_1( void)
{
  /* Re-map interrupt vectors to internal RAM */
  MEMMAP = SET_MEMMAP_MAP( MEMMAP, 2);

  /* Initialize External Memory Controller (EMC) */
  lpc24xx_init_emc();

  /* Copy data */
  lpc24xx_copy_data();

  /* Clear BSS */
  lpc24xx_clear_bss();
}

void bsp_start( void)
{
  printk( "CPU clock (CCLK): %u\n", lpc24xx_cclk());

  /* Exceptions */
  /* FIXME
  rtems_exception_init_mngt();
  */

  /* Interrupts */
  if (bsp_interrupt_initialize() != RTEMS_SUCCESSFUL) {
    /* FIXME */
    printk( "cannot intitialize interrupt support\n");
    lpc24xx_fatal_error();
  }

  /* DMA */
  lpc24xx_dma_initialize();

  /* Task stacks */
  bsp_stack_initialize(
    bsp_section_stack_begin,
    (intptr_t) bsp_section_stack_size
  );

  /* UART configurations */
  #ifdef LPC24XX_CONFIG_UART_1
    lpc24xx_module_enable( LPC24XX_MODULE_UART, 1, LPC24XX_MODULE_CCLK);
    lpc24xx_io_config( LPC24XX_MODULE_UART, 1, LPC24XX_CONFIG_UART_1);
  #endif
  #ifdef LPC24XX_CONFIG_UART_2
    lpc24xx_module_enable( LPC24XX_MODULE_UART, 2, LPC24XX_MODULE_CCLK);
    lpc24xx_io_config( LPC24XX_MODULE_UART, 2, LPC24XX_CONFIG_UART_2);
  #endif
  #ifdef LPC24XX_CONFIG_UART_3
    lpc24xx_module_enable( LPC24XX_MODULE_UART, 3, LPC24XX_MODULE_CCLK);
    lpc24xx_io_config( LPC24XX_MODULE_UART, 3, LPC24XX_CONFIG_UART_3);
  #endif
}

#define ULSR_THRE 0x00000020U

static void lpc24xx_BSP_output_char( char c)
{
  while (IS_FLAG_CLEARED( U0LSR, ULSR_THRE)) {
    /* Wait */
  }
  U0THR = c;

  if (c == '\n') {
    while (IS_FLAG_CLEARED( U0LSR, ULSR_THRE)) {
      /* Wait */
    }
    U0THR = '\r';
  }
}

BSP_output_char_function_type BSP_output_char = lpc24xx_BSP_output_char;
