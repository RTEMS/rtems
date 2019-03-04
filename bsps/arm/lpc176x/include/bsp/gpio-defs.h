/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief API definitions of the GPIO driver for the lpc176x bsp in RTEMS.
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

#ifndef LIBBSP_ARM_LPC176X_GPIO_DEFS_H
#define LIBBSP_ARM_LPC176X_GPIO_DEFS_H

#include <bsp/common-types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* General Purpose Input/Output (GPIO) */
#define LPC176X_GPIO_BASE_ADDR 0x40028000U
#define LPC176X_GPIO_INTERRUPT_STATUS 0x40028080U

#define LPC176X_IOPIN0 ( *(volatile uint32_t *) ( LPC176X_GPIO_BASE_ADDR + \
                                                  0x00U ) )
#define LPC176X_IOSET0 ( *(volatile uint32_t *) ( LPC176X_GPIO_BASE_ADDR + \
                                                  0x04U ) )
#define LPC176X_IODIR0 ( *(volatile uint32_t *) ( LPC176X_GPIO_BASE_ADDR + \
                                                  0x08U ) )
#define LPC176X_IOCLR0 ( *(volatile uint32_t *) ( LPC176X_GPIO_BASE_ADDR + \
                                                  0x0CU ) )
#define LPC176X_IOPIN1 ( *(volatile uint32_t *) ( LPC176X_GPIO_BASE_ADDR + \
                                                  0x10U ) )
#define LPC176X_IOSET1 ( *(volatile uint32_t *) ( LPC176X_GPIO_BASE_ADDR + \
                                                  0x14U ) )
#define LPC176X_IODIR1 ( *(volatile uint32_t *) ( LPC176X_GPIO_BASE_ADDR + \
                                                  0x18U ) )
#define LPC176X_IOCLR1 ( *(volatile uint32_t *) ( LPC176X_GPIO_BASE_ADDR + \
                                                  0x1CU ) )

/* GPIO Interrupt Registers */
#define LPC176X_IO0_INT_EN_R ( *(volatile uint32_t *) ( LPC176X_GPIO_BASE_ADDR \
                                                        + 0x90U ) )
#define LPC176X_IO0_INT_EN_F ( *(volatile uint32_t *) ( LPC176X_GPIO_BASE_ADDR \
                                                        + 0x94U ) )
#define LPC176X_IO0_INT_STAT_R ( *(volatile uint32_t *) ( \
                                   LPC176X_GPIO_BASE_ADDR \
                                   + 0x84U ) )
#define LPC176X_IO0_INT_STAT_F ( *(volatile uint32_t *) ( \
                                   LPC176X_GPIO_BASE_ADDR \
                                   + 0x88U ) )
#define LPC176X_IO0_INT_CLR ( *(volatile uint32_t *) ( LPC176X_GPIO_BASE_ADDR \
                                                       + 0x8CU ) )
#define LPC176X_IO2_INT_EN_R ( *(volatile uint32_t *) ( LPC176X_GPIO_BASE_ADDR \
                                                        + 0xB0U ) )
#define LPC176X_IO2_INT_EN_F ( *(volatile uint32_t *) ( LPC176X_GPIO_BASE_ADDR \
                                                        + 0xB4U ) )
#define LPC176X_IO2_INT_STAT_R ( *(volatile uint32_t *) ( \
                                   LPC176X_GPIO_BASE_ADDR \
                                   + 0xA4U ) )
#define LPC176X_IO2_INT_STAT_F ( *(volatile uint32_t *) ( \
                                   LPC176X_GPIO_BASE_ADDR \
                                   + 0xA8U ) )
#define LPC176X_IO2_INT_CLR ( *(volatile uint32_t *) ( LPC176X_GPIO_BASE_ADDR \
                                                       + 0xACU ) )
#define LPC176X_IO_INT_STAT ( *(volatile uint32_t *) ( LPC176X_GPIO_BASE_ADDR \
                                                       + 0x80U ) )

#define LPC176X_RESERVED_ISR_FUNCT_SIZE 2U
#define LPC176X_RESERVED_ISR_FUNCT_MAX_SIZE 5U

#define LPC176X_MAX_PORT_NUMBER 160U
#define LPC176X_SET_BIT( reg, pin, value ) \
  reg = ( reg & ~( 1U << pin ) ) | ( ( value & 1U ) << pin )

