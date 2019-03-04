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

#ifndef LPC176X_ADC_H
#define LPC176X_ADC_H

#include <bsp.h>
#include <bsp/common-types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Opens and initializes the ADC device.
 *
 * @param adc_number The ADC pin number to be initialized.
 * @return RTEMS_SUCCESSFUL if the initialization was succesful,
 *  RTEMS_INVALID_NUMBER if wrong parameter.
 */
rtems_status_code adc_open( const lpc176x_pin_number pin_number );

/**
 * @brief Closes the ADC device.
 *
 * @return RTEMS_SUCCESSFUL if closed succesfully.
 */
rtems_status_code adc_close( void );

/**
 * @brief Starts a conversion, waits for it to finish and reads the value.
 *
 * @param pin_number The port to read the value.
 * @param result The read result. (In a percentage between 0.0f and 1.0f).
 */
rtems_status_code adc_read(
  const lpc176x_pin_number pin_number ,
  float *const             result
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
