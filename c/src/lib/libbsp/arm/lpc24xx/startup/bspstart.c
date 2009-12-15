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
    U0LCR = 0;
    U0IER = 0;
    U0LCR = 0x80;
    U0DLL = lpc24xx_cclk() / 16 / LPC24XX_UART_BAUD;
    U0DLM = 0;
    U0LCR = 0x03;
    U0FCR = 0x07;
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

#define ULSR_THRE 0x00000020U

static void lpc24xx_BSP_output_char(char c)
{
  while (IS_FLAG_CLEARED(U0LSR, ULSR_THRE)) {
    /* Wait */
  }
  U0THR = c;

  if (c == '\n') {
    while (IS_FLAG_CLEARED(U0LSR, ULSR_THRE)) {
      /* Wait */
    }
    U0THR = '\r';
  }
}

BSP_output_char_function_type BSP_output_char = lpc24xx_BSP_output_char;
