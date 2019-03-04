/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief GPIO library for the lpc176x bsp.
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

#include <assert.h>
#include <bsp/irq.h>
#include <bsp/io.h>
#include <bsp/lpc-gpio.h>
#include <rtems/status-checks.h>

static uint32_t                              function_vector_size = 0u;
static lpc176x_registered_interrupt_function function_vector[
  LPC176X_RESERVED_ISR_FUNCT_SIZE ];
static bool isr_installed = false;

rtems_status_code lpc176x_gpio_config(
  const lpc176x_pin_number     pin,
  const lpc176x_gpio_direction dir
)
{
  rtems_status_code status_code = RTEMS_INVALID_NUMBER;

  if ( ( pin < LPC176X_MAX_PORT_NUMBER ) &&
       ( dir < LPC176X_GPIO_FUNCTION_COUNT ) ) {
    const lpc176x_gpio_ports port = LPC176X_IO_PORT( pin );
    const uint32_t           pin_of_port = LPC176X_IO_PORT_BIT( pin );

    lpc176x_pin_select( pin, LPC176X_PIN_FUNCTION_00 );

    LPC176X_SET_BIT( LPC176X_FIO[ port ].dir, pin_of_port, dir );

    status_code = RTEMS_SUCCESSFUL;
  }

  /* else implies that the pin or the egde are out of range. Also,
     an invalid number is returned. */

  return status_code;
}

/**
 * @brief Check for a rising edge and call the interrupt function.
 *
 * @param statR Rising edge interrupt.
 * @param pin The pin to check.
 * @param registered_isr_function Interrupt to check.
 * @return TRUE if is a rising edge. FALSE otherwise.
 */
static bool lpc176x_check_rising_edge_and_call(
  const uint32_t                              statR,
  const lpc176x_registered_interrupt_function registered_isr_function,
  const uint32_t                              pin
)
{
  bool is_rising = false;

  if ( statR & LPC176X_PIN_BIT( pin ) ) {
    registered_isr_function.function( registered_isr_function.pin,
      LPC176X_GPIO_INTERRUPT_RISING );
    is_rising = true;
  }

  /* else implies that the current interrupt is not STATR. Also,
     there is nothing to do. */

  return is_rising;
}

/**
 * @brief Check for a falling edge and call the interrupt function.
 *
 * @param statR Falling edge interrupt.
 * @param pin The pin to check.
 * @param registered_isr_function Interrupt to check.
 * @return TRUE if is a falling edge. FALSE otherwise.
 */
static bool lpc176x_check_falling_edge_and_call(
  const uint32_t                              statF,
  const lpc176x_registered_interrupt_function registered_isr_function,
  const uint32_t                              pin
)
{
  bool is_falling = false;

  if ( statF & LPC176X_PIN_BIT( pin ) ) {
    registered_isr_function.function( registered_isr_function.pin,
      LPC176X_GPIO_INTERRUPT_FALLING );
    is_falling = true;
  }

  /* else implies that the current interrupt is not STATF. Also,
     there is nothing to do. */

  return is_falling;
}

/**
 * @brief Returns the interrupts base address according to the current port.
 *
 * @param  port Input/Output port.
 * @return Interrupt base address.
 */
static lpc176x_interrupt_control*lpc176x_get_interrupt_address(
  const lpc176x_gpio_ports port )
{
  lpc176x_interrupt_control *interrupt;

  switch ( port ) {
    case ( LPC176X_GPIO_PORT_0 ):
      interrupt = (lpc176x_interrupt_control *) LPC176X_IO0_INT_BASE_ADDRESS;
      break;
    case ( LPC176X_GPIO_PORT_2 ):
      interrupt = (lpc176x_interrupt_control *) LPC176X_IO2_INT_BASE_ADDRESS;
      break;
    case ( LPC176X_GPIO_PORT_1 ):
    case ( LPC176X_GPIO_PORT_3 ):
    case ( LPC176X_GPIO_PORT_4 ):
    default:
      interrupt = NULL;
  }

  return interrupt;
}

/**
 * @brief Checks the type of the current interrupt.
 *
 * @param registered_isr_function Interrupt to check.
 */
