/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC32XX
 *
 * @brief Console configuration.
 */

/*
 * Copyright (c) 2009-2014 embedded brains GmbH.  All rights reserved.
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

#include <libchip/ns16550.h>

#include <bsp.h>
#include <bsp/lpc32xx.h>
#include <bsp/irq.h>
#include <bsp/hsu.h>
#include <bsp/console-termios.h>

static uint8_t lpc32xx_uart_get_register(uintptr_t addr, uint8_t i)
{
  volatile uint32_t *reg = (volatile uint32_t *) addr;

  return (uint8_t) reg [i];
}

static void lpc32xx_uart_set_register(uintptr_t addr, uint8_t i, uint8_t val)
{
  volatile uint32_t *reg = (volatile uint32_t *) addr;

  reg [i] = val;
}

#ifdef LPC32XX_UART_3_BAUD
  static bool lpc32xx_uart_probe_3(rtems_termios_device_context *context)
  {
    LPC32XX_UARTCLK_CTRL |= BSP_BIT32(0);
    LPC32XX_U3CLK = LPC32XX_CONFIG_U3CLK;
    LPC32XX_UART_CLKMODE = BSP_FLD32SET(LPC32XX_UART_CLKMODE, 0x2, 4, 5);

    return ns16550_probe(context);
  }
#endif

#ifdef LPC32XX_UART_4_BAUD
  static bool lpc32xx_uart_probe_4(rtems_termios_device_context *context)
  {
    volatile lpc32xx_gpio *gpio = &lpc32xx.gpio;

    /*
     * Set GPO_21/U4_TX/LCDVD[3] to U4_TX.  This works only if LCD module is
     * disabled.
     */
    gpio->p2_mux_set = BSP_BIT32(2);

    LPC32XX_UARTCLK_CTRL |= BSP_BIT32(1);
    LPC32XX_U4CLK = LPC32XX_CONFIG_U4CLK;
    LPC32XX_UART_CLKMODE = BSP_FLD32SET(LPC32XX_UART_CLKMODE, 0x2, 6, 7);

    return ns16550_probe(context);
  }
#endif

#ifdef LPC32XX_UART_6_BAUD
  static bool lpc32xx_uart_probe_6(rtems_termios_device_context *context)
  {
    /* Bypass the IrDA modulator/demodulator */
    LPC32XX_UART_CTRL |= BSP_BIT32(5);

    LPC32XX_UARTCLK_CTRL |= BSP_BIT32(3);
    LPC32XX_U6CLK = LPC32XX_CONFIG_U6CLK;
    LPC32XX_UART_CLKMODE = BSP_FLD32SET(LPC32XX_UART_CLKMODE, 0x2, 10, 11);

    return ns16550_probe(context);
  }
#endif

/* FIXME: Console selection */

#ifdef LPC32XX_UART_5_BAUD
static ns16550_context lpc32xx_uart_context_5 = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART 5"),
  .get_reg = lpc32xx_uart_get_register,
  .set_reg = lpc32xx_uart_set_register,
  .port = LPC32XX_BASE_UART_5,
  .irq = LPC32XX_IRQ_UART_5,
  .clock = 16 * LPC32XX_UART_5_BAUD,
  .initial_baud = LPC32XX_UART_5_BAUD
};
#endif

#ifdef LPC32XX_UART_3_BAUD
static ns16550_context lpc32xx_uart_context_3 = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART 3"),
  .get_reg = lpc32xx_uart_get_register,
  .set_reg = lpc32xx_uart_set_register,
  .port = LPC32XX_BASE_UART_3,
  .irq = LPC32XX_IRQ_UART_3,
  .clock = 16 * LPC32XX_UART_3_BAUD,
  .initial_baud = LPC32XX_UART_3_BAUD
};
#endif

