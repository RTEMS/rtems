/**
 * @file
 *
 * @ingroup lpc24xx
 *
 * Input and output module.
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

#define LPC24XX_IO_SELECT( index) (index >> 4U)

#define LPC24XX_IO_SELECT_SHIFT( index) ((index & 0xfU) << 1U)

#define LPC24XX_IO_SELECT_MASK 0x3U

#define LPC24XX_IO_PRIMARY 0x0U

#define LPC24XX_IO_ALTERNATE_0 0x1U

#define LPC24XX_IO_ALTERNATE_1 0x2U

#define LPC24XX_IO_ALTERNATE_2 0x3U

#define LPC24XX_IO_HEADER_FLAG 0x80U

#define LPC24XX_IO_ENTRY( b, e, f) \
  { .function = f, .begin = b, .end = e }

#define LPC24XX_IO_HEADER( module, index, config) \
  { .function = config | LPC24XX_IO_HEADER_FLAG, .begin = module, .end = index }

typedef struct  __attribute__ ((__packed__)) {
  unsigned char function;
  unsigned char begin;
  unsigned char end;
} lpc24xx_io_entry;

typedef void (*lpc24xx_io_iterate_routine)( unsigned /* index */, unsigned /* function */);

static const lpc24xx_io_entry lpc24xx_io_config_table [] = {
  /* EMC */
  LPC24XX_IO_HEADER( LPC24XX_MODULE_EMC, 0, 0),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 2, 16), LPC24XX_IO_INDEX_BY_PORT( 2, 22), LPC24XX_IO_ALTERNATE_0),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 2, 24), LPC24XX_IO_INDEX_BY_PORT( 2, 26), LPC24XX_IO_ALTERNATE_0),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 2, 26), LPC24XX_IO_INDEX_BY_PORT( 2, 30), LPC24XX_IO_ALTERNATE_0),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 3, 0), LPC24XX_IO_INDEX_BY_PORT( 3, 16), LPC24XX_IO_ALTERNATE_0),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 4, 0), LPC24XX_IO_INDEX_BY_PORT( 4, 28), LPC24XX_IO_ALTERNATE_0),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 4, 30), LPC24XX_IO_INDEX_BY_PORT( 5, 0), LPC24XX_IO_ALTERNATE_0),

  /* UART */
  LPC24XX_IO_HEADER( LPC24XX_MODULE_UART, 0, 0),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 0, 2), LPC24XX_IO_INDEX_BY_PORT( 0, 4), LPC24XX_IO_ALTERNATE_0),
  LPC24XX_IO_HEADER( LPC24XX_MODULE_UART, 1, 0),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 0, 15), LPC24XX_IO_INDEX_BY_PORT( 0, 17), LPC24XX_IO_ALTERNATE_1),
  LPC24XX_IO_HEADER( LPC24XX_MODULE_UART, 1, 1),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 2, 0), LPC24XX_IO_INDEX_BY_PORT( 2, 2), LPC24XX_IO_ALTERNATE_2),
  LPC24XX_IO_HEADER( LPC24XX_MODULE_UART, 1, 2),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 3, 16), LPC24XX_IO_INDEX_BY_PORT( 3, 18), LPC24XX_IO_ALTERNATE_1),
  LPC24XX_IO_HEADER( LPC24XX_MODULE_UART, 2, 0),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 0, 10), LPC24XX_IO_INDEX_BY_PORT( 0, 12), LPC24XX_IO_ALTERNATE_1),
  LPC24XX_IO_HEADER( LPC24XX_MODULE_UART, 2, 1),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 2, 8), LPC24XX_IO_INDEX_BY_PORT( 2, 10), LPC24XX_IO_ALTERNATE_1),
  LPC24XX_IO_HEADER( LPC24XX_MODULE_UART, 2, 2),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 4, 22), LPC24XX_IO_INDEX_BY_PORT( 4, 24), LPC24XX_IO_ALTERNATE_1),
  LPC24XX_IO_HEADER( LPC24XX_MODULE_UART, 3, 0),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 0, 0), LPC24XX_IO_INDEX_BY_PORT( 0, 2), LPC24XX_IO_ALTERNATE_2),
  LPC24XX_IO_HEADER( LPC24XX_MODULE_UART, 3, 1),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 0, 25), LPC24XX_IO_INDEX_BY_PORT( 0, 27), LPC24XX_IO_ALTERNATE_2),
  LPC24XX_IO_HEADER( LPC24XX_MODULE_UART, 3, 2),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 4, 28), LPC24XX_IO_INDEX_BY_PORT( 4, 30), LPC24XX_IO_ALTERNATE_2),

  /* Ethernet */
  LPC24XX_IO_HEADER( LPC24XX_MODULE_ETHERNET, 0, 0),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 1, 0), LPC24XX_IO_INDEX_BY_PORT( 1, 18), LPC24XX_IO_ALTERNATE_0),
  LPC24XX_IO_HEADER( LPC24XX_MODULE_ETHERNET, 0, 1),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 1, 0), LPC24XX_IO_INDEX_BY_PORT( 1, 2), LPC24XX_IO_ALTERNATE_0),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 1, 4), LPC24XX_IO_INDEX_BY_PORT( 1, 5), LPC24XX_IO_ALTERNATE_0),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 1, 8), LPC24XX_IO_INDEX_BY_PORT( 1, 11), LPC24XX_IO_ALTERNATE_0),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 1, 14), LPC24XX_IO_INDEX_BY_PORT( 1, 18), LPC24XX_IO_ALTERNATE_0),

  /* ADC */
  LPC24XX_IO_HEADER( LPC24XX_MODULE_ADC, 0, 0),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 0, 12), LPC24XX_IO_INDEX_BY_PORT( 0, 14), LPC24XX_IO_ALTERNATE_2),

  /* I2C */
  LPC24XX_IO_HEADER( LPC24XX_MODULE_I2C, 0, 0),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 0, 27), LPC24XX_IO_INDEX_BY_PORT( 0, 29), LPC24XX_IO_ALTERNATE_0),
  LPC24XX_IO_HEADER( LPC24XX_MODULE_I2C, 1, 0),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 0, 0), LPC24XX_IO_INDEX_BY_PORT( 0, 2), LPC24XX_IO_ALTERNATE_2),
  LPC24XX_IO_HEADER( LPC24XX_MODULE_I2C, 1, 1),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 0, 19), LPC24XX_IO_INDEX_BY_PORT( 0, 21), LPC24XX_IO_ALTERNATE_2),
  LPC24XX_IO_HEADER( LPC24XX_MODULE_I2C, 1, 2),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 2, 14), LPC24XX_IO_INDEX_BY_PORT( 2, 16), LPC24XX_IO_ALTERNATE_2),
  LPC24XX_IO_HEADER( LPC24XX_MODULE_I2C, 2, 0),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 0, 10), LPC24XX_IO_INDEX_BY_PORT( 0, 12), LPC24XX_IO_ALTERNATE_1),
  LPC24XX_IO_HEADER( LPC24XX_MODULE_I2C, 2, 1),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 2, 30), LPC24XX_IO_INDEX_BY_PORT( 3, 0), LPC24XX_IO_ALTERNATE_2),
  LPC24XX_IO_HEADER( LPC24XX_MODULE_I2C, 2, 2),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 4, 20), LPC24XX_IO_INDEX_BY_PORT( 4, 22), LPC24XX_IO_ALTERNATE_1),

  /* USB */
  LPC24XX_IO_HEADER( LPC24XX_MODULE_USB, 0, 0),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 0, 29), LPC24XX_IO_INDEX_BY_PORT( 0, 31), LPC24XX_IO_ALTERNATE_0),
  LPC24XX_IO_ENTRY( LPC24XX_IO_INDEX_BY_PORT( 1, 19), LPC24XX_IO_INDEX_BY_PORT( 1, 20), LPC24XX_IO_ALTERNATE_1),
};