static void check_for_interrupt(
  const lpc176x_registered_interrupt_function registered_isr_function )
{
  assert( registered_isr_function.pin < LPC176X_MAX_PORT_NUMBER );

  const lpc176x_gpio_ports port = LPC176X_IO_PORT(
    registered_isr_function.pin );
  const uint32_t pin = LPC176X_IO_PORT_BIT( registered_isr_function.pin );

  lpc176x_interrupt_control *interrupt = lpc176x_get_interrupt_address( port );
  assert( interrupt != NULL );

  bool is_rising_edge = lpc176x_check_rising_edge_and_call( interrupt->StatR,
    registered_isr_function,
    pin );

  bool is_falling_edge = lpc176x_check_falling_edge_and_call( interrupt->StatF,
    registered_isr_function,
    pin );

  if ( is_rising_edge || is_falling_edge ) {
    interrupt->Clr = LPC176X_PIN_BIT( pin );
  }

  /* else implies that the current interrupt is not CLR. Also,
     there is nothing to do. */
}

/**
 * @brief Checks all interrupts types.
 *
 * @param arg Interrupt to check.
 */
static inline void lpc176x_gpio_isr( void *arg )
{
  unsigned int i;

  for ( i = 0; i < function_vector_size; ++i ) {
    check_for_interrupt( function_vector[ i ] );
  }
}

/**
 * @brief Depending of the current edge sets rising/falling interrupt.
 *
 * @param edge Current edge.
 * @param pin_of_port Pin of the port to set the interrupt.
 * @param interrupt To enable the falling o rising edge.
 */
static void lpc176x_set_falling_or_rising_interrupt(
  const lpc176x_gpio_interrupt edge,
  const uint32_t               pin_of_port,
  lpc176x_interrupt_control   *interrupt
)
{
  if ( edge & LPC176X_GPIO_INTERRUPT_RISING ) {
    LPC176X_SET_BIT( interrupt->EnR, pin_of_port, LPC176X_INT_ENABLE );
  }

  /* else implies that it should not install the interrupt for a RISING edge.
      Also, there is nothing to do. */

  if ( edge & LPC176X_GPIO_INTERRUPT_FALLING ) {
    LPC176X_SET_BIT( interrupt->EnF, pin_of_port, LPC176X_INT_ENABLE );
  }

  /* else implies that it should not install the interrupt for a FALLING edge.
      Also, there is nothing to do. */
}

/**
 * @brief Registers the pin and the callbacks functions.
 *
 * @param edge Current edge.
 * @param pin The pin to configure.
 * @param isr_funct Callback function to set.
 */
static void lpc176x_register_pin_and_callback(
  const lpc176x_gpio_interrupt          edge,
  const lpc176x_pin_number              pin,
  const lpc176x_gpio_interrupt_function isr_funct
)
{
  if ( edge ) {
    assert( function_vector_size < LPC176X_RESERVED_ISR_FUNCT_SIZE );
    function_vector[ function_vector_size ].function = isr_funct;
    function_vector[ function_vector_size ].pin = pin;
    ++function_vector_size;
  }

  /* else implies that the current interrupt is DISABLED or BOTH. Also,
     there is nothing to do. */
}

/**
 * @brief Installs the interrupt handler.
 *
 * @param edge Which edge enable.
 * @return  RTEMS_SUCCESSFUL if the installation was success.
 */
static rtems_status_code lpc176x_install_interrupt_handler(
  const lpc176x_gpio_interrupt edge )
{
  rtems_status_code status_code = RTEMS_SUCCESSFUL;

  if ( !isr_installed && edge ) {
    status_code = rtems_interrupt_handler_install( LPC176X_IRQ_EINT_3,
      "gpio_interrupt",
      RTEMS_INTERRUPT_UNIQUE,
      lpc176x_gpio_isr,
      NULL );
    isr_installed = true;
  }

  /* else implies that the interrupts have been previously installed. Also,
     there is nothing to do. */

  return status_code;
}

/**
 * @brief Configures the pin as input, enables interrupt for an
 * edge/s and sets isrfunct as the function to call when that
 * interrupt occurs.
 *
 * @param pin The pin to configure.
 * @param edge Which edge or edges will activate the interrupt.
 * @param isrfunct The function that is called when the interrupt occurs.
 * @return RTEMS_SUCCESSFUL if the configuration was success.
 */
