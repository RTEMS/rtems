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

#ifndef LIBBSP_ARM_LPC24XX_IO_H
#define LIBBSP_ARM_LPC24XX_IO_H

#include <rtems.h>

#include <bsp/lpc24xx.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup lpc24xx_io IO Support and Configuration
 *
 * @ingroup lpc24xx
 *
 * @brief Input and output module.
 *
 * @{
 */

#define LPC24XX_IO_PORT_COUNT 5U

#define LPC24XX_IO_INDEX_MAX (LPC24XX_IO_PORT_COUNT * 32U)

#define LPC24XX_IO_INDEX_BY_PORT(port, bit) (((port) << 5U) + (bit))

#define LPC24XX_IO_PORT(index) (index >> 5U)

#define LPC24XX_IO_PORT_BIT(index) (index & 0x1fU)

typedef enum {
  LPC24XX_MODULE_ACF = 0,
  LPC24XX_MODULE_ADC,
  LPC24XX_MODULE_BAT_RAM,
  LPC24XX_MODULE_CAN_0,
  LPC24XX_MODULE_CAN_1,
  LPC24XX_MODULE_DAC,
  LPC24XX_MODULE_EMC,
  LPC24XX_MODULE_ETHERNET,
  LPC24XX_MODULE_GPDMA,
  LPC24XX_MODULE_GPIO,
  LPC24XX_MODULE_I2C_0,
  LPC24XX_MODULE_I2C_1,
  LPC24XX_MODULE_I2C_2,
  LPC24XX_MODULE_I2S,
  LPC24XX_MODULE_LCD,
  LPC24XX_MODULE_MCI,
  LPC24XX_MODULE_PCB,
  LPC24XX_MODULE_PWM_0,
  LPC24XX_MODULE_PWM_1,
  LPC24XX_MODULE_RTC,
  LPC24XX_MODULE_SPI,
  LPC24XX_MODULE_SSP_0,
  LPC24XX_MODULE_SSP_1,
  LPC24XX_MODULE_SYSCON,
  LPC24XX_MODULE_TIMER_0,
  LPC24XX_MODULE_TIMER_1,
  LPC24XX_MODULE_TIMER_2,
  LPC24XX_MODULE_TIMER_3,
  LPC24XX_MODULE_UART_0,
  LPC24XX_MODULE_UART_1,
  LPC24XX_MODULE_UART_2,
  LPC24XX_MODULE_UART_3,
  LPC24XX_MODULE_USB,
  LPC24XX_MODULE_WDT
} lpc24xx_module;

#define LPC24XX_MODULE_FIRST LPC24XX_MODULE_ACF

#define LPC24XX_MODULE_COUNT (LPC24XX_MODULE_WDT + 1)

typedef enum {
  LPC24XX_MODULE_PCLK_DEFAULT = 0x0U,
  LPC24XX_MODULE_CCLK = 0x1U,
  LPC24XX_MODULE_CCLK_2 = 0x2U,
  LPC24XX_MODULE_CCLK_4 = 0x0U,
  LPC24XX_MODULE_CCLK_6 = 0x3U,
  LPC24XX_MODULE_CCLK_8 = 0x3U
} lpc24xx_module_clock;

#define LPC24XX_MODULE_CLOCK_MASK 0x3U

typedef enum {
  LPC24XX_GPIO_DEFAULT = 0x0U,
  LPC24XX_GPIO_RESISTOR_DEFAULT = 0x0U,
  LPC24XX_GPIO_RESISTOR_NONE = 0x1U,
  LPC24XX_GPIO_RESISTOR_PULL_UP = 0x2U,
  LPC24XX_GPIO_RESISTOR_PULL_DOWN = 0x3U,
  LPC24XX_GPIO_INPUT = 0x0U,
  LPC24XX_GPIO_OUTPUT = 0x8U
} lpc24xx_gpio_settings;

#define LPC24XX_GPIO_RESISTOR_MASK 0x3U

rtems_status_code lpc24xx_module_enable(
  lpc24xx_module module,
  lpc24xx_module_clock clock
);

rtems_status_code lpc24xx_module_disable(
  lpc24xx_module module
);

rtems_status_code lpc24xx_io_config(
  lpc24xx_module module,
  unsigned config
);

rtems_status_code lpc24xx_io_release(
  lpc24xx_module module,
  unsigned config
);

rtems_status_code lpc24xx_gpio_config(
  unsigned index,
  lpc24xx_gpio_settings settings
);

static inline void lpc24xx_gpio_set(unsigned index)
{
  if (index <= LPC24XX_IO_INDEX_MAX) {
    unsigned port = LPC24XX_IO_PORT(index);
    unsigned bit = LPC24XX_IO_PORT_BIT(index);

    LPC24XX_FIO [port].set = 1U << bit;
  }
}

static inline void lpc24xx_gpio_clear(unsigned index)
{
  if (index <= LPC24XX_IO_INDEX_MAX) {
    unsigned port = LPC24XX_IO_PORT(index);
    unsigned bit = LPC24XX_IO_PORT_BIT(index);

    LPC24XX_FIO [port].clr = 1U << bit;
  }
}

static inline void lpc24xx_gpio_write(unsigned index, bool value)
{
  if (value) {
    lpc24xx_gpio_set(index);
  } else {
    lpc24xx_gpio_clear(index);
  }
}

static inline bool lpc24xx_gpio_get(unsigned index)
{
  if (index <= LPC24XX_IO_INDEX_MAX) {
    unsigned port = LPC24XX_IO_PORT(index);
    unsigned bit = LPC24XX_IO_PORT_BIT(index);

    return (LPC24XX_FIO [port].pin & (1U << bit)) != 0;
  } else {
    return false;
  }
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC24XX_IO_H */
