/*
 * Copyright (c) 2012 Sebastian Huber.  All rights reserved.
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

#include <libchip/sersupp.h>

#include <bsp.h>
#include <bsp/io.h>
#include <bsp/rcc.h>
#include <bsp/irq.h>
#include <bsp/usart.h>
#include <bsp/stm32f4.h>

static volatile stm32f4_usart *usart_get_regs(const console_tbl *ct)
{
  return (stm32f4_usart *) ct->ulCtrlPort1;
}

#if 0
static rtems_vector_number usart_get_irq_number(const console_tbl *ct)
{
  return ct->ulIntVector;
}
#endif

static const stm32f4_rcc_index usart_rcc_index [] = {
  STM32F4_RCC_USART1,
  STM32F4_RCC_USART2,
  STM32F4_RCC_USART3,
  STM32F4_RCC_UART4,
  STM32F4_RCC_UART5,
  STM32F4_RCC_USART6
};

static stm32f4_rcc_index usart_get_rcc_index(const console_tbl *ct)
{
  return usart_rcc_index [ct->ulCtrlPort2];
}

static const uint8_t usart_pclk_index [] = { 1, 0, 0, 0, 0, 1 };

static const uint32_t usart_pclk_by_index [] = {
  STM32F4_PCLK1,
  STM32F4_PCLK2
};

static uint32_t usart_get_pclk(const console_tbl *ct)
{
  return usart_pclk_by_index [usart_pclk_index [ct->ulCtrlPort2]];
}

static uint32_t usart_get_baud(const console_tbl *ct)
{
  return ct->ulClock;
}

/*
 * a = 8 * (2 - CR1[OVER8])
 *
 * usartdiv = div_mantissa + div_fraction / a
 *
 * baud = pclk / (a * usartdiv)
 *
 * usartdiv = pclk / (a * baud)
 *
 * Calculation in integer arithmetic:
 *
 * 1. div_mantissa = pclk / (a * baud)
 *
 * 2. div_fraction = pclk / (baud - a * div_mantissa)
 */
static uint32_t usart_get_bbr(
  volatile stm32f4_usart *usart,
  uint32_t pclk,
  uint32_t baud
)
{
  uint32_t a = 8 * (2 - ((usart->cr1 & STM32F4_USART_CR1_OVER8) != 0));
  uint32_t div_mantissa_low = pclk / (a * baud);
  uint32_t div_fraction_low = pclk / (baud - a * div_mantissa_low);
  uint32_t div_mantissa_high;
  uint32_t div_fraction_high;
  uint32_t high_err;
  uint32_t low_err;
  uint32_t div_mantissa;
  uint32_t div_fraction;

  if (div_fraction_low < a - 1) {
    div_mantissa_high = div_fraction_low;
    div_fraction_high = div_fraction_low + 1;
  } else {
    div_mantissa_high = div_fraction_low + 1;
    div_fraction_high = 0;
  }

  high_err = pclk - baud * (a * div_mantissa_high + div_fraction_high);
  low_err = baud * (a * div_mantissa_low + div_fraction_low) - pclk;

  if (low_err < high_err) {
    div_mantissa = div_mantissa_low;
    div_fraction = div_fraction_low;
  } else {
    div_mantissa = div_mantissa_high;
    div_fraction = div_fraction_high;
  }

  return STM32F4_USART_BBR_DIV_MANTISSA(div_mantissa)
    | STM32F4_USART_BBR_DIV_FRACTION(div_fraction);
}

#define USART_CFG(port, idx, altfunc) \
  { \
    .pin = STM32F4_GPIO_PIN(port, idx), \
    .mode = STM32F4_GPIO_MODE_AF, \
    .otype = STM32F4_GPIO_OTYPE_PUSH_PULL, \
    .ospeed = STM32F4_GPIO_OSPEED_2_MHZ, \
    .pupd = STM32F4_GPIO_PULL_UP, \
    .af = altfunc \
  }

