/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC24XX
 *
 * @brief Startup code.
 */

/*
 * Copyright (c) 2008-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/dma.h>
#include <bsp/io.h>
#include <bsp/irq-generic.h>
#include <bsp/irq.h>
#include <bsp/linker-symbols.h>
#include <bsp/lpc24xx.h>
#include <bsp/system-clocks.h>
#include <bsp/uart-output-char.h>

#ifdef LPC24XX_HEAP_EXTEND
  LINKER_SYMBOL(lpc24xx_region_heap_0_begin);
  LINKER_SYMBOL(lpc24xx_region_heap_0_size);
  LINKER_SYMBOL(lpc24xx_region_heap_0_end);

  LINKER_SYMBOL(lpc24xx_region_heap_1_begin);
  LINKER_SYMBOL(lpc24xx_region_heap_1_size);
  LINKER_SYMBOL(lpc24xx_region_heap_1_end);

  extern Heap_Control *RTEMS_Malloc_Heap;
#endif

static void heap_extend(void)
{
  #ifdef LPC24XX_HEAP_EXTEND
    _Heap_Extend(
      RTEMS_Malloc_Heap,
      lpc24xx_region_heap_0_begin,
      (uintptr_t) lpc24xx_region_heap_0_size,
      0
    );
    _Heap_Extend(
      RTEMS_Malloc_Heap,
      lpc24xx_region_heap_1_begin,
      (uintptr_t) lpc24xx_region_heap_1_size,
      0
    );
  #endif
}

static void initialize_console(void)
{
  #ifdef LPC24XX_CONFIG_CONSOLE
    static const lpc24xx_pin_range pins [] = {
      LPC24XX_PIN_UART_0_TXD,
      LPC24XX_PIN_UART_0_RXD,
      LPC24XX_PIN_TERMINAL
    };

    lpc24xx_module_enable(LPC24XX_MODULE_UART_0, LPC24XX_MODULE_PCLK_DEFAULT);
    lpc24xx_pin_config(&pins [0], LPC24XX_PIN_SET_FUNCTION);
    BSP_CONSOLE_UART_INIT(LPC24XX_PCLK / 16 / LPC24XX_UART_BAUD);
  #endif
}

void bsp_start(void)
{
  /* Initialize Timer 1 */
  lpc24xx_module_enable(LPC24XX_MODULE_TIMER_1, LPC24XX_MODULE_PCLK_DEFAULT);

  /* Initialize standard timer */
  lpc24xx_timer_initialize();

  initialize_console();

  /* Interrupts */
  bsp_interrupt_initialize();

  /* DMA */
  lpc24xx_dma_initialize();

  heap_extend();
}
