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

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/dma.h>
#include <bsp/io.h>
#include <bsp/irq-generic.h>
#include <bsp/irq.h>
#include <bsp/linker-symbols.h>
#include <bsp/lpc24xx.h>
#include <bsp/stackalloc.h>
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

void bsp_pretasking_hook(void)
{
  #ifdef LPC24XX_HEAP_EXTEND
    _Heap_Extend(
      RTEMS_Malloc_Heap,
      lpc24xx_region_heap_0_begin,
      (uintptr_t) lpc24xx_region_heap_0_size,
      NULL
    );
    _Heap_Extend(
      RTEMS_Malloc_Heap,
      lpc24xx_region_heap_1_begin,
      (uintptr_t) lpc24xx_region_heap_1_size,
      NULL
    );
  #endif
}

void bsp_start(void)
{
  /* Initialize Timer 1 */
  lpc24xx_module_enable(LPC24XX_MODULE_TIMER_1, LPC24XX_MODULE_CCLK);

  /* Initialize standard timer */
  lpc24xx_timer_initialize();

  /* Initialize console */
  #ifdef LPC24XX_CONFIG_CONSOLE
    lpc24xx_module_enable(LPC24XX_MODULE_UART_0, LPC24XX_MODULE_CCLK);
    lpc24xx_io_config(LPC24XX_MODULE_UART_0, LPC24XX_CONFIG_CONSOLE);
    BSP_CONSOLE_UART_INIT(lpc24xx_cclk() / 16 / LPC24XX_UART_BAUD);
  #endif

  /* Interrupts */
  if (bsp_interrupt_initialize() != RTEMS_SUCCESSFUL) {
    _CPU_Fatal_halt(0xe);
  }

  /* DMA */
  lpc24xx_dma_initialize();

  /* Task stacks */
  #ifdef LPC24XX_SPECIAL_TASK_STACKS_SUPPORT
    bsp_stack_initialize(
      bsp_section_stack_begin,
      (uintptr_t) bsp_section_stack_size
    );
  #endif

  /* UART configurations */
  #ifdef LPC24XX_CONFIG_UART_1
    lpc24xx_module_enable(LPC24XX_MODULE_UART_1, LPC24XX_MODULE_CCLK);
    lpc24xx_io_config(LPC24XX_MODULE_UART_1, LPC24XX_CONFIG_UART_1);
  #endif
  #ifdef LPC24XX_CONFIG_UART_2
    lpc24xx_module_enable(LPC24XX_MODULE_UART_2, LPC24XX_MODULE_CCLK);
    lpc24xx_io_config(LPC24XX_MODULE_UART_2, LPC24XX_CONFIG_UART_2);
  #endif
  #ifdef LPC24XX_CONFIG_UART_3
    lpc24xx_module_enable(LPC24XX_MODULE_UART_3, LPC24XX_MODULE_CCLK);
    lpc24xx_io_config(LPC24XX_MODULE_UART_3, LPC24XX_CONFIG_UART_3);
  #endif
}
