/**
 * @file adc.c
 *
 * @ingroup lpc176x
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
#include <rtems/status-checks.h>
#include <bsp/adc.h>
#include <bsp/adc-defs.h>

static lpc176x_adc_device *const adc_device =
  (lpc176x_adc_device *) AD0_BASE_ADDR;

static const lpc176x_adc_pin_map adc_pinmap[ ADC_DEVICES_COUNT ] =
{
  {
    .pin_number = 23u,
    .pin_function = LPC176X_PIN_FUNCTION_01
  },
  {
    .pin_number = 24u,
    .pin_function = LPC176X_PIN_FUNCTION_01
  },
  {
    .pin_number = 25u,
    .pin_function = LPC176X_PIN_FUNCTION_01
  },
  {
    .pin_number = 26u,
    .pin_function = LPC176X_PIN_FUNCTION_01
  },
  {
    .pin_number = 62u,
    .pin_function = LPC176X_PIN_FUNCTION_11
  },
  {
    .pin_number = 63u,
    .pin_function = LPC176X_PIN_FUNCTION_11
  },
  {
    .pin_number = 3u,
    .pin_function = LPC176X_PIN_FUNCTION_10
  },
  {
    .pin_number = 2u,
    .pin_function = LPC176X_PIN_FUNCTION_10
  }
};

/**
 * @brief Checks for a valid pin number for an ADC
 *
 * @param pin_number The pin to check
 * @param adc_number The returned ADC device corresponding to that pin.
 *
 * @return true if valid, false otherwise.
 */
static bool valid_pin_number (
  const lpc176x_pin_number pin_number,
  lpc176x_adc_number *const adc_number
  )
{
  bool found = false;
  lpc176x_adc_number adc_device = ADC_0;

  while (!found && (adc_device < ADC_DEVICES_COUNT))
  {
    if (adc_pinmap[adc_device].pin_number == pin_number)
    {
      *adc_number = adc_device;
      found = true;
    }
    ++adc_device;
  }

  return found;
}

/**
 * @brief Turns on the device and sets its clock divisor.
 *
 */
static void turn_on_and_set_clkdiv( void )
{
  const uint32_t clkdiv = LPC176X_CCLK / ( LPC176X_PCLKDIV * MAX_ADC_CLK );

  adc_device->ADCR = ADC_CR_PDN | ADC_CR_CLKDIV( clkdiv );
}

rtems_status_code adc_open( const lpc176x_pin_number pin_number )
{
  rtems_status_code sc = RTEMS_INVALID_NUMBER;
  lpc176x_adc_number adc_number = 0;
  if ( valid_pin_number( pin_number, &adc_number ) ) {
    sc =
      lpc176x_module_enable( LPC176X_MODULE_ADC, LPC176X_MODULE_PCLK_DEFAULT );
    RTEMS_CHECK_SC( sc, "enable adc module" );

    turn_on_and_set_clkdiv();
    lpc176x_pin_select( adc_pinmap[ adc_number ].pin_number,
      adc_pinmap[ adc_number ].pin_function );
    lpc176x_pin_set_mode( adc_pinmap[ adc_number ].pin_number,
      LPC176X_PIN_MODE_NONE );
  }

  return sc;
}

rtems_status_code adc_close( void )
{
  adc_device->ADCR &= ~ADC_CR_PDN;

  return lpc176x_module_disable( LPC176X_MODULE_ADC );
}

/**
 * @brief Starts the conversion for the given channel.
 *
 * @param number The channel to start the conversion.
 */
static inline void start_conversion( const lpc176x_adc_number number )
{
  adc_device->ADCR =
    ADC_CR_SEL_SET( adc_device->ADCR, ( 1 << number ) ) | ADC_CR_START_NOW;
}

/**
 * @brief Stops the conversion.
 *
 */
static inline void stop_conversion( void )
{
  adc_device->ADCR &= ~ADC_CR_START_NOW;
}

/**
 * @brief Gets float percentage of the result of a conversion.
 *
 * @param data The result of a conversion.
 * @return A float percentage (between 0.0f and 1.0f).
 */
static inline float get_float( const uint32_t data )
{
  return ( (float) data / (float) ADC_RANGE );
}


/**
 * @brief Reads the ADC value for the given ADC number.
 *
 * @param adc_number Which ADC device read.
 * @return The read value.
 */
static uint32_t read( const lpc176x_adc_number adc_number )
{
  uint32_t data;

  start_conversion( adc_number );

  do {
    data = adc_device->ADGDR;
  } while ( !ADC_DATA_CONVERSION_DONE( data ) );

  stop_conversion();

  return ADC_DR_VALUE( data );
}

/**
 * @brief Checks if the passed parameters are ordered from lowest
 *  to highest.
 *
 *  @param a The suggested lowest value.
 *  @param b The suggested middle value.
 *  @param c The suggested highest value.
 *  @return  True if it is in the right order, false otherwise.
 */
static inline bool lowest_to_highest_ordered(
  const uint32_t a,
  const uint32_t b,
  const uint32_t c
)
{
  return ( ( a <= b ) && ( b <= c ) );
}

/**
 * @brief Gets median value from the three passed parameters.
 *
 * @param  a The first parameter.
 * @param  b The second parameter.
 * @param  c The third parameter.
 *
 * @return  The median of the three parameters.
 */
static uint32_t get_median(
  const uint32_t a,
  const uint32_t b,
  const uint32_t c
)
{
  uint32_t median;

  if ( lowest_to_highest_ordered( a, b, c)
              || lowest_to_highest_ordered( c, b, a ) ) {
    median = b;
  } else if ( lowest_to_highest_ordered( b, a, c )
              || lowest_to_highest_ordered( c, a, b ) ) {
    median = a;
  } else {
    median = c;
  }

  return median;
}

rtems_status_code adc_read(
  const lpc176x_pin_number pin_number ,
  float *const             result
)
{
  rtems_status_code sc = RTEMS_INVALID_NUMBER;
  lpc176x_adc_number adc_number = 0;
  if ( valid_pin_number( pin_number, &adc_number ) ) {
    const uint32_t first = read( adc_number );
    const uint32_t second = read( adc_number );
    const uint32_t third = read( adc_number );
    const uint32_t median = get_median( first, second, third );
    *result = get_float( median );
    sc = RTEMS_SUCCESSFUL;
  }

  return sc;
}
