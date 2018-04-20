/*
 * Copyright © 2013 Eugeniy Meshcheryakov <eugen@debian.org>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp/io.h>
#include <bsp/lm3s69xx.h>
#include <bsp/syscon.h>
#include <rtems.h>

static void set_bit(volatile uint32_t *reg, unsigned index, uint32_t set)
{
  uint32_t mask = 1U;
  uint32_t val = *reg;

  val &= ~(mask << index);
  val |= set << index;

  *reg = val;
}

static void set_config(unsigned int pin, const lm3s69xx_gpio_config *config)
{
  unsigned int port = LM3S69XX_GPIO_PORT_OF_PIN(pin);
  volatile lm3s69xx_gpio *gpio = LM3S69XX_GPIO(port);
  unsigned int index = LM3S69XX_GPIO_INDEX_OF_PIN(pin);
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);

  lm3s69xx_syscon_enable_gpio_clock(port, true);

  /* Disable digital and analog functions before reconfiguration. */
  set_bit(&gpio->den, index, 0);
  set_bit(&gpio->amsel, index, 0);

  set_bit(&gpio->afsel, index, config->alternate);
  set_bit(&gpio->dir, index, config->dir);
  set_bit(&gpio->odr, index, config->otype);

  switch (config->drive) {
  case LM3S69XX_GPIO_DRIVE_4MA:
    gpio->dr4r |= 1 << index;
    break;
  case LM3S69XX_GPIO_DRIVE_8MA:
    gpio->dr8r |= 1 << index;
    break;
  default:
    gpio->dr2r |= 1 << index;
    break;
  }

  switch (config->pull) {
  case LM3S69XX_GPIO_PULL_UP:
    gpio->pur |= 1 << index;
    break;
  case LM3S69XX_GPIO_PULL_DOWN:
    gpio->pdr |= 1 << index;
    break;
  default:
    set_bit(&gpio->pdr, index, 0);
    set_bit(&gpio->pur, index, 0);
    break;
  }

  set_bit(&gpio->slr, index, config->slr);

  set_bit(&gpio->den, index, config->digital);
  set_bit(&gpio->amsel, index, config->analog);

  rtems_interrupt_enable(level);
}

void lm3s69xx_gpio_set_config(const lm3s69xx_gpio_config *config)
{
  unsigned int current = config->pin_first;
  unsigned int last = config->pin_last;

  while (current <= last) {
    set_config(current, config);
    current++;
  }
}

void lm3s69xx_gpio_set_config_array(const lm3s69xx_gpio_config *configs, unsigned int count)
{
  unsigned int i;

  for (i = 0; i < count; i++)
    lm3s69xx_gpio_set_config(&configs[i]);
}

/**
 * Enables/disables digital function on the specified pin.
 */
void lm3s69xx_gpio_digital_enable(unsigned int pin, bool enable)
{
  unsigned int port = LM3S69XX_GPIO_PORT_OF_PIN(pin);
  volatile lm3s69xx_gpio *gpio = LM3S69XX_GPIO(port);
  unsigned int index = LM3S69XX_GPIO_INDEX_OF_PIN(pin);
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  set_bit(&gpio->den, index, enable);
  rtems_interrupt_enable(level);
}

/**
 * Enables/disables analog mode on the specified pin.
 */
void lm3s69xx_gpio_analog_mode_select(unsigned int pin, bool enable)
{
  unsigned int port = LM3S69XX_GPIO_PORT_OF_PIN(pin);
  volatile lm3s69xx_gpio *gpio = LM3S69XX_GPIO(port);
  unsigned int index = LM3S69XX_GPIO_INDEX_OF_PIN(pin);
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  set_bit(&gpio->amsel, index, enable);
  rtems_interrupt_enable(level);
}

void lm3s69xx_gpio_set_pin(unsigned int pin, bool set)
{
  unsigned int port = LM3S69XX_GPIO_PORT_OF_PIN(pin);
  volatile lm3s69xx_gpio *gpio = LM3S69XX_GPIO(port);
  unsigned int index = LM3S69XX_GPIO_INDEX_OF_PIN(pin);
  uint32_t mask = 1U << index;

  gpio->data[mask] = set ? mask : 0;
}

bool lm3s69xx_gpio_get_pin(unsigned int pin)
{
  unsigned int port = LM3S69XX_GPIO_PORT_OF_PIN(pin);
  volatile lm3s69xx_gpio *gpio = LM3S69XX_GPIO(port);
  unsigned int index = LM3S69XX_GPIO_INDEX_OF_PIN(pin);
  uint32_t mask = 1U << index;

  return gpio->data[mask] != 0;
}
