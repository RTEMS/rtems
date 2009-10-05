/**
 * @file
 *
 * @ingroup lpc24xx_io
 *
 * @brief Input and output module.
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

#include <bsp/io.h>
#include <bsp/system-clocks.h>

#define LPC24XX_IO_SELECT(pin) (pin >> 4U)

#define LPC24XX_IO_SELECT_SHIFT(pin) ((pin & 0xfU) << 1U)

#define LPC24XX_IO_SELECT_MASK 0x3U

#define LPC24XX_IO_PRIMARY 0x0U

#define LPC24XX_IO_ALTERNATE_0 0x1U

#define LPC24XX_IO_ALTERNATE_1 0x2U

#define LPC24XX_IO_ALTERNATE_2 0x3U

#define LPC24XX_IO_ENTRY(mod, idx, cfg, begin_port, begin_index, last_port, last_index, function) \
  { \
    .module = mod, \
    .index = idx, \
    .config = cfg, \
    .pin_begin = LPC24XX_IO_INDEX_BY_PORT(begin_port, begin_index), \
    .pin_last = LPC24XX_IO_INDEX_BY_PORT(last_port, last_index), \
    .pin_function = function \
  }

typedef struct {
  unsigned module : 5;
  unsigned index : 4;
  unsigned config : 4;
  unsigned pin_begin : 8;
  unsigned pin_last : 8;
  unsigned pin_function : 3;
} lpc24xx_io_entry;

typedef void (*lpc24xx_io_iterate_routine)(unsigned /* pin */, unsigned /* function */);

static const lpc24xx_io_entry lpc24xx_io_config_table [] = {
  /* UART */
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_UART, 0, 0, 0, 2, 0, 3, LPC24XX_IO_ALTERNATE_0),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_UART, 1, 0, 0, 15, 0, 16, LPC24XX_IO_ALTERNATE_0),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_UART, 1, 1, 2, 0, 2, 1, LPC24XX_IO_ALTERNATE_1),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_UART, 1, 2, 3, 16, 3, 17, LPC24XX_IO_ALTERNATE_2),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_UART, 2, 0, 0, 10, 0, 11, LPC24XX_IO_ALTERNATE_0),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_UART, 2, 1, 2, 8, 2, 9, LPC24XX_IO_ALTERNATE_1),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_UART, 2, 2, 4, 22, 4, 23, LPC24XX_IO_ALTERNATE_1),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_UART, 3, 0, 0, 0, 0, 1, LPC24XX_IO_ALTERNATE_1),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_UART, 3, 1, 0, 25, 0, 26, LPC24XX_IO_ALTERNATE_2),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_UART, 3, 2, 4, 28, 4, 29, LPC24XX_IO_ALTERNATE_2),

  /* Ethernet */
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_ETHERNET, 0, 0, 1, 0, 1, 17, LPC24XX_IO_ALTERNATE_0),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_ETHERNET, 0, 1, 1, 0, 1, 1, LPC24XX_IO_ALTERNATE_0),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_ETHERNET, 0, 1, 1, 4, 1, 4, LPC24XX_IO_ALTERNATE_0),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_ETHERNET, 0, 1, 1, 8, 1, 10, LPC24XX_IO_ALTERNATE_0),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_ETHERNET, 0, 1, 1, 14, 1, 17, LPC24XX_IO_ALTERNATE_0),

  /* ADC */
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_ADC, 0, 0, 0, 12, 0, 13, LPC24XX_IO_ALTERNATE_2),

  /* I2C */
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_I2C, 0, 0, 0, 27, 0, 28, LPC24XX_IO_ALTERNATE_0),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_I2C, 1, 0, 0, 0, 0, 1, LPC24XX_IO_ALTERNATE_2),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_I2C, 1, 1, 0, 19, 0, 20, LPC24XX_IO_ALTERNATE_2),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_I2C, 1, 2, 2, 14, 2, 15, LPC24XX_IO_ALTERNATE_2),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_I2C, 2, 0, 0, 10, 0, 11, LPC24XX_IO_ALTERNATE_1),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_I2C, 2, 1, 2, 30, 2, 31, LPC24XX_IO_ALTERNATE_2),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_I2C, 2, 2, 4, 20, 4, 21, LPC24XX_IO_ALTERNATE_1),

  /* SSP */
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_SSP, 0, 0, 0, 15, 0, 18, LPC24XX_IO_ALTERNATE_1),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_SSP, 0, 1, 1, 20, 0, 21, LPC24XX_IO_ALTERNATE_2),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_SSP, 0, 1, 1, 23, 0, 24, LPC24XX_IO_ALTERNATE_2),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_SSP, 0, 2, 2, 22, 2, 23, LPC24XX_IO_ALTERNATE_2),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_SSP, 0, 2, 2, 26, 2, 27, LPC24XX_IO_ALTERNATE_2),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_SSP, 1, 0, 0, 6, 0, 9, LPC24XX_IO_ALTERNATE_1),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_SSP, 1, 1, 0, 12, 0, 13, LPC24XX_IO_ALTERNATE_1),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_SSP, 1, 1, 0, 14, 0, 14, LPC24XX_IO_ALTERNATE_2),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_SSP, 1, 1, 1, 31, 1, 31, LPC24XX_IO_ALTERNATE_1),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_SSP, 1, 2, 4, 20, 4, 23, LPC24XX_IO_ALTERNATE_2),

  /* USB */
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_USB, 0, 0, 0, 29, 0, 30, LPC24XX_IO_ALTERNATE_0),
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_USB, 0, 0, 1, 19, 1, 19, LPC24XX_IO_ALTERNATE_1),

  /* Terminate */
  LPC24XX_IO_ENTRY(LPC24XX_MODULE_COUNT, 0, 0, 0, 0, 0, 0, 0),
};

