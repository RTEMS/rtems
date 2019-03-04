/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief Input/output module definitions.
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

#ifndef LIBBSP_ARM_LPC176X_IO_DEFS_H
#define LIBBSP_ARM_LPC176X_IO_DEFS_H

#include <bsp/lpc176x.h>
#include <bsp/common-types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LPC176X_PLL0CON 0XAAU
#define LPC176X_PLL0CFG 0X55U

#define LPC176X_CCLK_PRESCALER_DIVISOR 1000000U

#define LPC176X_PINSEL ( &PINSEL0 )
#define LPC176X_PINMODE ( &PINMODE0 )

#define LPC176X_PIN_SELECT( index ) ( ( index ) >> 4U )
#define LPC176X_PIN_SELECT_SHIFT( index ) ( ( ( index ) & 0xFU ) << 1U )
#define LPC176X_PIN_SELECT_MASK 0x3U
#define LPC176X_PIN_SELECT_MASK_SIZE 2U
#define LPC176X_PIN_UART_0_TXD 2U
#define LPC176X_PIN_UART_0_RXD 3U
#define LPC176X_PIN_UART_1_TXD 15U
#define LPC176X_PIN_UART_1_RXD 16U
#define LPC176X_PIN_UART_2_TXD 10U
#define LPC176X_PIN_UART_2_RXD 11U
#define LPC176X_PIN_UART_3_TXD 0U
#define LPC176X_PIN_UART_3_RXD 1U

#define LPC176X_MODULE_BITS_COUNT 32U
#define LPC176X_MODULE_COUNT ( LPC176X_MODULE_USB + 1U )

#define LPC176X_IO_PORT_COUNT 5U
#define LPC176X_IO_INDEX_MAX ( LPC176X_IO_PORT_COUNT *       \
                               LPC176X_MODULE_BITS_COUNT )
#define LPC176X_IO_INDEX_BY_PORT( port, bit ) ( ( ( port ) << 5U ) + ( bit ) )
#define LPC176X_IO_PORT( index ) ( ( index ) >> 5U )
#define LPC176X_IO_PORT_BIT( index ) ( ( index ) & 0x1FU )

/**
 * @brief Defines the functions according to the pin.
 *
 * Enumerated type to define the set of pin function for a io device.
 */
typedef enum {
  LPC176X_PIN_FUNCTION_00,
  LPC176X_PIN_FUNCTION_01,
  LPC176X_PIN_FUNCTION_10,
  LPC176X_PIN_FUNCTION_11,
  LPC176X_PIN_FUNCTION_COUNT
}
lpc176x_pin_function;

/**
 * @brief Defines the pin modes.
 *
 */
typedef enum {
  LPC176X_PIN_MODE_PULLUP,
  LPC176X_PIN_MODE_REPEATER,
  LPC176X_PIN_MODE_NONE,
  LPC176X_PIN_MODE_PULLDOWN,
  LPC176X_PIN_MODE_COUNT
}
lpc176x_pin_mode;

/**
 * @brief Defines all type of pins.
 *
 * Enumerated type to define the set of pin type for a io device.
 */
typedef enum {
  LPC176X_PIN_TYPE_DEFAULT,
  LPC176X_PIN_TYPE_ADC,
  LPC176X_PIN_TYPE_DAC,
  LPC176X_PIN_TYPE_OPEN_DRAIN
} lpc176x_pin_type;

/**
 * @brief Represents each pclksel number.
 *
 * Enumerated type to define the set of values for a pcklsel.
 */
typedef enum {
  LPC176X_SCB_PCLKSEL0,
  LPC176X_SCB_PCLKSEL1,
  LPC176X_SCB_PCLKSEL_COUNT
} lpc176x_scb_value_pclksel;

/**
 * @brief Defines the module entry.
 */
typedef struct {
  /**
   * @brief Power entry bit.
   */
  unsigned char power : 1;
  /**
   * @brief Clock entry bit.
   */
  unsigned char clock : 1;
  /**
   * @brief Index entry bits.
   */
  unsigned char index : 6;
} lpc176x_module_entry;

#define LPC176X_MODULE_ENTRY( mod, pwr, clk, idx )  \
  [ mod ] = {                                       \
    .power = pwr,                                 \
    .clock = clk,                                 \
    .index = idx                                  \
  }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC176X_IO_DEFS_H */
