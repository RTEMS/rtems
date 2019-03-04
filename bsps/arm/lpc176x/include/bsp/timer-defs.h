/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief API definitions of the for the timer of the lpc176x bsp.
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

#ifndef LIBBSP_ARM_LPC176X_TIMER_DEFS_H
#define LIBBSP_ARM_LPC176X_TIMER_DEFS_H

#include <bsp/common-types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Timer 0 */
#define LPC176X_TMR0_BASE_ADDR 0x40004000U

#define LPC176X_T0IR ( *(volatile uint32_t *) ( LPC176X_TMR0_BASE_ADDR + \
                                                0x00U ) )
#define LPC176X_T0TCR ( *(volatile uint32_t *) ( LPC176X_TMR0_BASE_ADDR + \
                                                 0x04U ) )
#define LPC176X_T0TC ( *(volatile uint32_t *) ( LPC176X_TMR0_BASE_ADDR + \
                                                0x08U ) )
#define LPC176X_T0PR ( *(volatile uint32_t *) ( LPC176X_TMR0_BASE_ADDR + \
                                                0x0CU ) )
#define LPC176X_T0PC ( *(volatile uint32_t *) ( LPC176X_TMR0_BASE_ADDR + \
                                                0x10U ) )
#define LPC176X_T0MCR ( *(volatile uint32_t *) ( LPC176X_TMR0_BASE_ADDR + \
                                                 0x14U ) )
#define LPC176X_T0MR0 ( *(volatile uint32_t *) ( LPC176X_TMR0_BASE_ADDR + \
                                                 0x18U ) )
#define LPC176X_T0MR1 ( *(volatile uint32_t *) ( LPC176X_TMR0_BASE_ADDR + \
                                                 0x1CU ) )
#define LPC176X_T0MR2 ( *(volatile uint32_t *) ( LPC176X_TMR0_BASE_ADDR + \
                                                 0x20U ) )
#define LPC176X_T0MR3 ( *(volatile uint32_t *) ( LPC176X_TMR0_BASE_ADDR + \
                                                 0x24U ) )
#define LPC176X_T0CCR ( *(volatile uint32_t *) ( LPC176X_TMR0_BASE_ADDR + \
                                                 0x28U ) )
#define LPC176X_T0CR0 ( *(volatile uint32_t *) ( LPC176X_TMR0_BASE_ADDR + \
                                                 0x2CU ) )
#define LPC176X_T0CR1 ( *(volatile uint32_t *) ( LPC176X_TMR0_BASE_ADDR + \
                                                 0x30U ) )
#define LPC176X_T0CR2 ( *(volatile uint32_t *) ( LPC176X_TMR0_BASE_ADDR + \
                                                 0x34U ) )
#define LPC176X_T0CR3 ( *(volatile uint32_t *) ( LPC176X_TMR0_BASE_ADDR + \
                                                 0x38U ) )
#define LPC176X_T0EMR ( *(volatile uint32_t *) ( LPC176X_TMR0_BASE_ADDR + \
                                                 0x3CU ) )
#define LPC176X_T0CTCR ( *(volatile uint32_t *) ( LPC176X_TMR0_BASE_ADDR + \
                                                  0x70U ) )

/* Timer 1 */
#define LPC176X_TMR1_BASE_ADDR 0x40008000U

#define LPC176X_T1IR ( *(volatile uint32_t *) ( LPC176X_TMR1_BASE_ADDR + \
                                                0x00U ) )
#define LPC176X_T1TCR ( *(volatile uint32_t *) ( LPC176X_TMR1_BASE_ADDR + \
                                                 0x04U ) )
#define LPC176X_T1TC ( *(volatile uint32_t *) ( LPC176X_TMR1_BASE_ADDR + \
                                                0x08U ) )
#define LPC176X_T1PR ( *(volatile uint32_t *) ( LPC176X_TMR1_BASE_ADDR + \
                                                0x0CU ) )
#define LPC176X_T1PC ( *(volatile uint32_t *) ( LPC176X_TMR1_BASE_ADDR + \
                                                0x10U ) )
#define LPC176X_T1MCR ( *(volatile uint32_t *) ( LPC176X_TMR1_BASE_ADDR + \
                                                 0x14U ) )
#define LPC176X_T1MR0 ( *(volatile uint32_t *) ( LPC176X_TMR1_BASE_ADDR + \
                                                 0x18U ) )