static rtems_status_code lpc24xx_io_iterate(
  lpc24xx_module module,
  unsigned index,
  unsigned config,
  lpc24xx_io_iterate_routine routine
)
{
  rtems_status_code sc = RTEMS_INVALID_ID;
  const lpc24xx_io_entry *e = &lpc24xx_io_config_table [0];

  while (e->module != LPC24XX_MODULE_COUNT) {
    if (e->module == module && e->index == index && e->config == config) {
      unsigned pin = e->pin_begin;
      unsigned last = e->pin_last;
      unsigned function = e->pin_function;

      while (pin <= last) {
        (*routine)(pin, function);

        ++pin;
      }

      sc = RTEMS_SUCCESSFUL;
    }
    ++e;
  }

  return sc;
}

static void lpc24xx_io_do_config(unsigned pin, unsigned function)
{
  rtems_interrupt_level level;
  unsigned select = LPC24XX_IO_SELECT(pin);
  unsigned shift = LPC24XX_IO_SELECT_SHIFT(pin);

  rtems_interrupt_disable(level);

  LPC24XX_PINSEL [select] =
    (LPC24XX_PINSEL [select] & ~(LPC24XX_IO_SELECT_MASK << shift))
      | ((function & LPC24XX_IO_SELECT_MASK) << shift);

  rtems_interrupt_flash(level);

  LPC24XX_PINMODE [select] &= ~(LPC24XX_IO_SELECT_MASK << shift);

  rtems_interrupt_enable(level);
}

static void lpc24xx_io_do_release(unsigned pin, unsigned function)
{
  rtems_interrupt_level level;
  unsigned select = LPC24XX_IO_SELECT(pin);
  unsigned shift = LPC24XX_IO_SELECT_SHIFT(pin);

  rtems_interrupt_disable(level);
  LPC24XX_PINSEL [select] =
    (LPC24XX_PINSEL [select] & ~(LPC24XX_IO_SELECT_MASK << shift));
  rtems_interrupt_enable(level);
}

rtems_status_code lpc24xx_io_config(
  lpc24xx_module module,
  unsigned index,
  unsigned config
)
{
  return lpc24xx_io_iterate(module, index, config, lpc24xx_io_do_config);
}