static size_t lpc24xx_io_get_entry_index( lpc24xx_module module, unsigned index, unsigned config)
{
  size_t i = 0;

  config |= LPC24XX_IO_HEADER_FLAG;

  for (i = 0; i < sizeof( lpc24xx_io_config_table); ++i) {
    const lpc24xx_io_entry *e = lpc24xx_io_config_table + i;

    if (e->function == config && e->begin == module && e->end == index) {
      return i + 1;
    }
  }

  return (size_t) -1;
}

static void lpc24xx_io_do_config( unsigned index, unsigned function)
{
  rtems_interrupt_level level;
  unsigned select = LPC24XX_IO_SELECT( index);
  unsigned shift = LPC24XX_IO_SELECT_SHIFT( index);

  rtems_interrupt_disable( level);

  LPC24XX_PINSEL [select] =
    (LPC24XX_PINSEL [select] & ~(LPC24XX_IO_SELECT_MASK << shift))
      | ((function & LPC24XX_IO_SELECT_MASK) << shift);

  rtems_interrupt_flash( level);

  LPC24XX_PINMODE [select] &= ~(LPC24XX_IO_SELECT_MASK << shift);

  rtems_interrupt_enable( level);
}

static void lpc24xx_io_do_release( unsigned index, unsigned function)
{
  rtems_interrupt_level level;
  unsigned select = LPC24XX_IO_SELECT( index);
  unsigned shift = LPC24XX_IO_SELECT_SHIFT( index);

  rtems_interrupt_disable( level);
  LPC24XX_PINSEL [select] =
    (LPC24XX_PINSEL [select] & ~(LPC24XX_IO_SELECT_MASK << shift));
  rtems_interrupt_enable( level);
}