static rtems_status_code lpc176x_check_edge_and_set_gpio_interrupts(
  const lpc176x_pin_number              pin,
  const lpc176x_gpio_interrupt          edge,
  const lpc176x_gpio_interrupt_function isr_funct
)
{
  rtems_status_code status_code = RTEMS_SUCCESSFUL;

  const lpc176x_gpio_ports port = LPC176X_IO_PORT( pin );
  const uint32_t           pin_of_port = LPC176X_IO_PORT_BIT( pin );

  lpc176x_interrupt_control *interrupt = lpc176x_get_interrupt_address( port );

  assert( interrupt != NULL );

  lpc176x_gpio_config( pin, LPC176X_GPIO_FUNCTION_INPUT );

  lpc176x_set_falling_or_rising_interrupt( edge, pin_of_port, interrupt );

  lpc176x_register_pin_and_callback( edge, pin, isr_funct );

  status_code = lpc176x_install_interrupt_handler( edge );

  return status_code;
}

rtems_status_code lpc176x_gpio_config_input_with_interrupt(
  const lpc176x_pin_number              pin,
  const lpc176x_gpio_interrupt          edge,
  const lpc176x_gpio_interrupt_function isr_funct
)
{
  rtems_status_code status_code = RTEMS_INVALID_NUMBER;

  if ( ( pin < LPC176X_MAX_PORT_NUMBER )
       && ( edge < LPC176X_GPIO_INTERRUPT_COUNT ) ) {
    status_code = lpc176x_check_edge_and_set_gpio_interrupts( pin,
      edge,
      isr_funct );
  }

  /* else implies that the pin or the egde are out of range. Also,
     an invalid number is returned. */

  return status_code;
}

rtems_status_code lpc176x_gpio_set_pin( const lpc176x_pin_number pin )
{
  rtems_status_code status_code = RTEMS_INVALID_NUMBER;

  if ( pin < LPC176X_MAX_PORT_NUMBER ) {
    const lpc176x_gpio_ports port = LPC176X_IO_PORT( pin );
    const uint32_t           pin_of_port = LPC176X_IO_PORT_BIT( pin );

    LPC176X_FIO[ port ].set = LPC176X_PIN_BIT( pin_of_port );

    status_code = RTEMS_SUCCESSFUL;
  }

  /* else implies that the pin or the egde are out of range. Also,
     an invalid number is returned. */

  return status_code;
}

rtems_status_code lpc176x_gpio_clear_pin( const lpc176x_pin_number pin )
{
  rtems_status_code status_code = RTEMS_INVALID_NUMBER;

  if ( pin < LPC176X_MAX_PORT_NUMBER ) {
    const lpc176x_gpio_ports port = LPC176X_IO_PORT( pin );
    const uint32_t           pin_of_port = LPC176X_IO_PORT_BIT( pin );

    LPC176X_FIO[ port ].clr = LPC176X_PIN_BIT( pin_of_port );

    status_code = RTEMS_SUCCESSFUL;
  }

  /* else implies that the pin or the egde are out of range. Also,
     an invalid number is returned. */

  return status_code;
}

rtems_status_code lpc176x_gpio_write_pin(
  const lpc176x_pin_number pin,
  const bool               value
)
{
  rtems_status_code status_code;

  if ( value ) {
    status_code = lpc176x_gpio_set_pin( pin );
  } else {
    status_code = lpc176x_gpio_clear_pin( pin );
  }

  return status_code;
}

inline rtems_status_code lpc176x_gpio_get_pin_value(
  const lpc176x_pin_number pin,
  bool                    *pin_value
)
{
  assert( pin < LPC176X_MAX_PORT_NUMBER );

  rtems_status_code status_code = RTEMS_SUCCESSFUL;

  const lpc176x_gpio_ports port = LPC176X_IO_PORT( pin );
  const uint32_t           pin_of_port = LPC176X_IO_PORT_BIT( pin );
  *pin_value = ( LPC176X_FIO[ port ].pin & LPC176X_PIN_BIT( pin_of_port ) );

  return status_code;
}