#define LPC176X_INT_STATUS ( *(volatile uint32_t *) \
                             ( LPC176X_GPIO_INTERRUPT_STATUS ) )
#define LPC176X_INT_STATUS_P0 1U
#define LPC176X_INT_STATUS_P2 ( 1U << 2U )
#define LPC176X_INT_ENABLE 1U
#define LPC176X_INT_DISABLE 0U

#define LPC176X_IRQ_EINT_3 21U

#define LPC176X_PIN_BIT( pin ) ( 1U << pin )

/**
 * @brief The direction of the GPIO port (input or output).
 *
 * Enumerated type to define the set of function types for a gpio device.
 */
typedef enum {
  LPC176X_GPIO_FUNCTION_INPUT,
  LPC176X_GPIO_FUNCTION_OUTPUT,
  LPC176X_GPIO_FUNCTION_COUNT
}
lpc176x_gpio_direction;

/**
 * @brief The interrupt sources edge for a GPIO.
 *
 * Enumerated type to define the set of interrupt types for a gpio device.
 */
typedef enum {
  LPC176X_GPIO_INTERRUPT_DISABLE,
  LPC176X_GPIO_INTERRUPT_RISING,
  LPC176X_GPIO_INTERRUPT_FALLING,
  LPC176X_GPIO_INTERRUPT_BOTH,
  LPC176X_GPIO_INTERRUPT_COUNT
} lpc176x_gpio_interrupt;

/**
 * @brief The ports for a GPIO.
 *
 * Enumerated type to define the set of ports for a gpio device.
 */
typedef enum {
  LPC176X_GPIO_PORT_0,
  LPC176X_GPIO_PORT_1,
  LPC176X_GPIO_PORT_2,
  LPC176X_GPIO_PORT_3,
  LPC176X_GPIO_PORT_4,
  LPC176X_GPIO_PORTS_COUNT
} lpc176x_gpio_ports;

/**
 * @brief Addresses for a GPIO.
 *
 * Enumerated type to define the set of fio bases addresses
 *     for a gpio device.
 */
typedef enum {
  LPC176X_FIO0_BASE_ADDRESS = 0x2009C000U,
  LPC176X_FIO1_BASE_ADDRESS = 0x2009C020U,
  LPC176X_FIO2_BASE_ADDRESS = 0x2009C040U,
  LPC176X_FIO3_BASE_ADDRESS = 0x2009C060U,
  LPC176X_FIO4_BASE_ADDRESS = 0x2009C080U,
} lpc176x_gpio_address;

/**
 * @brief Addresses for the two interrupts.
 *
 * Enumerated type to define the set of interrupt addresses
 *     for a gpio device.
 */
typedef enum {
  LPC176X_IO0_INT_BASE_ADDRESS = 0x40028084U,
  LPC176X_IO2_INT_BASE_ADDRESS = 0x400280A4U,
} lpc176x_interrupt_address;

/**
 * @brief GPIO Interrupt register map.
 */
typedef struct {
  /**
   * @brief Interrupt Enable for Rising edge.
   */
  volatile uint32_t StatR;
  /**
   * @brief Interrupt Enable for Falling edge.
   */
  volatile uint32_t StatF;
  /**
   * @brief Interrupt Clear.
   */
  volatile uint32_t Clr;
  /**
   * @brief Interrupt Enable for Rising edge.
   */
  volatile uint32_t EnR;
  /**
   * @brief Interrupt Enable for Falling edge.
   */
  volatile uint32_t EnF;
} lpc176x_interrupt_control;

/**
 * @brief A function that attends an interrupt for GPIO.
 *
 * @param  pin Pin number.
 * @param edge Interrupt.
 * @return Pointer to the interrupt function.
 */
typedef void (*lpc176x_gpio_interrupt_function) (
  const lpc176x_pin_number     pin,
  const lpc176x_gpio_interrupt edge
);

/**
 * @brief A registered interrupt function for the pin 'pin'.
 */
typedef struct {
  /**
   * @brief Pin board.
   */
  lpc176x_pin_number pin;
  /**
   * @brief A function that attends an interrupt for 'pin'.
   */
  lpc176x_gpio_interrupt_function function;
} lpc176x_registered_interrupt_function;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC176X_GPIO_DEFS_H */
