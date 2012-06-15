/**
 * @file
 *
 * @ingroup lpc24xx_io
 *
 * @brief Input and output module.
 */

/*
 * Copyright (c) 2009-2012 embedded brains GmbH.  All rights reserved.
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

#include <bsp.h>
#include <bsp/io.h>
#include <bsp/start.h>
#include <bsp/system-clocks.h>

#define LPC24XX_PIN_SELECT(index) ((index) >> 4U)

#define LPC24XX_PIN_SELECT_SHIFT(index) (((index) & 0xfU) << 1U)

#define LPC24XX_PIN_SELECT_MASK 0x3U

rtems_status_code lpc24xx_gpio_config(
  unsigned index,
  lpc24xx_gpio_settings settings
)
{
  if (index <= LPC24XX_IO_INDEX_MAX) {
    rtems_interrupt_level level;
    uint32_t port = LPC24XX_IO_PORT(index);
    uint32_t port_bit = LPC24XX_IO_PORT_BIT(index);
    uint32_t output = (settings & LPC24XX_GPIO_OUTPUT) != 0 ? 1U : 0U;
    uint32_t resistor = settings & 0x3U;
    #ifdef ARM_MULTILIB_ARCH_V4
      uint32_t select = LPC24XX_PIN_SELECT(index);
      uint32_t shift = LPC24XX_PIN_SELECT_SHIFT(index);

      /* Get resistor flags */
      switch (resistor) {
        case LPC24XX_GPIO_RESISTOR_PULL_UP:
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
    #else
      uint32_t iocon_mask = IOCON_HYS | IOCON_INV
        | IOCON_SLEW | IOCON_OD | IOCON_FILTER;
      uint32_t iocon = (settings & iocon_mask) | IOCON_ADMODE;
      uint32_t iocon_invalid = settings & ~(iocon_mask | LPC24XX_GPIO_OUTPUT);

      /* Get resistor flags */
      switch (resistor) {
        case LPC24XX_GPIO_RESISTOR_NONE:
          resistor = IOCON_MODE(0);
          break;
        case LPC24XX_GPIO_RESISTOR_PULL_DOWN:
          resistor = IOCON_MODE(1);
          break;
        case LPC24XX_GPIO_RESISTOR_PULL_UP:
          resistor = IOCON_MODE(2);
          break;
        case LPC17XX_GPIO_HYSTERESIS:
          resistor = IOCON_MODE(3);
          break;
      }
      iocon |= resistor;

      if (iocon_invalid != 0) {
        return RTEMS_INVALID_NUMBER;
      }

      if (output && (settings & LPC17XX_GPIO_INPUT_INVERT) != 0) {
        return RTEMS_INVALID_NUMBER;
      }

      if ((settings & LPC17XX_GPIO_INPUT_FILTER) == 0) {
        iocon |= IOCON_FILTER;
      } else {
        iocon &= ~IOCON_FILTER;
      }
    #endif

    rtems_interrupt_disable(level);

    #ifdef ARM_MULTILIB_ARCH_V4
      /* Resistor */
      LPC24XX_PINMODE [select] =
        (LPC24XX_PINMODE [select] & ~(LPC24XX_PIN_SELECT_MASK << shift))
          | ((resistor & LPC24XX_PIN_SELECT_MASK) << shift);
    #else
      LPC17XX_IOCON [index] = iocon;
    #endif

    rtems_interrupt_flash(level);

    /* Input or output */
    LPC24XX_FIO [port].dir =
      (LPC24XX_FIO [port].dir & ~(1U << port_bit)) | (output << port_bit);

    rtems_interrupt_enable(level);
  } else {
    return RTEMS_INVALID_ID;
  }

  return RTEMS_SUCCESSFUL;
}

#define LPC24XX_MODULE_ENTRY(mod, pwr, clk, idx) \
  [mod] = { \
    .power = pwr, \
    .clock = clk, \
    .index = idx \
  }

typedef struct {
  unsigned char power : 1;
  unsigned char clock : 1;
  unsigned char index : 6;
} lpc24xx_module_entry;

