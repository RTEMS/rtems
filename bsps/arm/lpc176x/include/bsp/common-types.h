/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief Definitions types used by some devices in common.
 */

/*
 * Copyright (c) 2014 Taller Technologies.
 *
 * @author  Boretto Martin    (martin.boretto@tallertechnologies.com)
 * @author  Diaz Marcos (marcos.diaz@tallertechnologies.com)
 * @author  Lenarduzzi Federico  (federico.lenarduzzi@tallertechnologies.com)
 * @author  Daniel Chicco  (daniel.chicco@tallertechnologies.com)
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_LPC176X_COMMON_TYPES_H
#define LIBBSP_ARM_LPC176X_COMMON_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief A pin of the board.
 */
typedef uint32_t lpc176x_pin_number;

/**
 * @brief Microseconds representation.
 */
typedef uint32_t lpc176x_microseconds;

/**
 * @brief lpc176x module representation.
 *
 * Enumerated type to define the set of modules for a lpc176x board.
 */
typedef enum {
  LPC176X_MODULE_WD,
  LPC176X_MODULE_ADC,
  LPC176X_MODULE_CAN_0,
  LPC176X_MODULE_CAN_1,
  LPC176X_MODULE_ACCF,
  LPC176X_MODULE_DAC,
  LPC176X_MODULE_GPDMA,
  LPC176X_MODULE_GPIO,
  LPC176X_MODULE_I2S,
  LPC176X_MODULE_MCI,
  LPC176X_MODULE_MCPWM,
  LPC176X_MODULE_PCB,
  LPC176X_MODULE_PWM_0,
  LPC176X_MODULE_PWM_1,
  LPC176X_MODULE_QEI,
  LPC176X_MODULE_RTC,
  LPC176X_MODULE_SYSCON,
  LPC176X_MODULE_TIMER_0,
  LPC176X_MODULE_TIMER_1,
  LPC176X_MODULE_TIMER_2,
  LPC176X_MODULE_TIMER_3,
  LPC176X_MODULE_UART_0,
  LPC176X_MODULE_UART_1,
  LPC176X_MODULE_UART_2,
  LPC176X_MODULE_UART_3,
  LPC176X_MODULE_USB
} lpc176x_module;

/**
 * @brief Defines all the clock modules.
 *
 * Enumerated type to define the set of clock modules for a lpc176x board.
 */
typedef enum {
  LPC176X_MODULE_PCLK_DEFAULT = 0x4U,
  LPC176X_MODULE_CCLK = 0x1U,
  LPC176X_MODULE_CCLK_2 = 0x2U,
  LPC176X_MODULE_CCLK_4 = 0x0U,
  LPC176X_MODULE_CCLK_6 = 0x3U,
  LPC176X_MODULE_CCLK_8 = 0x3U
} lpc176x_module_clock;

/**
 * @brief Fast Input/Output registers representation.
 */
typedef struct {
  /**
   * @brief Direction control register.
   */
  uint32_t dir;
  uint32_t reserved[ 3U ];
  /**
   * @brief Mask register for port.
   */
  uint32_t mask;
  /**
   * @brief Pinvalue register using 'mask'.
   */
  uint32_t pin;
  /**
   * @brief Output Set register using 'mask'.
   */
  uint32_t set;
  /**
   * @brief Output Clear register using 'maks'.
   */
  uint32_t clr;
} lpc176x_fio;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC176X_COMMON_TYPES_H */