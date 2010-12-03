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
 * <table>
 *   <tr><th>Module</th><th>Configuration</th><th>First Pin</th><th>Last Pin</th></tr>
 *   <tr><td>UART 0</td><td>0</td><td>P0.2</td><td>P0.3</td></tr>
 *   <tr><td rowspan=3>UART 1</td><td>0</td><td>P0.15</td><td>P0.16</td></tr>
 *   <tr><td>1</td><td>P2.0</td><td>P2.1</td></tr>
 *   <tr><td>2</td><td>P3.16</td><td>P3.17</td></tr>
 *   <tr><td rowspan=3>UART 2</td><td>0</td><td>P0.10</td><td>P0.11</td></tr>
 *   <tr><td>1</td><td>P2.8</td><td>P2.9</td></tr>
 *   <tr><td>2</td><td>P4.22</td><td>P4.23</td></tr>
 *   <tr><td rowspan=3>UART 3</td><td>0</td><td>P0.0</td><td>P0.1</td></tr>
 *   <tr><td>1</td><td>P0.25</td><td>P0.26</td></tr>
 *   <tr><td>2</td><td>P4.28</td><td>P4.29</td></tr>
 *   <tr><td rowspan=5>ETHERNET</td><td>0</td><td>P1.0</td><td>P1.17</td></tr>
 *   <tr><td rowspan=4>1</td><td>P1.0</td><td>P1.1</td></tr>
 *   <tr><td>P1.4</td><td>P1.4</td></tr>
 *   <tr><td>P1.8</td><td>P1.10</td></tr>
 *   <tr><td>P1.14</td><td>P1.17</td></tr>
 *   <tr><td rowspan=4>ADC</td><td>0</td><td>P0.12</td><td>P0.13</td></tr>
 *   <tr><td>1</td><td>P0.23</td><td>P0.25</td></tr>
 *   <tr><td rowspan=2>2</td><td>P0.26</td><td>P0.26</td></tr>
 *   <tr><td>P1.30</td><td>P1.31</td></tr>
 *   <tr><td>I2C 0</td><td>0</td><td>P0.27</td><td>P0.28</td></tr>
 *   <tr><td rowspan=3>I2C 1</td><td>0</td><td>P0.0</td><td>P0.1</td></tr>
 *   <tr><td>1</td><td>P0.19</td><td>P0.20</td></tr>
 *   <tr><td>2</td><td>P2.14</td><td>P2.15</td></tr>
 *   <tr><td rowspan=3>I2C 2</td><td>0</td><td>P0.10</td><td>P0.11</td></tr>
 *   <tr><td>1</td><td>P2.30</td><td>P2.31</td></tr>
 *   <tr><td>2</td><td>P4.20</td><td>P4.21</td></tr>
 *   <tr><td rowspan=3>I2S</td><td>0</td><td>P0.4</td><td>P0.9</td></tr>
 *   <tr><td rowspan=2>1</td><td>P0.23</td><td>P0.25</td></tr>
 *   <tr><td>P2.11</td><td>P2.13</td></tr>
 *   <tr><td rowspan=5>SSP 0</td><td>0</td><td>P0.15</td><td>P0.18</td></tr>
 *   <tr><td rowspan=2>1</td><td>P1.20</td><td>P0.21</td></tr>
 *   <tr><td>P1.23</td><td>P0.24</td></tr>
 *   <tr><td rowspan=2>2</td><td>P2.22</td><td>P2.23</td></tr>
 *   <tr><td>P2.26</td><td>P2.27</td></tr>
 *   <tr><td rowspan=5>SSP 1</td><td>0</td><td>P0.6</td><td>P0.9</td></tr>
 *   <tr><td rowspan=3>1</td><td>P0.12</td><td>P0.13</td></tr>
 *   <tr><td>P0.14</td><td>P0.14</td></tr>
 *   <tr><td>P1.31</td><td>P1.31</td></tr>
 *   <tr><td>2</td><td>P4.20</td><td>P4.23</td></tr>
 *   <tr><td rowspan=2>USB</td><td rowspan=2>0</td><td>P0.29</td><td>P0.30</td></tr>
 *   <tr><td>P1.19</td><td>P1.19</td></tr>
 *   <tr><td>SPI</td><td>0</td><td>P0.15</td><td>P0.18</td></tr>
 *   <tr><td>PWM 1</td><td>0</td><td>P2.0</td><td>P2.0</td></tr>
 *   <tr><td rowspan=11>LCD</td><td rowspan=6>0</td><td>P0.4</td><td>P0.9</td></tr>
 *   <tr><td>P1.20</td><td>P1.29</td></tr>
 *   <tr><td>P2.0</td><td>P2.3</td></tr>
 *   <tr><td>P2.5</td><td>P2.9</td></tr>
 *   <tr><td>P2.12</td><td>P2.13</td></tr>
 *   <tr><td>P4.28</td><td>P4.29</td></tr>
 *   <tr><td rowspan=5>1</td><td>P1.20</td><td>P1.29</td></tr>
 *   <tr><td>P2.0</td><td>P2.3</td></tr>
 *   <tr><td>P2.5</td><td>P2.9</td></tr>
 *   <tr><td>P2.12</td><td>P2.13</td></tr>
 *   <tr><td>P4.28</td><td>P4.29</td></tr>
 *   <tr><td>DAC</td><td>0</td><td>P0.26</td><td>P0.26</td></tr>
 * </table>
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

/**
 * @brief Applies the configuration with index @a config for the @a module.
 *
 * The pin mode will not be altered.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID Invalid module or configuration.
 */
rtems_status_code lpc24xx_io_config(
  lpc24xx_module module,
  unsigned config
);

/**
 * @brief Releases the configuration with index @a config for the @a module.
 *
 * The pins are set to general purpose IO function.  The pin mode will not be
 * altered.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID Invalid module or configuration.
 */
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