rtems_status_code lpc24xx_io_release(
  lpc24xx_module module,
  unsigned index,
  unsigned config
)
{
  return lpc24xx_io_iterate(module, index, config, lpc24xx_io_do_release);
}

rtems_status_code lpc24xx_gpio_config(
  unsigned pin,
  lpc24xx_gpio_settings settings
)
{
  if (pin <= LPC24XX_IO_INDEX_MAX) {
    rtems_interrupt_level level;
    unsigned port = LPC24XX_IO_PORT(pin);
    unsigned bit = LPC24XX_IO_PORT_BIT(pin);
    unsigned select = LPC24XX_IO_SELECT(pin);
    unsigned shift = LPC24XX_IO_SELECT_SHIFT(pin);
    unsigned resistor = settings & LPC24XX_GPIO_RESISTOR_MASK;
    unsigned output = (settings & LPC24XX_GPIO_OUTPUT) != 0 ? 1U : 0U;

    /* Get resistor flags */
    switch (resistor) {
      case LPC24XX_GPIO_RESISTOR_PULL_UP:
      case LPC24XX_GPIO_RESISTOR_DEFAULT:
        resistor = 0x0U;
        break;
      case LPC24XX_GPIO_RESISTOR_NONE:
        resistor = 0x2U;
        break;
      case LPC24XX_GPIO_RESISTOR_PULL_DOWN:
        resistor = 0x3U;
        break;
      default:
        return RTEMS_INVALID_NUMBER;
    }

    rtems_interrupt_disable(level);

    /* Resistor */
    LPC24XX_PINMODE [select] =
      (LPC24XX_PINMODE [select] & ~(LPC24XX_IO_SELECT_MASK << shift))
        | ((resistor & LPC24XX_IO_SELECT_MASK) << shift); 

    rtems_interrupt_flash(level);

    /* Input or output */
    LPC24XX_FIO [port].dir =
      (LPC24XX_FIO [port].dir & ~(1U << bit)) | (output << bit);

    rtems_interrupt_enable(level);
  } else {
    return RTEMS_INVALID_ID;
  }

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code lpc24xx_module_do_enable(
  lpc24xx_module module,
  unsigned index,
  lpc24xx_module_clock clock,
  bool enable
)
{
  const unsigned NO_POWER = 32U;
  const unsigned INVALID = 33U;
  unsigned power_bit = INVALID;
  unsigned clock_shift = INVALID;

  /* Check clock value */
  if ((clock & ~LPC24XX_MODULE_CLOCK_MASK) != 0U) {
    return RTEMS_INVALID_NUMBER;
  }

  /* Get power bit */
  switch (module) {
    case LPC24XX_MODULE_ACF:
      if (index == 0) {
        power_bit = NO_POWER;
        clock_shift = 30U;
      }
      break;
    case LPC24XX_MODULE_ADC:
      if (index == 0) {
        power_bit = 12U;
      }
      break;
    case LPC24XX_MODULE_BAT_RAM:
      if (index == 0) {
        power_bit = NO_POWER;
        clock_shift = 32U;
      }
      break;
    case LPC24XX_MODULE_CAN:
      if (index < 2) {
        power_bit = 13U + index;
      }
      break;
    case LPC24XX_MODULE_DAC:
      if (index == 0) {
        power_bit = NO_POWER;
        clock_shift = 22U;
      }
      break;
    case LPC24XX_MODULE_EMC:
      if (index == 0) {
        power_bit = 11U;
      }
      break;
    case LPC24XX_MODULE_ETHERNET:
      if (index == 0) {
        power_bit = 30U;
      }
      break;
    case LPC24XX_MODULE_GPDMA:
      if (index == 0) {
        power_bit = 29U;
      }
      break;
    case LPC24XX_MODULE_GPIO:
      if (index == 0) {
        power_bit = NO_POWER;
        clock_shift = 34U;
      }
      break;
    case LPC24XX_MODULE_I2C:
      switch (index) {
        case 0U:
          power_bit = 7U;
          break;
        case 1U:
          power_bit = 19U;
          break;
        case 2U:
          power_bit = 26U;
          break;
      }
      break;
    case LPC24XX_MODULE_I2S:
      if (index == 0) {
        power_bit = 27U;
      }
      break;
    case LPC24XX_MODULE_LCD:
      if (index == 0) {
        power_bit = 20U;
      }
      break;
    case LPC24XX_MODULE_MCI:
      if (index == 0) {
        power_bit = 28U;
      }
      break;
    case LPC24XX_MODULE_PCB:
      if (index == 0) {
        power_bit = NO_POWER;
        clock_shift = 36U;
      }
      break;
    case LPC24XX_MODULE_PWM:
      if (index < 2) {
        power_bit = 5U + index;
      }
      break;
    case LPC24XX_MODULE_RTC:
      if (index == 0) {
        power_bit = 9U;
      }
      break;
    case LPC24XX_MODULE_SPI:
      if (index == 0) {
        power_bit = 8U;
      }
      break;
    case LPC24XX_MODULE_SSP:
      switch (index) {
        case 0U:
          power_bit = 21U;
          break;
        case 1U:
          power_bit = 10U;
          break;
      }
      break;
    case LPC24XX_MODULE_SYSCON:
      if (index == 0) {
        power_bit = NO_POWER;
        clock_shift = 60U;
      }
      break;
    case LPC24XX_MODULE_TIMER:
      if (index < 2) {
        power_bit = 1U + index;
      } else if (index < 4) {
        power_bit = 20U + index;
      }
      break;
    case LPC24XX_MODULE_UART:
      if (index < 2) {
        power_bit = 3U + index;
      } else if (index < 4) {
        power_bit = 22U + index;
      }
      break;
    case LPC24XX_MODULE_USB:
      if (index == 0) {
        power_bit = 31U;
      }
      break;
    case LPC24XX_MODULE_WDT:
      if (index == 0) {
        power_bit = NO_POWER;
        clock_shift = 0U;
      }
      break;
    default:
      return RTEMS_INVALID_ID;
  }

  /* Check power bit */
  if (power_bit == INVALID) {
    return RTEMS_INVALID_ID;
  }

  /* Get clock shift */
  if (clock_shift == INVALID) {
    clock_shift = power_bit << 1U;
  }

  /* Enable or disable module */
  if (enable) {
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);
    PCONP |= 1U << power_bit;
    rtems_interrupt_enable(level);

    if (module != LPC24XX_MODULE_USB) {
      rtems_interrupt_disable(level);
      if (clock_shift < 32U) {
        PCLKSEL0 = (PCLKSEL0 & ~(LPC24XX_MODULE_CLOCK_MASK << clock_shift))
            | (clock << clock_shift);
      } else {
        clock_shift -= 32U;
        PCLKSEL1 = (PCLKSEL1 & ~(LPC24XX_MODULE_CLOCK_MASK << clock_shift))
            | (clock << clock_shift);
      }
      rtems_interrupt_enable(level);
    } else {
      unsigned pllclk = lpc24xx_pllclk();
      unsigned usbsel = pllclk / 48000000U - 1U;

      if (usbsel > 15U || (usbsel % 2U != 1U) || (pllclk % 48000000U) != 0U) {
        return RTEMS_INCORRECT_STATE;
      }

      USBCLKCFG = usbsel;
    }
  } else {
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);
    PCONP &= ~(1U << power_bit);
    rtems_interrupt_enable(level);
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code lpc24xx_module_enable(
  lpc24xx_module module,
  unsigned index,
  lpc24xx_module_clock clock
)
{
  return lpc24xx_module_do_enable(module, index, clock, true);
}

rtems_status_code lpc24xx_module_disable(
  lpc24xx_module module,
  unsigned index
)
{
  return lpc24xx_module_do_enable(module, index, 0U, false);
}
