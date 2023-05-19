/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC24XX
 *
 * @brief Startup code.
 */

/*
 * Copyright (C) 2008, 2014 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