static const stm32f4_gpio_config usart_gpio_config [] [2] = {
  {
    USART_CFG(0, 9, STM32F4_GPIO_AF_USART1),
    USART_CFG(0, 10, STM32F4_GPIO_AF_USART1)
  }, {
    USART_CFG(0, 2, STM32F4_GPIO_AF_USART2),
    USART_CFG(0, 3, STM32F4_GPIO_AF_USART2)
  }, {
    USART_CFG(3, 8, STM32F4_GPIO_AF_USART3),
    USART_CFG(3, 9, STM32F4_GPIO_AF_USART3)
  }, {
    USART_CFG(0, 1, STM32F4_GPIO_AF_UART4),
    USART_CFG(0, 2, STM32F4_GPIO_AF_UART4)
  }, {
    USART_CFG(2, 11, STM32F4_GPIO_AF_UART5),
    USART_CFG(2, 12, STM32F4_GPIO_AF_UART5)
  }, {
    USART_CFG(2, 6, STM32F4_GPIO_AF_USART6),
    USART_CFG(2, 7, STM32F4_GPIO_AF_USART6)
  }
};

static void usart_set_gpio_config(const console_tbl *ct)
{
  const stm32f4_gpio_config *config = usart_gpio_config [ct->ulCtrlPort2];

  stm32f4_rcc_set_gpio_clock(config [0].pin, true);
  stm32f4_gpio_set_config(&config [0]);
  stm32f4_rcc_set_gpio_clock(config [1].pin, true);
  stm32f4_gpio_set_config(&config [1]);
}

static void usart_initialize(int minor)
{
  const console_tbl *ct = Console_Port_Tbl [minor];
  volatile stm32f4_usart *usart = usart_get_regs(ct);
  uint32_t pclk = usart_get_pclk(ct);
  uint32_t baud = usart_get_baud(ct);
  stm32f4_rcc_index rcc_index = usart_get_rcc_index(ct);

  stm32f4_rcc_set_clock(rcc_index, true);
  usart_set_gpio_config(ct);

  usart->cr1 = 0;
  usart->cr2 = 0;
  usart->cr3 = 0;
  usart->bbr = usart_get_bbr(usart, pclk, baud);
  usart->cr1 = STM32F4_USART_CR1_UE
    | STM32F4_USART_CR1_TE
    | STM32F4_USART_CR1_RE;
}

static int usart_first_open(int major, int minor, void *arg)
{
  rtems_libio_open_close_args_t *oc = (rtems_libio_open_close_args_t *) arg;
  struct rtems_termios_tty *tty = (struct rtems_termios_tty *) oc->iop->data1;
  const console_tbl *ct = Console_Port_Tbl [minor];
  console_data *cd = &Console_Port_Data [minor];

  cd->termios_data = tty;
  rtems_termios_set_initial_baud(tty, ct->ulClock);

  return 0;
}

static int usart_last_close(int major, int minor, void *arg)
{
  return 0;
}

static int usart_read_polled(int minor)
{
  const console_tbl *ct = Console_Port_Tbl [minor];
  volatile stm32f4_usart *usart = usart_get_regs(ct);

  if ((usart->sr & STM32F4_USART_SR_RXNE) != 0) {
    return STM32F4_USART_DR_GET(usart->dr);
  } else {
    return -1;
  }
}

static void usart_write_polled(int minor, char c)
{
  const console_tbl *ct = Console_Port_Tbl [minor];
  volatile stm32f4_usart *usart = usart_get_regs(ct);

  while ((usart->sr & STM32F4_USART_SR_TXE) == 0) {
    /* Wait */
  }

  usart->dr = STM32F4_USART_DR(c);
}

static ssize_t usart_write_support_polled(
  int minor,
  const char *s,
  size_t n
)
{
  ssize_t i = 0;

  for (i = 0; i < n; ++i) {
    usart_write_polled(minor, s [i]);
  }

  return n;
}

static int usart_set_attributes(int minor, const struct termios *term)
{
  return -1;
}

console_fns stm32f4_usart_fns = {
  .deviceProbe = libchip_serial_default_probe,
  .deviceFirstOpen = usart_first_open,
  .deviceLastClose = usart_last_close,
  .deviceRead = usart_read_polled,
  .deviceWrite = usart_write_support_polled,
  .deviceInitialize = usart_initialize,
  .deviceWritePolled = usart_write_polled,
  .deviceSetAttributes = usart_set_attributes,
  .deviceOutputUsesInterrupts = false
};