static const lpc24xx_module_entry lpc24xx_module_table [] = {
  #ifdef ARM_MULTILIB_ARCH_V4
    LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_ACF, 0, 1, 15),
  #endif
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_ADC, 1, 1, 12),
  #ifdef ARM_MULTILIB_ARCH_V4
    LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_BAT_RAM, 0, 1, 16),
  #endif
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_CAN_0, 1, 1, 13),
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_CAN_1, 1, 1, 14),
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_DAC, 0, 1, 11),
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_EMC, 1, 0, 11),
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_ETHERNET, 1, 0, 30),
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_GPDMA, 1, 1, 29),
  #ifdef ARM_MULTILIB_ARCH_V4
    LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_GPIO, 0, 1, 17),
  #else
    LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_GPIO, 0, 1, 15),
  #endif
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_I2C_0, 1, 1, 7),
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_I2C_1, 1, 1, 19),
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_I2C_2, 1, 1, 26),
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_I2S, 1, 1, 27),
  #ifdef ARM_MULTILIB_ARCH_V4
    LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_LCD, 1, 0, 20),
  #else
    LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_LCD, 1, 0, 0),
  #endif
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_MCI, 1, 1, 28),
  #ifdef ARM_MULTILIB_ARCH_V7M
    LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_MCPWM, 1, 1, 17),
  #endif
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_PCB, 0, 1, 18),
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_PWM_0, 1, 1, 5),
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_PWM_1, 1, 1, 6),
  #ifdef ARM_MULTILIB_ARCH_V7M
    LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_QEI, 1, 1, 18),
  #endif
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_RTC, 1, 1, 9),
  #ifdef ARM_MULTILIB_ARCH_V4
    LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_SPI, 1, 1, 8),
  #endif
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_SSP_0, 1, 1, 21),
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_SSP_1, 1, 1, 10),
  #ifdef ARM_MULTILIB_ARCH_V7M
    LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_SSP_2, 1, 1, 20),
  #endif
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_SYSCON, 0, 1, 30),
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_TIMER_0, 1, 1, 1),
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_TIMER_1, 1, 1, 2),
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_TIMER_2, 1, 1, 22),
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_TIMER_3, 1, 1, 23),
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_UART_0, 1, 1, 3),
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_UART_1, 1, 1, 4),
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_UART_2, 1, 1, 24),
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_UART_3, 1, 1, 25),
  #ifdef ARM_MULTILIB_ARCH_V7M
    LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_UART_4, 1, 1, 8),
  #endif
  #ifdef ARM_MULTILIB_ARCH_V4
    LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_WDT, 0, 1, 0),
  #endif
  LPC24XX_MODULE_ENTRY(LPC24XX_MODULE_USB, 1, 0, 31)
};