static rtems_status_code lpc24xx_io_iterate(
  lpc24xx_module module,
  unsigned index,
  unsigned config,
  lpc24xx_io_iterate_routine routine
)
{
  size_t i = lpc24xx_io_get_entry_index( module, index, config);

  if (i != (size_t) -1) {
    const lpc24xx_io_entry * const table_end = lpc24xx_io_config_table
      + sizeof( lpc24xx_io_config_table) / sizeof( lpc24xx_io_config_table [0]);
    const lpc24xx_io_entry *e = lpc24xx_io_config_table + i;

    while (e != table_end && (e->function & LPC24XX_IO_HEADER_FLAG) == 0) {
      unsigned j = e->begin;
      unsigned end = e->end;
      unsigned function = e->function;

      while (j < end) {
        routine( j, function);

        ++j;
      }

      ++e;
    }
  } else {
    return RTEMS_INVALID_ID;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code lpc24xx_io_config(
  lpc24xx_module module,
  unsigned index,
  unsigned config
)
{
  return lpc24xx_io_iterate( module, index, config, lpc24xx_io_do_config);
}

rtems_status_code lpc24xx_io_release(
  lpc24xx_module module,
  unsigned index,
  unsigned config
)
{
  return lpc24xx_io_iterate( module, index, config, lpc24xx_io_do_release);
}

rtems_status_code lpc24xx_gpio_config(
  unsigned index,
  lpc24xx_gpio_settings settings
)
{
  if (index <= LPC24XX_IO_INDEX_MAX) {
    rtems_interrupt_level level;
    unsigned port = LPC24XX_IO_PORT( index);
    unsigned bit = LPC24XX_IO_PORT_BIT( index);
    unsigned select = LPC24XX_IO_SELECT( index);
    unsigned shift = LPC24XX_IO_SELECT_SHIFT( index);
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

    rtems_interrupt_disable( level);

    /* Resistor */
    LPC24XX_PINMODE [select] =
      (LPC24XX_PINMODE [select] & ~(LPC24XX_IO_SELECT_MASK << shift))
        | ((resistor & LPC24XX_IO_SELECT_MASK) << shift); 

    rtems_interrupt_flash( level);

    /* Input or output */
    LPC24XX_FIO [port].dir =
      (LPC24XX_FIO [port].dir & ~(1U << bit)) | (output << bit);

    rtems_interrupt_enable( level);
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

    rtems_interrupt_disable( level);
    PCONP |= 1U << power_bit;
    rtems_interrupt_enable( level);

    if (module != LPC24XX_MODULE_USB) {
      rtems_interrupt_disable( level);
      if (clock_shift < 32U) {
        PCLKSEL0 = (PCLKSEL0 & ~(LPC24XX_MODULE_CLOCK_MASK << clock_shift))
            | (clock << clock_shift);
      } else {
        clock_shift -= 32U;
        PCLKSEL1 = (PCLKSEL1 & ~(LPC24XX_MODULE_CLOCK_MASK << clock_shift))
            | (clock << clock_shift);
      }
      rtems_interrupt_enable( level);
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

    rtems_interrupt_disable( level);
    PCONP &= ~(1U << power_bit);
    rtems_interrupt_enable( level);
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code lpc24xx_module_enable(
  lpc24xx_module module,
  unsigned index,
  lpc24xx_module_clock clock
)
{
  return lpc24xx_module_do_enable( module, index, clock, true);
}

rtems_status_code lpc24xx_module_disable(
  lpc24xx_module module,
  unsigned index
)
{
  return lpc24xx_module_do_enable( module, index, 0U, false);
}
