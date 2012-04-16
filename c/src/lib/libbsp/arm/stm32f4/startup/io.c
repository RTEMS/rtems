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
#include <bsp/rcc.h>

#include <rtems.h>

RTEMS_STATIC_ASSERT(sizeof(stm32f4_gpio_config) == 4, size_of_config);

void stm32f4_gpio_set_clock(int pin, bool set)
{
  int port = STM32F4_GPIO_PORT_OF_PIN(pin);
  stm32f4_rcc_index index = STM32F4_RCC_GPIOA + port;

  stm32f4_rcc_set_clock(index, set);
}

static void clear_and_set(
  volatile uint32_t *reg,
  unsigned index,
  unsigned width,
  uint32_t set
)
{
  uint32_t mask = (1U << width) - 1U;
  unsigned shift = width * index;
  uint32_t val = *reg;

  val &= ~(mask << shift);
  val |= set << shift;

  *reg = val;
}

static void set_config(unsigned pin, const stm32f4_gpio_config *config)
{
  unsigned port = STM32F4_GPIO_PORT_OF_PIN(pin);
  volatile stm32f4_gpio *gpio = STM32F4_GPIO(port);
  unsigned index = STM32F4_GPIO_INDEX_OF_PIN(pin);
  unsigned af_reg = index >> 3;
  unsigned af_index = index & 0x7;
  int set_or_clear_offset = config->fields.output ? 0 : 16;
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  gpio->bsrr = 1U << (index + set_or_clear_offset);
  clear_and_set(&gpio->pupdr, index, 2, config->fields.pupd);
  clear_and_set(&gpio->otyper, index, 1, config->fields.otype);
  clear_and_set(&gpio->ospeedr, index, 2, config->fields.ospeed);
  clear_and_set(&gpio->afr [af_reg], af_index, 4, config->fields.af);
  clear_and_set(&gpio->moder, index, 2, config->fields.mode);
  rtems_interrupt_enable(level);
}

void stm32f4_gpio_set_config(const stm32f4_gpio_config *config)
{
  int current = config->fields.pin_first;
  int last = config->fields.pin_last;

  while (current <= last) {
    stm32f4_gpio_set_clock(current, true);
    set_config(current, config);
    ++current;
  }
}

void stm32f4_gpio_set_config_array(const stm32f4_gpio_config *configs)
{
  stm32f4_gpio_config terminal = STM32F4_GPIO_CONFIG_TERMINAL;

  while (configs->value != terminal.value) {
    stm32f4_gpio_set_config(configs);
    ++configs;
  }
}

void stm32f4_gpio_set_output(int pin, bool set)
{
  int port = STM32F4_GPIO_PORT_OF_PIN(pin);
  volatile stm32f4_gpio *gpio = STM32F4_GPIO(port);
  int index = STM32F4_GPIO_INDEX_OF_PIN(pin);
  int set_or_clear_offset = set ? 0 : 16;

  gpio->bsrr = 1U << (index + set_or_clear_offset);
}

bool stm32f4_gpio_get_input(int pin)
{
  int port = STM32F4_GPIO_PORT_OF_PIN(pin);
  volatile stm32f4_gpio *gpio = STM32F4_GPIO(port);
  int index = STM32F4_GPIO_INDEX_OF_PIN(pin);

  return (gpio->idr & (1U << index)) != 0;
}