#define LPC176X_T1MR1 ( *(volatile uint32_t *) ( LPC176X_TMR1_BASE_ADDR + \
                                                 0x1CU ) )
#define LPC176X_T1MR2 ( *(volatile uint32_t *) ( LPC176X_TMR1_BASE_ADDR + \
                                                 0x20U ) )
#define LPC176X_T1MR3 ( *(volatile uint32_t *) ( LPC176X_TMR1_BASE_ADDR + \
                                                 0x24U ) )
#define LPC176X_T1CCR ( *(volatile uint32_t *) ( LPC176X_TMR1_BASE_ADDR + \
                                                 0x28U ) )
#define LPC176X_T1CR0 ( *(volatile uint32_t *) ( LPC176X_TMR1_BASE_ADDR + \
                                                 0x2CU ) )
#define LPC176X_T1CR1 ( *(volatile uint32_t *) ( LPC176X_TMR1_BASE_ADDR + \
                                                 0x30U ) )
#define LPC176X_T1CR2 ( *(volatile uint32_t *) ( LPC176X_TMR1_BASE_ADDR + \
                                                 0x34U ) )
#define LPC176X_T1CR3 ( *(volatile uint32_t *) ( LPC176X_TMR1_BASE_ADDR + \
                                                 0x38U ) )
#define LPC176X_T1EMR ( *(volatile uint32_t *) ( LPC176X_TMR1_BASE_ADDR + \
                                                 0x3CU ) )
#define LPC176X_T1CTCR ( *(volatile uint32_t *) ( LPC176X_TMR1_BASE_ADDR + \
                                                  0x70U ) )

/* Timer 2 */
#define LPC176X_TMR2_BASE_ADDR 0x40090000U

#define LPC176X_T2IR ( *(volatile uint32_t *) ( LPC176X_TMR2_BASE_ADDR + \
                                                0x00U ) )
#define LPC176X_T2TCR ( *(volatile uint32_t *) ( LPC176X_TMR2_BASE_ADDR + \
                                                 0x04U ) )
#define LPC176X_T2TC ( *(volatile uint32_t *) ( LPC176X_TMR2_BASE_ADDR + \
                                                0x08U ) )
#define LPC176X_T2PR ( *(volatile uint32_t *) ( LPC176X_TMR2_BASE_ADDR + \
                                                0x0CU ) )
#define LPC176X_T2PC ( *(volatile uint32_t *) ( LPC176X_TMR2_BASE_ADDR + \
                                                0x10U ) )
#define LPC176X_T2MCR ( *(volatile uint32_t *) ( LPC176X_TMR2_BASE_ADDR + \
                                                 0x14U ) )
#define LPC176X_T2MR0 ( *(volatile uint32_t *) ( LPC176X_TMR2_BASE_ADDR + \
                                                 0x18U ) )
#define LPC176X_T2MR1 ( *(volatile uint32_t *) ( LPC176X_TMR2_BASE_ADDR + \
                                                 0x1CU ) )
#define LPC176X_T2MR2 ( *(volatile uint32_t *) ( LPC176X_TMR2_BASE_ADDR + \
                                                 0x20U ) )
#define LPC176X_T2MR3 ( *(volatile uint32_t *) ( LPC176X_TMR2_BASE_ADDR + \
                                                 0x24U ) )
#define LPC176X_T2CCR ( *(volatile uint32_t *) ( LPC176X_TMR2_BASE_ADDR + \
                                                 0x28U ) )
#define LPC176X_T2CR0 ( *(volatile uint32_t *) ( LPC176X_TMR2_BASE_ADDR + \
                                                 0x2CU ) )
#define LPC176X_T2CR1 ( *(volatile uint32_t *) ( LPC176X_TMR2_BASE_ADDR + \
                                                 0x30U ) )
#define LPC176X_T2CR2 ( *(volatile uint32_t *) ( LPC176X_TMR2_BASE_ADDR + \
                                                 0x34U ) )
#define LPC176X_T2CR3 ( *(volatile uint32_t *) ( LPC176X_TMR2_BASE_ADDR + \
                                                 0x38U ) )
#define LPC176X_T2EMR ( *(volatile uint32_t *) ( LPC176X_TMR2_BASE_ADDR + \
                                                 0x3CU ) )
#define LPC176X_T2CTCR ( *(volatile uint32_t *) ( LPC176X_TMR2_BASE_ADDR + \
                                                  0x70U ) )