static rtems_status_code lpc24xx_module_do_enable(
  lpc24xx_module module,
  lpc24xx_module_clock clock,
  bool enable
)
{
  rtems_interrupt_level level;
  bool has_power = false;
  bool has_clock = false;
  unsigned index = 0;
  #ifdef ARM_MULTILIB_ARCH_V7M
    volatile lpc17xx_scb *scb = &LPC17XX_SCB;
  #endif

  if ((unsigned) module >= LPC24XX_MODULE_COUNT) {
      return RTEMS_INVALID_ID;
  }

  #ifdef ARM_MULTILIB_ARCH_V4
    if (clock == LPC24XX_MODULE_PCLK_DEFAULT) {
      #if LPC24XX_PCLKDIV == 1U
        clock = LPC24XX_MODULE_CCLK;
      #elif LPC24XX_PCLKDIV == 2U
        clock = LPC24XX_MODULE_CCLK_2;
      #elif LPC24XX_PCLKDIV == 4U
        clock = LPC24XX_MODULE_CCLK_4;
      #elif LPC24XX_PCLKDIV == 8U
        clock = LPC24XX_MODULE_CCLK_8;
      #endif
    }

    if ((clock & ~LPC24XX_MODULE_CLOCK_MASK) != 0U) {
      return RTEMS_INVALID_CLOCK;
    }
  #else
    if (clock != LPC24XX_MODULE_PCLK_DEFAULT) {
      return RTEMS_INVALID_CLOCK;
    }
  #endif

  has_power = lpc24xx_module_table [module].power;
  has_clock = lpc24xx_module_table [module].clock;
  index = lpc24xx_module_table [module].index;

  /* Enable or disable module */
  if (enable) {
    if (has_power) {
      rtems_interrupt_disable(level);
      #ifdef ARM_MULTILIB_ARCH_V4
        PCONP |= 1U << index;
      #else
        scb->pconp |= 1U << index;
      #endif
      rtems_interrupt_enable(level);
    }

    if (module != LPC24XX_MODULE_USB) {
      if (has_clock) {
        #ifdef ARM_MULTILIB_ARCH_V4
          unsigned clock_shift = 2U * index;

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
        #endif
      }
    } else {
      #ifdef ARM_MULTILIB_ARCH_V4
        unsigned pllclk = lpc24xx_pllclk();
        unsigned usbsel = pllclk / 48000000U - 1U;

        if (
          usbsel > 15U
            || (usbsel % 2U != 1U)
            || (pllclk % 48000000U) != 0U
        ) {
          return RTEMS_INCORRECT_STATE;
        }

        USBCLKCFG = usbsel;
      #else
        /* FIXME */
        scb->usbclksel = 0;
      #endif
    }
  } else {
    if (has_power) {
      rtems_interrupt_disable(level);
      #ifdef ARM_MULTILIB_ARCH_V4
        PCONP &= ~(1U << index);
      #else
        scb->pconp &= ~(1U << index);
      #endif
      rtems_interrupt_enable(level);
    }
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code lpc24xx_module_enable(
  lpc24xx_module module,
  lpc24xx_module_clock clock
)
{
  return lpc24xx_module_do_enable(module, clock, true);
}

rtems_status_code lpc24xx_module_disable(
  lpc24xx_module module
)
{
  return lpc24xx_module_do_enable(
    module,
    LPC24XX_MODULE_PCLK_DEFAULT,
    false
  );
}

bool lpc24xx_module_is_enabled(lpc24xx_module module)
{
  bool enabled = false;

  if ((unsigned) module < LPC24XX_MODULE_COUNT) {
    bool has_power = lpc24xx_module_table [module].power;

    if (has_power) {
      unsigned index = lpc24xx_module_table [module].index;
      #ifdef ARM_MULTILIB_ARCH_V4
        uint32_t pconp = PCONP;
      #else
        uint32_t pconp = LPC17XX_SCB.pconp;
      #endif

      enabled = (pconp & (1U << index)) != 0;
    } else {
      enabled = true;
    }
  }

  return enabled;
}

typedef rtems_status_code (*lpc24xx_pin_visitor)(
  #ifdef ARM_MULTILIB_ARCH_V4
    volatile uint32_t *pinsel,
    uint32_t pinsel_mask,
    uint32_t pinsel_value,
  #else
    volatile uint32_t *iocon,
    lpc24xx_pin_range pin_range,
  #endif
  volatile uint32_t *fio_dir,
  uint32_t fio_bit
);

static BSP_START_TEXT_SECTION __attribute__((flatten)) rtems_status_code
lpc24xx_pin_set_function(
  #ifdef ARM_MULTILIB_ARCH_V4
    volatile uint32_t *pinsel,
    uint32_t pinsel_mask,
    uint32_t pinsel_value,
  #else
    volatile uint32_t *iocon,
    lpc24xx_pin_range pin_range,
  #endif
  volatile uint32_t *fio_dir,
  uint32_t fio_bit
)
{
  #ifdef ARM_MULTILIB_ARCH_V4
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);
    *pinsel = (*pinsel & ~pinsel_mask) | pinsel_value;
    rtems_interrupt_enable(level);
  #else
    /* TODO */
    *iocon = IOCON_FUNC(pin_range.fields.function);
  #endif

  return RTEMS_SUCCESSFUL;
}

static BSP_START_TEXT_SECTION rtems_status_code lpc24xx_pin_check_function(
  #ifdef ARM_MULTILIB_ARCH_V4
    volatile uint32_t *pinsel,
    uint32_t pinsel_mask,
    uint32_t pinsel_value,
  #else
    volatile uint32_t *iocon,
    lpc24xx_pin_range pin_range,
  #endif
  volatile uint32_t *fio_dir,
  uint32_t fio_bit
)
{
  #ifdef ARM_MULTILIB_ARCH_V4
    if ((*pinsel & pinsel_mask) == pinsel_value) {
      return RTEMS_SUCCESSFUL;
    } else {
      return RTEMS_IO_ERROR;
    }
  #else
    /* TODO */
    return RTEMS_IO_ERROR;
  #endif
}

static BSP_START_TEXT_SECTION __attribute__((flatten)) rtems_status_code
lpc24xx_pin_set_input(
  #ifdef ARM_MULTILIB_ARCH_V4
    volatile uint32_t *pinsel,
    uint32_t pinsel_mask,
    uint32_t pinsel_value,
  #else
    volatile uint32_t *iocon,
    lpc24xx_pin_range pin_range,
  #endif
  volatile uint32_t *fio_dir,
  uint32_t fio_bit
)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  *fio_dir &= ~fio_bit;
  #ifdef ARM_MULTILIB_ARCH_V4
    *pinsel &= ~pinsel_mask;
  #else
    *iocon = IOCON_MODE(2) | IOCON_ADMODE | IOCON_FILTER;
  #endif
  rtems_interrupt_enable(level);

  return RTEMS_SUCCESSFUL;
}

