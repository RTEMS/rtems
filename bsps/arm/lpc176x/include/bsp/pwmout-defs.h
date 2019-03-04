/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief PWM-Out controller for the mbed lpc1768 board.
 */

/*
 * Copyright (c) 2014 Taller Technologies.
 *
 * @author  Diaz Marcos (marcos.diaz@tallertechnologies.com)
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LPC176X_PWMOUT_DEFS_H
#define LPC176X_PWMOUT_DEFS_H

#include <bsp/lpc176x.h>
#include <bsp/pwmout.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define PWM_DEFAULT_PERIOD 20000u
#define PWM_DEFAULT_PULSEWIDTH 0u

#define PWM_PRESCALER_USECOND ( LPC176X_CCLK / 1000000 )
#define PWM_MCR_RESET_ON_MATCH0 ( 1 << 1 )
#define PWM_PCR_ENABLE_PWM( pwmout ) ( 1 << ( 9 + pwmout ) )
#define PWM_TCR_RESET ( 1 << 1 )
#define PWM_TCR_ENABLE ( 1u )
#define PWM_TCR_PWM ( 1 << 3 )
#define PWM_LER_LATCH_MATCH_0 1u
#define PWM_LER_LATCH( match ) ( 1 << ( ( match ) + 1 ) )

/**
 * @brief The low-level PWM output device.
 */
typedef struct {
  volatile uint32_t IR;
  volatile uint32_t TCR;
  volatile uint32_t TC;
  volatile uint32_t PR;
  volatile uint32_t PC;
  volatile uint32_t MCR;
  volatile uint32_t MR0;
  volatile uint32_t MR1;
  volatile uint32_t MR2;
  volatile uint32_t MR3;
  volatile uint32_t CCR;
  volatile uint32_t CR0;
  volatile uint32_t CR1;
  volatile uint32_t CR2;
  volatile uint32_t CR3;
  volatile uint32_t RESERVED0;
  volatile uint32_t MR4;
  volatile uint32_t MR5;
  volatile uint32_t MR6;
  volatile uint32_t PCR;
  volatile uint32_t LER;
  volatile uint32_t RESERVED1[ 7 ];
  volatile uint32_t CTCR;
} lpc176x_pwm_device;

/**
 * @brief Represents one pin and the respective function to be set
 *  for each PWM output.
 */
typedef struct {
  uint32_t pin_number;
  lpc176x_pin_function pin_function;
} lpc176x_pwm_pin;

/**
 * @brief The PWM outputs of the board.
 */
typedef enum {
  PWMO_1,
  PWMO_2,
  PWMO_3,
  PWMO_4,
  PWMO_5,
  PWMO_6,
  PWM_OUTPUT_NUMBER
} lpc176x_pwm_number;

/**
 * @brief A pin for each PWM output.
 */
typedef enum {
  PWM_FIRST_PIN,
  PWM_SECOND_PIN,
  PWM_NUMBER_OF_PINS
} lpc176x_pwm_pin_number;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