/* Timer 3 */
#define LPC176X_TMR3_BASE_ADDR 0x40094000U

#define LPC176X_T3IR ( *(volatile uint32_t *) ( LPC176X_TMR3_BASE_ADDR + \
                                                0x00U ) )
#define LPC176X_T3TCR ( *(volatile uint32_t *) ( LPC176X_TMR3_BASE_ADDR + \
                                                 0x04U ) )
#define LPC176X_T3TC ( *(volatile uint32_t *) ( LPC176X_TMR3_BASE_ADDR + \
                                                0x08U ) )
#define LPC176X_T3PR ( *(volatile uint32_t *) ( LPC176X_TMR3_BASE_ADDR + \
                                                0x0CU ) )
#define LPC176X_T3PC ( *(volatile uint32_t *) ( LPC176X_TMR3_BASE_ADDR + \
                                                0x10U ) )
#define LPC176X_T3MCR ( *(volatile uint32_t *) ( LPC176X_TMR3_BASE_ADDR + \
                                                 0x14U ) )
#define LPC176X_T3MR0 ( *(volatile uint32_t *) ( LPC176X_TMR3_BASE_ADDR + \
                                                 0x18U ) )
#define LPC176X_T3MR1 ( *(volatile uint32_t *) ( LPC176X_TMR3_BASE_ADDR + \
                                                 0x1CU ) )
#define LPC176X_T3MR2 ( *(volatile uint32_t *) ( LPC176X_TMR3_BASE_ADDR + \
                                                 0x20U ) )
#define LPC176X_T3MR3 ( *(volatile uint32_t *) ( LPC176X_TMR3_BASE_ADDR + \
                                                 0x24U ) )
#define LPC176X_T3CCR ( *(volatile uint32_t *) ( LPC176X_TMR3_BASE_ADDR + \
                                                 0x28U ) )
#define LPC176X_T3CR0 ( *(volatile uint32_t *) ( LPC176X_TMR3_BASE_ADDR + \
                                                 0x2CU ) )
#define LPC176X_T3CR1 ( *(volatile uint32_t *) ( LPC176X_TMR3_BASE_ADDR + \
                                                 0x30U ) )
#define LPC176X_T3CR2 ( *(volatile uint32_t *) ( LPC176X_TMR3_BASE_ADDR + \
                                                 0x34U ) )
#define LPC176X_T3CR3 ( *(volatile uint32_t *) ( LPC176X_TMR3_BASE_ADDR + \
                                                 0x38U ) )
#define LPC176X_T3EMR ( *(volatile uint32_t *) ( LPC176X_TMR3_BASE_ADDR + \
                                                 0x3CU ) )
#define LPC176X_T3CTCR ( *(volatile uint32_t *) ( LPC176X_TMR3_BASE_ADDR + \
                                                  0x70U ) )

/**
 * @brief Represents the timer device registers.
 */
typedef struct {
  /**
   * @brief Interrupt Register.
   */
  volatile uint32_t IR;
  /**
   * @brief Timer Control Register.
   */
  volatile uint32_t TCR;
  /**
   * @brief Timer Counter.
   */
  volatile uint32_t TC;
  /**
   * @brief Prescale Register.
   */
  volatile uint32_t PR;
  /**
   * @brief Prescale Counter.
   */
  volatile uint32_t PC;
  /**
   * @brief Match Control Register.
   */
  volatile uint32_t MCR;
  /**
   * @brief Match Register (0, 1, 2, 3)
   */
  volatile uint32_t MR[ 4 ];
  /**
   * @brief Capture Control Register.
   */
  volatile uint32_t CCR;
  /**
   * @brief Capture Register (0, 1)
   */
  volatile uint32_t CR[ 2 ];
  volatile uint32_t reserved0;
  volatile uint32_t reserved1;
  /**
   * @brief External Match Register.
   */
  volatile uint32_t EMR;
  volatile uint32_t reserved2[ 12 ];
  /**
   * @brief Count Control Register.
   */
  volatile uint32_t CTCR;
} lpc176x_timer_device;

#define LPC176X_PIN_SELECT_TIMER 3U
#define LPC176X_PINSEL_NO_PORT 999U

