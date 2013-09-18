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
#include <bsp/stm32f4.h>

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

#ifdef STM32F4_FAMILY_F10XXX
#define STM32F4_AFIO_REMAP_ENTRY(mod, afio_reg_v, start_v, width_v, value_v) \
  [mod] = { \
    .afio_reg = afio_reg_v, \
    .start = start_v, \
    .width = width_v, \
    .value = value_v, \
    .reserved = 0 \
  }

typedef struct {
  uint16_t afio_reg : 3;
  uint16_t start : 5;
  uint16_t width : 2;
  uint16_t value : 3;
  uint16_t reserved : 3;
} stm32f4_afio_remap_entry;

static const stm32f4_afio_remap_entry stm32f4_afio_remap_table [] = {
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_DONT_CHANGE, 0, 0, 0, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_SPI1_0, 1, 0, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_SPI1_1, 1, 0, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_I2C1_0, 1, 1, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_I2C1_1, 1, 1, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_USART1_0, 1, 2, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_USART1_1, 1, 2, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_USART2_0, 1, 3, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_USART2_1, 1, 3, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_USART3_0, 1, 4, 2, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_USART3_1, 1, 4, 2, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_USART3_3, 1, 4, 2, 3),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM1_0, 1, 6, 2, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM1_1, 1, 6, 2, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM1_3, 1, 6, 2, 3),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM2_0, 1, 8, 2, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM2_1, 1, 8, 2, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM2_2, 1, 8, 2, 2),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM2_3, 1, 8, 2, 3),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM3_0, 1, 10, 2, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM3_2, 1, 10, 2, 2),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM3_3, 1, 10, 2, 3),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM4_0, 1, 12, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM4_1, 1, 12, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_CAN1_0, 1, 13, 2, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_CAN1_2, 1, 13, 2, 2),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_CAN1_3, 1, 13, 2, 3),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_PD01_0, 1, 15, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_PD01_1, 1, 15, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM5CH4_0, 1, 16, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM5CH4_1, 1, 16, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_ADC1_ETRGINJ_0, 1, 17, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_ADC1_ETRGINJ_1, 1, 17, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_ADC1_ETRGREG_0, 1, 18, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_ADC1_ETRGREG_1, 1, 18, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_ADC2_ETRGINJ_0, 1, 19, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_ADC2_ETRGINJ_1, 1, 19, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_ADC2_ETRGREG_0, 1, 20, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_ADC2_ETRGREG_1, 1, 20, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_ETH_0, 1, 21, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_ETH_1, 1, 21, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_CAN2_0, 1, 22, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_CAN2_1, 1, 22, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_MII_RMII_0, 1, 23, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_MII_RMII_1, 1, 23, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_SWJ_0, 1, 24, 3, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_SWJ_1, 1, 24, 3, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_SWJ_2, 1, 24, 3, 2),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_SWJ_4, 1, 24, 3, 4),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_SPI3_0, 1, 28, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_SPI3_1, 1, 28, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM2ITR1_0, 1, 29, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM2ITR1_1, 1, 29, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_PTP_PPS_0, 1, 30, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_PTP_PPS_1, 1, 30, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM15_0, 6, 0, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM15_1, 6, 0, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM16_0, 6, 1, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM16_1, 6, 1, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM17_0, 6, 2, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM17_1, 6, 2, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_CEC_0, 6, 3, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_CEC_1, 6, 3, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM1_DMA_0, 6, 4, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM1_DMA_1, 6, 4, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM9_0, 6, 5, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM9_1, 6, 5, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM10_0, 6, 6, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM10_1, 6, 6, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM11_0, 6, 7, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM11_1, 6, 7, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM13_0, 6, 8, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM13_1, 6, 8, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM14_0, 6, 9, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM14_1, 6, 9, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_FSMC_0, 6, 10, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_FSMC_1, 6, 10, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM67_DAC_DMA_0, 6, 11, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM67_DAC_DMA_1, 6, 11, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM12_0, 6, 12, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_TIM12_1, 6, 12, 1, 1),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_MISC_0, 6, 13, 1, 0),
  STM32F4_AFIO_REMAP_ENTRY(STM32F4_GPIO_REMAP_MISC_1, 6, 13, 1, 1),
};

static void set_remap_config(stm32f4_gpio_remap remap)
{
  if(remap != STM32F4_GPIO_REMAP_DONT_CHANGE)
  {
    stm32f4_afio_remap_entry entry = stm32f4_afio_remap_table[remap];
    volatile stm32f4_afio *afio = STM32F4_AFIO;
    volatile uint32_t *reg = ((uint32_t*) afio) + entry.afio_reg;
    uint32_t mask = (1 << entry.width) - 1;
    uint32_t value = *reg;

    value &= mask << entry.start;
    value |= entry.value << entry.start;

    *reg = value;
  }
}

#endif /* STM32F4_FAMILY_F10XXX */

static void set_config(unsigned pin, const stm32f4_gpio_config *config)
{
  unsigned port = STM32F4_GPIO_PORT_OF_PIN(pin);
  volatile stm32f4_gpio *gpio = STM32F4_GPIO(port);
  unsigned index = STM32F4_GPIO_INDEX_OF_PIN(pin);
  rtems_interrupt_level level;
  int set_or_clear_offset = config->fields.output ? 0 : 16;
#ifdef STM32F4_FAMILY_F4XXXX
  unsigned af_reg = index >> 3;
  unsigned af_index = index & 0x7;

  rtems_interrupt_disable(level);
  gpio->bsrr = 1U << (index + set_or_clear_offset);
  clear_and_set(&gpio->pupdr, index, 2, config->fields.pupd);
  clear_and_set(&gpio->otyper, index, 1, config->fields.otype);
  clear_and_set(&gpio->ospeedr, index, 2, config->fields.ospeed);
  clear_and_set(&gpio->afr [af_reg], af_index, 4, config->fields.af);
  clear_and_set(&gpio->moder, index, 2, config->fields.mode);
  rtems_interrupt_enable(level);

#endif /* STM32F4_FAMILY_F4XXXX */
#ifdef STM32F4_FAMILY_F10XXX
  unsigned cr_reg = index >> 3;
  unsigned cr_index = index & 3;

  rtems_interrupt_disable(level);
  gpio->bsrr = 1U << (index + set_or_clear_offset);
  clear_and_set(&gpio->cr[cr_reg], cr_index, 4,
    (config->fields.cnf << 2) | config->fields.mode);
  set_remap_config(config->fields.remap);
  rtems_interrupt_enable(level);

#endif /* STM32F4_FAMILY_F10XXX */
}

void stm32f4_gpio_set_config(const stm32f4_gpio_config *config)
{
  int current = config->fields.pin_first;
  int last = config->fields.pin_last;

#ifdef STM32F4_FAMILY_F10XXX
  stm32f4_rcc_set_clock(STM32F4_RCC_AFIO, true);
#endif /* STM32F4_FAMILY_F10XXX */

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