#ifdef LPC32XX_UART_4_BAUD
static ns16550_context lpc32xx_uart_context_4 = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART 4"),
  .get_reg = lpc32xx_uart_get_register,
  .set_reg = lpc32xx_uart_set_register,
  .port = LPC32XX_BASE_UART_4,
  .irq = LPC32XX_IRQ_UART_4,
  .clock = 16 * LPC32XX_UART_4_BAUD,
  .initial_baud = LPC32XX_UART_4_BAUD
};
#endif

#ifdef LPC32XX_UART_6_BAUD
static ns16550_context lpc32xx_uart_context_6 = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART 6"),
  .get_reg = lpc32xx_uart_get_register,
  .set_reg = lpc32xx_uart_set_register,
  .port = LPC32XX_BASE_UART_6,
  .irq = LPC32XX_IRQ_UART_6,
  .clock = 16 * LPC32XX_UART_6_BAUD,
  .initial_baud = LPC32XX_UART_6_BAUD
};
#endif

#ifdef LPC32XX_UART_1_BAUD
static lpc32xx_hsu_context lpc32xx_uart_context_1 = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART 1"),
  .hsu = (volatile lpc32xx_hsu *) LPC32XX_BASE_UART_1,
  .irq = LPC32XX_IRQ_UART_1,
  .initial_baud = LPC32XX_UART_1_BAUD
};
#endif

#ifdef LPC32XX_UART_2_BAUD
static lpc32xx_hsu_context lpc32xx_uart_context_2 = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART 2"),
  .hsu = (volatile lpc32xx_hsu *) LPC32XX_BASE_UART_2,
  .irq = LPC32XX_IRQ_UART_2,
  .initial_baud = LPC32XX_UART_2_BAUD
};
#endif

#ifdef LPC32XX_UART_7_BAUD
static lpc32xx_hsu_context lpc32xx_uart_context_7 = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART 7"),
  .hsu = (volatile lpc32xx_hsu *) LPC32XX_BASE_UART_7,
  .irq = LPC32XX_IRQ_UART_7,
  .initial_baud = LPC32XX_UART_7_BAUD
};
#endif

const console_device console_device_table[] = {
  #ifdef LPC32XX_UART_5_BAUD
    {
      .device_file = "/dev/ttyS5",
      .probe = console_device_probe_default,
      .handler = &ns16550_handler_interrupt,
      .context = &lpc32xx_uart_context_5.base
    },
  #endif
  #ifdef LPC32XX_UART_3_BAUD
    {
      .device_file = "/dev/ttyS3",
      .probe = lpc32xx_uart_probe_3,
      .handler = &ns16550_handler_interrupt,
      .context = &lpc32xx_uart_context_3.base
    },
  #endif
  #ifdef LPC32XX_UART_4_BAUD
    {
      .device_file = "/dev/ttyS4",
      .probe = lpc32xx_uart_probe_4,
      .handler = &ns16550_handler_interrupt,
      .context = &lpc32xx_uart_context_4.base
    },
  #endif
  #ifdef LPC32XX_UART_6_BAUD
    {
      .device_file = "/dev/ttyS6",
      .probe = lpc32xx_uart_probe_6,
      .handler = &ns16550_handler_interrupt,
      .context = &lpc32xx_uart_context_6.base
    },
  #endif
  #ifdef LPC32XX_UART_1_BAUD
    {
      .device_file = "/dev/ttyS1",
      .probe = lpc32xx_hsu_probe,
      .handler = &lpc32xx_hsu_fns,
      .context = &lpc32xx_uart_context_1.base
    },
  #endif
  #ifdef LPC32XX_UART_2_BAUD
    {
      .device_file = "/dev/ttyS2",
      .probe = lpc32xx_hsu_probe,
      .handler = &lpc32xx_hsu_fns,
      .context = &lpc32xx_uart_context_2.base
    },
  #endif
  #ifdef LPC32XX_UART_7_BAUD
    {
      .device_file = "/dev/ttyS7",
      .probe = lpc32xx_hsu_probe,
      .handler = &lpc32xx_hsu_fns,
      .context = &lpc32xx_uart_context_7.base
    },
  #endif
};

const size_t console_device_count = RTEMS_ARRAY_SIZE(console_device_table);