#define LPC176X_TIMER_RESET ( 1U << 1U )
#define LPC176X_TIMER_START 1U
#define LPC176X_TIMER_MODE_COUNTER_SOURCE_CAP0 0U
#define LPC176X_TIMER_MODE_COUNTER_SOURCE_CAP1 ( 1U << 2U )
#define LPC176X_TIMER0_CAPTURE_PORTS { 58U, 59U }
#define LPC176X_TIMER1_CAPTURE_PORTS { 50U, 51U }
#define LPC176X_TIMER2_CAPTURE_PORTS { 4U, 5U }
#define LPC176X_TIMER3_CAPTURE_PORTS { 23U, 24U }
#define LPC176X_TIMER0_EMATCH_PORTS { 60U,   \
                                      61U,    \
                                      LPC176X_PINSEL_NO_PORT, \
                                      LPC176X_PINSEL_NO_PORT }
#define LPC176X_TIMER1_EMATCH_PORTS { 54U, \
                                      57U,    \
                                      LPC176X_PINSEL_NO_PORT, \
                                      LPC176X_PINSEL_NO_PORT }
#define LPC176X_TIMER2_EMATCH_PORTS { 6U, 7U, 8U, 9U }
#define LPC176X_TIMER3_EMATCH_PORTS { 10U, \
                                      11U, \
                                      LPC176X_PINSEL_NO_PORT, \
                                      LPC176X_PINSEL_NO_PORT }
#define LPC176X_TIMER_DEFAULT_RESOLUTION 1U
#define LPC176X_TIMER_MCR_MASK 7U
#define LPC176X_TIMER_MCR_MASK_SIZE 3U
#define LPC176X_TIMER_CCR_MASK 7U
#define LPC176X_TIMER_CCR_MASK_SIZE 3U
#define LPC176X_TIMER_EMR_MASK 3U
#define LPC176X_TIMER_EMR_MASK_SIZE 2U
#define LPC176X_TIMER_EMR_MASK_OFFSET 4U
#define LPC176X_TIMER_CLEAR_FUNCTION 0U
#define LPC176X_TIMER_PRESCALER_DIVISOR 1000000U
#define LPC176X_TIMER_VECTOR_NUMBER( timernumber ) ( timernumber + 1U )
#define LPC176X_TIMER_INTERRUPT_SOURCE_BIT( i ) ( 1U << i )
#define LPC176X_TIMER_MATCH_FUNCTION_COUNT 8U
#define LPC176X_TIMER_CAPTURE_FUNCTION_COUNT 8U

#define LPC176X_ISR_NAME_STRING_SIZE 10U

#define LPC176X_SET_MCR( mcr, match_port, function )  \
  SET_FIELD( mcr, \
  function, \
  ( 0x7U << ( 3U * match_port ) ), \
  ( 3U * match_port ) )
#define LPC176X_SET_CCR( mcr, capture_port, function )  \
  SET_FIELD( mcr, function, ( 0x7U << ( 3U * capture_port ) ), \
  ( 3U * capture_port ) )
#define LPC176X_SET_EMR( mcr, match_port, function )  \
  SET_FIELD( mcr, function, ( 0x3U << ( 2U * match_port + 4U ) ), \
  ( 2U * match_port + 4U ) )

/**
 * @brief Capture ports of a timer.
 *
 * Enumerated type to define the set of capture ports for a timer device.
 */
typedef enum {
  LPC176X_CAPn_0,
  LPC176X_CAPn_1,
  LPC176X_CAPTURE_PORTS_COUNT
} lpc176x_capture_port;

/**
 * @brief Match ports of a timer.
 *
 * Enumerated type to define the set of match ports for a timer device.
 */
typedef enum {
  LPC176X_MATn_0,
  LPC176X_MATn_1,
  LPC176X_MATn_2,
  LPC176X_MATn_3,
  LPC176X_EMATCH_PORTS_COUNT
} lpc176x_match_port;

/**
 * @brief Timer modes of a timer.
 *
 * Enumerated type to define the set of modes for a timer device.
 */
typedef enum {
  LPC176X_TIMER_MODE_TIMER,
  LPC176X_TIMER_MODE_COUNTER_RISING_CAP0,
  LPC176X_TIMER_MODE_COUNTER_FALLING_CAP0,
  LPC176X_TIMER_MODE_COUNTER_BOTH_CAP0,
  LPC176X_TIMER_MODE_COUNTER_RISING_CAP1 = ( 1U & ( 1U << 2U ) ),
  LPC176X_TIMER_MODE_COUNTER_FALLING_CAP1 = ( 2U & ( 1U << 2U ) ),
  LPC176X_TIMER_MODE_COUNTER_BOTH_CAP1 = ( 3U & ( 1U << 2U ) ),
} lpc176x_timer_mode;

