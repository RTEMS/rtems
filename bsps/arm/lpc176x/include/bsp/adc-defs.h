/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief ADC library for the lpc176x bsp.
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
#ifndef LPC176X_ADC_DEFS_H
#define LPC176X_ADC_DEFS_H

#include <bsp.h>
#include <bsp/io.h>
#include <bsp/lpc176x.h>
#include <bsp/adc.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief The ADC inputs of the board.
 */
typedef enum {
  ADC_0,
  ADC_1,
  ADC_2,
  ADC_3,
  ADC_4,
  ADC_5,
  ADC_6,
  ADC_7,
  ADC_DEVICES_COUNT
} lpc176x_adc_number;

#define MAX_ADC_CLK 13000000u

#define ADC_RANGE 0xFFFu

#define ADC_NUMBER_VALID( number ) ( ( (uint32_t) number ) < \
                                     ADC_DEVICES_COUNT )

#define ADC_CR_SEL( val ) BSP_FLD32( val, 0, 7 )
#define ADC_CR_SEL_GET( val ) BSP_FLD32GET( val, 0, 7 )
#define ADC_CR_SEL_SET( reg, val ) BSP_FLD32SET( reg, val, 0, 7 )
#define ADC_CR_CLKDIV( val ) BSP_FLD32( val, 8, 15 )
#define ADC_CR_CLKDIV_GET( reg ) BSP_FLD32GET( reg, 8, 15 )
#define ADC_CR_CLKDIV_SET( reg, val ) BSP_FLD32SET( reg, val, 8, 15 )
#define ADC_CR_BURST BSP_BIT32( 16 )
#define ADC_CR_CLKS( val ) BSP_FLD32( val, 17, 19 )
#define ADC_CR_PDN BSP_BIT32( 21 )
#define ADC_CR_START_NOW BSP_BIT32( 24 )
#define ADC_CR_START( val ) BSP_FLD32( val, 24, 26 )
#define ADC_CR_EDGE BSP_BIT32( 27 )

#define ADC_DR_VALUE( reg ) BSP_FLD32GET( reg, 4, 15 )
#define ADC_DR_OVERRUN BSP_BIT32( 30 )
#define ADC_DR_DONE BSP_BIT32( 31 )

#define ADC_DATA_CONVERSION_DONE( val ) ( ( val & ADC_DR_DONE ) != 0u )

/**
 * @brief The ADC low-level device.
 */
typedef struct {
  volatile uint32_t ADCR;
  volatile uint32_t ADGDR;
  volatile uint32_t RESERVED0;
  volatile uint32_t ADINTEN;
  volatile uint32_t ADDR[ ADC_DEVICES_COUNT ];
  volatile uint32_t ADSTAT;
  volatile uint32_t ADTRM;
} lpc176x_adc_device;

/**
 * @brief Represents the pin and function for each ADC input.
 */
typedef struct {
  uint32_t pin_number;
  lpc176x_pin_function pin_function;
} lpc176x_adc_pin_map;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
