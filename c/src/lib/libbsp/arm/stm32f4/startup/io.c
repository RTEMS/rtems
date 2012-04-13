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

#include <bsp/io.h>

#include <rtems.h>

static void clear_and_set(
  volatile uint32_t *reg,
  unsigned index,
  unsigned width,
  uint32_t set
)
{
  uint32_t one = 1;
  uint32_t mask = (one << width) - one;
  unsigned shift = width * index;
  uint32_t val = *reg;

  val &= ~(mask << shift);
  val |= set << shift;

  *reg = val;
}

void stm32f4_gpio_set_config(const stm32f4_gpio_config *config)
{
  unsigned pin = config->pin;
  unsigned port = STM32F4_GPIO_PORT_OF_PIN(pin);
  volatile stm32f4_gpio *gpio = STM32F4_GPIO(port);
  unsigned index = STM32F4_GPIO_INDEX_OF_PIN(pin);
  unsigned af_reg = index >> 8;
  unsigned af_index = index & 0x3;
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  clear_and_set(&gpio->moder, index, 2, config->mode);
  clear_and_set(&gpio->afr [af_reg], af_index, 4, config->af);
  clear_and_set(&gpio->pupdr, index, 2, config->pupd);
  clear_and_set(&gpio->otyper, index, 1, config->otype);
  clear_and_set(&gpio->ospeedr, index, 2, config->ospeed);
  rtems_interrupt_enable(level);
}

void stm32f4_gpio_set_output(int pin, bool set)
{
  int port = STM32F4_GPIO_PORT_OF_PIN(pin);
  volatile stm32f4_gpio *gpio = STM32F4_GPIO(port);
  int index = STM32F4_GPIO_INDEX_OF_PIN(pin);
  int offset = set ? 0 : 16;
  uint32_t one = 1;

  gpio->bsrr = one << (index + offset);
}

bool stm32f4_gpio_get_input(int pin)
{
  int port = STM32F4_GPIO_PORT_OF_PIN(pin);
  volatile stm32f4_gpio *gpio = STM32F4_GPIO(port);
  int index = STM32F4_GPIO_INDEX_OF_PIN(pin);
  uint32_t one = 1;

  return (gpio->idr & (one << index)) != 0;
}
