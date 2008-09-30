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
#include <bsp/irq.h>
#include <bsp/linker-symbols.h>
#include <bsp/lpc24xx.h>
#include <bsp/start.h>
#include <bsp/system-clocks.h>

void bsp_start_hook_0( void)
{
  /* Re-map interrupt vectors to internal RAM */
  SET_MEMMAP_MAP( MEMMAP, 2);
}

void bsp_start_hook_1( void)
{
  unsigned zero = 0;
  unsigned *out = bsp_section_bss_start;

  /* Clear BSS */
  while (out < bsp_section_bss_end) {
    *out = zero;
    ++out;
  }
}

void bsp_start( void)
{
  printk( "CPU Clock: %u\n", lpc24xx_cclk());

  /* Exceptions */
  rtems_exception_init_mngt();

  /* Interrupts */
  if (bsp_interrupt_initialize() != RTEMS_SUCCESSFUL) {
    /* FIXME */
    printk( "Cannot intitialize interrupt support\n");
    while (1) {
      /* Spin forever */
    }
  }

  /* DMA */
  lpc24xx_dma_initialize();
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