static BSP_START_TEXT_SECTION rtems_status_code lpc24xx_pin_check_input(
  #ifdef ARM_MULTILIB_ARCH_V4
    volatile uint32_t *pinsel,
    uint32_t pinsel_mask,
    uint32_t pinsel_value,
  #else
    volatile uint32_t *iocon,
    lpc24xx_pin_range pin_range,
  #endif
  volatile uint32_t *fio_dir,
  uint32_t fio_bit
)
{
  rtems_status_code sc = RTEMS_IO_ERROR;
  bool is_input = (*fio_dir & fio_bit) == 0;

  if (is_input) {
    #ifdef ARM_MULTILIB_ARCH_V4
      bool is_gpio = (*pinsel & pinsel_mask) == 0;
    #else
      bool is_gpio = IOCON_FUNC_GET(*iocon) == 0;
    #endif

    if (is_gpio) {
      sc = RTEMS_SUCCESSFUL;
    }
  }

  return sc;
}

static BSP_START_DATA_SECTION const lpc24xx_pin_visitor
  lpc24xx_pin_visitors [] = {
  [LPC24XX_PIN_SET_FUNCTION] = lpc24xx_pin_set_function,
  [LPC24XX_PIN_CHECK_FUNCTION] = lpc24xx_pin_check_function,
  [LPC24XX_PIN_SET_INPUT] = lpc24xx_pin_set_input,
  [LPC24XX_PIN_CHECK_INPUT] = lpc24xx_pin_check_input
};

BSP_START_TEXT_SECTION rtems_status_code lpc24xx_pin_config(
  const lpc24xx_pin_range *pins,
  lpc24xx_pin_action action
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if ((unsigned) action <= LPC24XX_PIN_CHECK_INPUT) {
    lpc24xx_pin_visitor visitor = lpc24xx_pin_visitors [action];
    lpc24xx_pin_range terminal = LPC24XX_PIN_TERMINAL;
    lpc24xx_pin_range pin_range = *pins;
    uint32_t previous_port_bit = pin_range.fields.port_bit;

    while (sc == RTEMS_SUCCESSFUL && pin_range.value != terminal.value) {
      uint32_t port = pin_range.fields.port;
      uint32_t port_bit = pin_range.fields.port_bit;
      uint32_t port_bit_last = port_bit;
      uint32_t range = pin_range.fields.range;
      #ifdef ARM_MULTILIB_ARCH_V4
        uint32_t function = pin_range.fields.function;
      #endif
      volatile uint32_t *fio_dir = &LPC24XX_FIO [port].dir;

      if (range) {
        port_bit = previous_port_bit;
      }

      while (sc == RTEMS_SUCCESSFUL && port_bit <= port_bit_last) {
        uint32_t index = LPC24XX_IO_INDEX_BY_PORT(port, port_bit);
        uint32_t fio_bit = 1U << port_bit;
        #ifdef ARM_MULTILIB_ARCH_V4
          uint32_t select = LPC24XX_PIN_SELECT(index);
          uint32_t shift = LPC24XX_PIN_SELECT_SHIFT(index);
          volatile uint32_t *pinsel = &LPC24XX_PINSEL [select];
          uint32_t pinsel_mask = LPC24XX_PIN_SELECT_MASK << shift;
          uint32_t pinsel_value = (function & LPC24XX_PIN_SELECT_MASK) << shift;

          sc = (*visitor)(pinsel, pinsel_mask, pinsel_value, fio_dir, fio_bit);
        #else
          volatile uint32_t *iocon = &LPC17XX_IOCON [index];

          sc = (*visitor)(iocon, pin_range, fio_dir, fio_bit);
        #endif

        ++port_bit;
      }

      ++pins;
      previous_port_bit = port_bit;
      pin_range = *pins;
    }
  } else {
    sc = RTEMS_NOT_DEFINED;
  }

  return sc;
}
