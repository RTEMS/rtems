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

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>
#include <bsp/irq.h>
#include <bsp/linker-symbols.h>
#include <bsp/stackalloc.h>
#include <bsp/lpc32xx.h>

/* FIXME */
#define CONSOLE_RBR (*(volatile uint32_t *) (LPC32XX_BASE_UART_5 + 0x00))
#define CONSOLE_THR (*(volatile uint32_t *) (LPC32XX_BASE_UART_5 + 0x00))
#define CONSOLE_DLL (*(volatile uint32_t *) (LPC32XX_BASE_UART_5 + 0x00))
#define CONSOLE_DLM (*(volatile uint32_t *) (LPC32XX_BASE_UART_5 + 0x04))
#define CONSOLE_IER (*(volatile uint32_t *) (LPC32XX_BASE_UART_5 + 0x04))
#define CONSOLE_IIR (*(volatile uint32_t *) (LPC32XX_BASE_UART_5 + 0x08))
#define CONSOLE_FCR (*(volatile uint32_t *) (LPC32XX_BASE_UART_5 + 0x08))
#define CONSOLE_LCR (*(volatile uint32_t *) (LPC32XX_BASE_UART_5 + 0x0C))
#define CONSOLE_LSR (*(volatile uint32_t *) (LPC32XX_BASE_UART_5 + 0x14))
#define CONSOLE_SCR (*(volatile uint32_t *) (LPC32XX_BASE_UART_5 + 0x1C))
#define CONSOLE_ACR (*(volatile uint32_t *) (LPC32XX_BASE_UART_5 + 0x20))
#define CONSOLE_ICR (*(volatile uint32_t *) (LPC32XX_BASE_UART_5 + 0x24))
#define CONSOLE_FDR (*(volatile uint32_t *) (LPC32XX_BASE_UART_5 + 0x28))
#define CONSOLE_TER (*(volatile uint32_t *) (LPC32XX_BASE_UART_5 + 0x30))

void bsp_start(void)
{
  #ifdef LPC32XX_CONFIG_U3CLK
    LPC32XX_U3CLK = LPC32XX_CONFIG_U3CLK;
  #endif
  #ifdef LPC32XX_CONFIG_U4CLK
    LPC32XX_U4CLK = LPC32XX_CONFIG_U4CLK;
  #endif
  #ifdef LPC32XX_CONFIG_U5CLK
    LPC32XX_U5CLK = LPC32XX_CONFIG_U5CLK;
  #endif
  #ifdef LPC32XX_CONFIG_U6CLK
    LPC32XX_U6CLK = LPC32XX_CONFIG_U6CLK;
  #endif

  #ifdef LPC32XX_CONFIG_UART_CLKMODE
    LPC32XX_UART_CLKMODE = LPC32XX_CONFIG_UART_CLKMODE;
  #endif

  LPC32XX_UART_CTRL = 0x0;
  LPC32XX_UART_LOOP = 0x0;

  /* FIXME */
  CONSOLE_LCR = 0x0;
  CONSOLE_IER = 0x0;
  CONSOLE_LCR = 0x80;
  CONSOLE_DLL = 0x1; /* Clock is already set in LPC32XX_U5CLK */
  CONSOLE_DLM = 0x0;
  CONSOLE_LCR = 0x3;
  CONSOLE_FCR = 0x7;

#if 0
  /* FIXME */
  printk("LPC32XX_U3CLK %08x\n", LPC32XX_U3CLK);
  printk("LPC32XX_U4CLK %08x\n", LPC32XX_U4CLK);
  printk("LPC32XX_U5CLK %08x\n", LPC32XX_U5CLK);
  printk("LPC32XX_U6CLK %08x\n", LPC32XX_U6CLK);
  printk("LPC32XX_IRDACLK %08x\n", LPC32XX_IRDACLK);
  printk("LPC32XX_UART_CTRL %08x\n", LPC32XX_UART_CTRL);
  printk("LPC32XX_UART_CLKMODE %08x\n", LPC32XX_UART_CLKMODE);
  printk("LPC32XX_UART_LOOP %08x\n", LPC32XX_UART_LOOP);
#endif

  /* Interrupts */
  if (bsp_interrupt_initialize() != RTEMS_SUCCESSFUL) {
    _CPU_Fatal_halt(0xe);
  }

  /* Task stacks */
  bsp_stack_initialize(
    bsp_section_stack_begin,
    (uintptr_t) bsp_section_stack_size
  );
}

#define UART_LSR_THRE 0x00000020U

static void lpc32xx_console_wait(void)
{
  while ((CONSOLE_LSR & UART_LSR_THRE) == 0) {
    /* Wait */
  }
}

static void lpc32xx_BSP_output_char(char c)
{
  lpc32xx_console_wait();

  CONSOLE_THR = c;

  if (c == '\n') {
    lpc32xx_console_wait();

    CONSOLE_THR = '\r';
  }
}

BSP_output_char_function_type BSP_output_char = lpc32xx_BSP_output_char;