/**
 * @brief The timer devices in the board.
 *
 * Enumerated type to define the timer device's numbers.
 */
typedef enum {
  LPC176X_TIMER_0,
  LPC176X_TIMER_1,
  LPC176X_TIMER_2,
  LPC176X_TIMER_3,
  LPC176X_TIMER_COUNT
} lpc176x_timer_number;

/**
 * @brief The index for the isr_funct_vector representing the functions
 *        that attends each possible interrupt source for a timer.
 *
 * Enumerated type to define the set of  isr timer functions .
 */
typedef enum {
  LPC176X_MAT0_ISR_FUNCTION,
  LPC176X_MAT1_ISR_FUNCTION,
  LPC176X_MAT2_ISR_FUNCTION,
  LPC176X_MAT3_ISR_FUNCTION,
  LPC176X_CAP0_ISR_FUNCTION,
  LPC176X_CAP1_ISR_FUNCTION,
  LPC176X_ISR_FUNCTIONS_COUNT
} lpc176x_isr_function;

/**
 * @brief The possible functions at match. This options could be
 *        used together.
 *
 * Enumerated type to define the set of  functions at mach for a
 *     timer device.
 */
typedef enum {
  LPC176X_TIMER_MATCH_FUNCTION_NONE = 0U,
  LPC176X_TIMER_MATCH_FUNCTION_INTERRUPT = 1U,
  LPC176X_TIMER_MATCH_FUNCTION_RESET = ( 1U << 1U ),
  LPC176X_TIMER_MATCH_FUNCTION_STOP = ( 1U << 2U )
} lpc176x_match_function;

/**
 * @brief The possible functions at capture. This options could
 *        be used together.
 *
 * Enumerated type to define the set of  functions at capture for
 *     a timer device.
 */
typedef enum {
  LPC176X_TIMER_CAPTURE_FUNCTION_NONE = 0U,
  LPC176X_TIMER_CAPTURE_FUNCTION_RISING = 1U,
  LPC176X_TIMER_CAPTURE_FUNCTION_FALLING = ( 1U << 1U ),
  LPC176X_TIMER_CAPTURE_FUNCTION_INTERRUPT = ( 1U << 2U )
} lpc176x_capture_function;

/**
 * @brief The possible functions at match, for the external ports.
 *
 * Enumerated type to define the set of  functions at match, for external
 *     ports, for a timer device.
 */
typedef enum {
  LPC176X_TIMER_EXTMATCH_FUNCTION_NONE,
  LPC176X_TIMER_EXTMATCH_FUNCTION_CLEAR,
  LPC176X_TIMER_EXTMATCH_FUNCTION_SET,
  LPC176X_TIMER_EXTMATCH_FUNCTION_TOGGLE
} lpc176x_ext_match_function;

/**
 * @brief A function that attends an interruption for a timer.
 *
 * @param  tnumber Timer number.
 * @return Pointer to the match function.
 */
typedef void (*lpc176x_isr_funct) ( const lpc176x_timer_number tnumber );

/**
 * @brief The vector of functions that attends each possible interrupt
 *        source for a timer.
 */
typedef lpc176x_isr_funct const lpc176x_isr_funct_vector[
    LPC176X_ISR_FUNCTIONS_COUNT ];

/**
 * @brief The Timer device representation.
 */
typedef struct {
  /**
   * @brief The address of the controlling registers for the timer.
   */
  lpc176x_timer_device *const device;
  /**
   * @brief The module for the RTEMS module starting (power and clock).
   */
  const lpc176x_module module;
  /**
   * @brief The Pins for the Capture ports of this timer.
   */
  const lpc176x_pin_number pinselcap[ LPC176X_CAPTURE_PORTS_COUNT ];
  /**
   * @brief The Pins for the external match ports of this timer.
   */
  const lpc176x_pin_number pinselemat[ LPC176X_EMATCH_PORTS_COUNT ];
} lpc176x_timer;

/**
 * @brief The Timer functions.
 */
typedef struct {
  /**
   * @brief The vector of isr functions for this timer.
   */
  const lpc176x_isr_funct_vector *funct_vector;
} lpc176x_timer_functions;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC176X_TIMER_DEFS_H */