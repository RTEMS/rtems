/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief Timer controller for the mbed lpc1768 board.
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

#include <stdio.h>
#include <rtems/status-checks.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/io.h>
#include <bsp/timer.h>

/**
 * @brief Represents all the timers.
 */
const lpc176x_timer timers[ LPC176X_TIMER_COUNT ] =
{
  {
    .device = (lpc176x_timer_device *) LPC176X_TMR0_BASE_ADDR,
    .module = LPC176X_MODULE_TIMER_0,
    .pinselcap = LPC176X_TIMER0_CAPTURE_PORTS,
    .pinselemat = LPC176X_TIMER0_EMATCH_PORTS,
  },
  {
    .device = (lpc176x_timer_device *) LPC176X_TMR1_BASE_ADDR,
    .module = LPC176X_MODULE_TIMER_1,
    .pinselcap = LPC176X_TIMER1_CAPTURE_PORTS,
    .pinselemat = LPC176X_TIMER1_EMATCH_PORTS,
  },
  {
    .device = (lpc176x_timer_device *) LPC176X_TMR2_BASE_ADDR,
    .module = LPC176X_MODULE_TIMER_2,
    .pinselcap = LPC176X_TIMER2_CAPTURE_PORTS,
    .pinselemat = LPC176X_TIMER2_EMATCH_PORTS,
  },
  {
    .device = (lpc176x_timer_device *) LPC176X_TMR3_BASE_ADDR,
    .module = LPC176X_MODULE_TIMER_3,
    .pinselcap = LPC176X_TIMER3_CAPTURE_PORTS,
    .pinselemat = LPC176X_TIMER3_EMATCH_PORTS,
  }
};

/**
 * @brief Represents all the functions according to the timers.
 */
lpc176x_timer_functions functions_vector[ LPC176X_TIMER_COUNT ] =
{
  {
    .funct_vector = NULL
  },
  {
    .funct_vector = NULL
  },
  {
    .funct_vector = NULL
  },
  {
    .funct_vector = NULL
  }
};

/**
 * @brief Calls the corresponding interrupt function and pass the timer
 *        as parameter.
 *
 * @param  timer The specific device.
 * @param  interruptnumber Interrupt number.
 */
static inline void lpc176x_call_desired_isr(
  const lpc176x_timer_number number,
  const lpc176x_isr_function interruptfunction
)
{
  if ( ( *functions_vector[ number ].funct_vector )[ interruptfunction ] !=
       NULL ) {
    ( *functions_vector[ number ].funct_vector )[ interruptfunction ]( number );
  }

  /* else implies that the function vector points NULL. Also,
     there is nothing to do. */
}

/**
 * @brief Gets true if the selected interrupt is pending
 *
 * @param number: the number of the timer.
 * @param interrupt: the interrupt we are checking for.
 * @return TRUE if the interrupt is pending.
 */
static inline bool lpc176x_timer_interrupt_is_pending(
  const lpc176x_timer_number tnumber,
  const lpc176x_isr_function function
)
{
  assert( ( tnumber < LPC176X_TIMER_COUNT )
    && ( function < LPC176X_ISR_FUNCTIONS_COUNT ) );

  return ( timers[ tnumber ].device->IR &
           LPC176X_TIMER_INTERRUPT_SOURCE_BIT( function ) );
}

/**
 * @brief Resets interrupt status for the selected interrupt
 *
 * @param tnumber: the number of the timer
 * @param interrupt: the interrupt we are resetting
 */
static inline void lpc176x_timer_reset_interrupt(
  const lpc176x_timer_number tnumber,
  const lpc176x_isr_function function
)
{
  assert( ( tnumber < LPC176X_TIMER_COUNT )
    && ( function < LPC176X_ISR_FUNCTIONS_COUNT ) );
  timers[ tnumber ].device->IR =
    LPC176X_TIMER_INTERRUPT_SOURCE_BIT( function );
}

inline rtems_status_code lpc176x_timer_reset(
  const lpc176x_timer_number tnumber )
{
  rtems_status_code status_code = RTEMS_INVALID_NUMBER;

  if ( tnumber < LPC176X_TIMER_COUNT ) {
    timers[ tnumber ].device->TCR = LPC176X_TIMER_RESET;
    status_code = RTEMS_SUCCESSFUL;
  }

  /* else implies that the timer number is invalid. Also,
     an invalid number is returned. */

  return status_code;
}

inline rtems_status_code lpc176x_timer_set_mode(
  const lpc176x_timer_number tnumber,
  const lpc176x_timer_mode   mode
)
{
  rtems_status_code status_code = RTEMS_INVALID_NUMBER;

  if ( tnumber < LPC176X_TIMER_COUNT ) {
    timers[ tnumber ].device->CTCR = mode;
    status_code = RTEMS_SUCCESSFUL;
  }

  /* else implies that the timer number is invalid. Also,
     an invalid number is returned. */

  return status_code;
}

inline rtems_status_code lpc176x_timer_start(
  const lpc176x_timer_number tnumber )
{
  rtems_status_code status_code = RTEMS_INVALID_NUMBER;

  if ( tnumber < LPC176X_TIMER_COUNT ) {
    timers[ tnumber ].device->TCR = LPC176X_TIMER_START;
    status_code = RTEMS_SUCCESSFUL;
  }

  /* else implies that the timer number is invalid. Also,
     an invalid number is returned. */

  return status_code;
}

inline rtems_status_code lpc176x_timer_is_started(
  const lpc176x_timer_number tnumber,
  bool                      *is_started
)
{
  rtems_status_code status_code = RTEMS_INVALID_NUMBER;

  if ( tnumber < LPC176X_TIMER_COUNT ) {
    *is_started = ( timers[ tnumber ].device->TCR & LPC176X_TIMER_START ) ==
                  LPC176X_TIMER_START;
    status_code = RTEMS_SUCCESSFUL;
  }

  /* else implies that the timer number is invalid. Also,
     an invalid number is returned. */

  return status_code;
}

inline rtems_status_code lpc176x_timer_set_resolution(
  const lpc176x_timer_number tnumber,
  const lpc176x_microseconds resolution
)
{
  rtems_status_code status_code = RTEMS_INVALID_NUMBER;

  if ( tnumber < LPC176X_TIMER_COUNT ) {
    timers[ tnumber ].device->PR = ( LPC176X_CCLK /
                                     LPC176X_TIMER_PRESCALER_DIVISOR ) *
                                   resolution;
    status_code = RTEMS_SUCCESSFUL;
  }

  /* else implies that the timer number is invalid. Also,
     an invalid number is returned. */

  return status_code;
}

rtems_status_code lpc176x_timer_match_config(
  const lpc176x_timer_number   tnumber,
  const lpc176x_match_port     match_port,
  const lpc176x_match_function function,
  const uint32_t               match_value
)
{
  rtems_status_code status_code = RTEMS_INVALID_NUMBER;

  if ( ( tnumber < LPC176X_TIMER_COUNT )
       && ( match_port < LPC176X_EMATCH_PORTS_COUNT )
       && ( function < LPC176X_TIMER_MATCH_FUNCTION_COUNT ) ) {
    timers[ tnumber ].device->MCR =
      LPC176X_SET_MCR( timers[ tnumber ].device->MCR,
        match_port, function );
    timers[ tnumber ].device->MR[ match_port ] = match_value;
    status_code = RTEMS_SUCCESSFUL;
  }

  /* else implies that the timer number, or a match port or a function
      is invalid. Also, an invalid number is returned. */

  return status_code;
}

inline rtems_status_code lpc176x_timer_capture_config(
  const lpc176x_timer_number     tnumber,
  const lpc176x_capture_port     capture_port,
  const lpc176x_capture_function function
)
{
  rtems_status_code status_code = RTEMS_INVALID_NUMBER;

  if ( ( tnumber < LPC176X_TIMER_COUNT )
       && ( capture_port < LPC176X_CAPTURE_PORTS_COUNT )
       && ( function < LPC176X_TIMER_CAPTURE_FUNCTION_COUNT ) ) {
    timers[ tnumber ].device->CCR =
      LPC176X_SET_CCR( timers[ tnumber ].device->CCR,
        capture_port, function );
    lpc176x_pin_select( timers[ tnumber ].pinselcap[ capture_port ],
      LPC176X_PIN_FUNCTION_11 );
  }

  /* else implies that the timer number or the capture port is invalid. Also,
     an invalid number is returned. */

  return status_code;
}

inline rtems_status_code lpc176x_timer_external_match_config(
  const lpc176x_timer_number       number,
  const lpc176x_match_port         match_port,
  const lpc176x_ext_match_function function
)
{
  rtems_status_code status_code = RTEMS_INVALID_NUMBER;

  if ( ( number < LPC176X_TIMER_COUNT )
       && ( match_port < LPC176X_EMATCH_PORTS_COUNT ) ) {
    timers[ number ].device->EMR =
      LPC176X_SET_EMR( timers[ number ].device->EMR,
        match_port, function );
    lpc176x_pin_select( timers[ number ].pinselemat[ match_port ],
      LPC176X_PIN_FUNCTION_11 );
    status_code = RTEMS_SUCCESSFUL;
  }

  /* else implies that the timer number or the match port is invalid. Also,
     an invalid number is returned. */

  return status_code;
}

inline uint32_t lpc176x_timer_get_capvalue(
  const lpc176x_timer_number number,
  const lpc176x_capture_port capture_port
)
{
  assert( ( number < LPC176X_TIMER_COUNT )
    && ( capture_port < LPC176X_CAPTURE_PORTS_COUNT ) );

  return timers[ number ].device->CR[ capture_port ];
}

inline uint32_t lpc176x_timer_get_timer_value(
  const lpc176x_timer_number tnumber )
{
  assert( tnumber < LPC176X_TIMER_COUNT );

  return timers[ tnumber ].device->TC;
}

inline rtems_status_code lpc176x_timer_set_timer_value(
  const lpc176x_timer_number tnumber,
  const uint32_t             timer_value
)
{
  rtems_status_code status_code = RTEMS_INVALID_NUMBER;

  if ( tnumber < LPC176X_TIMER_COUNT ) {
    timers[ tnumber ].device->TC = timer_value;
    status_code = RTEMS_SUCCESSFUL;
  }

  /* else implies that the timer number is invalid. Also,
     an invalid number is returned. */

  return status_code;
}

void lpc176x_timer_isr( void *arg )
{
  const lpc176x_timer_number tnumber = (lpc176x_timer_number) arg;

  if ( tnumber < LPC176X_TIMER_COUNT ) {
    lpc176x_isr_function i;

    for ( i = 0; i < LPC176X_ISR_FUNCTIONS_COUNT; ++i ) {
      if ( lpc176x_timer_interrupt_is_pending( tnumber, i ) ) {
        lpc176x_call_desired_isr( tnumber, i );
        lpc176x_timer_reset_interrupt( tnumber, i );
      }

      /* else implies that the current timer is not pending. Also,
         there is nothing to do. */
    }
  }

  /* else implies that the timer number is not valid. Also,
     there is nothing to do. */
}

rtems_status_code lpc176x_timer_init( const lpc176x_timer_number tnumber )
{
  rtems_status_code status_code = RTEMS_INVALID_NUMBER;

  if ( tnumber < LPC176X_TIMER_COUNT ) {
    status_code = lpc176x_module_enable( timers[ tnumber ].module,
      LPC176X_MODULE_PCLK_DEFAULT );
    RTEMS_CHECK_SC( status_code, "Enabling the timer module." );

    status_code = lpc176x_timer_reset( tnumber );
    status_code = lpc176x_timer_set_mode( tnumber,
      LPC176X_TIMER_MODE_TIMER );
    status_code = lpc176x_timer_set_resolution( tnumber,
      LPC176X_TIMER_DEFAULT_RESOLUTION );

    timers[ tnumber ].device->MCR = LPC176X_TIMER_CLEAR_FUNCTION;
    timers[ tnumber ].device->CCR = LPC176X_TIMER_CLEAR_FUNCTION;
    timers[ tnumber ].device->EMR = LPC176X_TIMER_CLEAR_FUNCTION;
  }

  /* else implies that the timer number is not valid. Also,
     an invalid number is returned. */

  return status_code;
}

rtems_status_code lpc176x_timer_init_with_interrupt(
  const lpc176x_timer_number            tnumber,
  const lpc176x_isr_funct_vector *const vector
)
{
  rtems_status_code status_code = RTEMS_INVALID_NUMBER;

  char isrname[ LPC176X_ISR_NAME_STRING_SIZE ];

  snprintf( isrname, LPC176X_ISR_NAME_STRING_SIZE, "TimerIsr%d", tnumber );

  if ( tnumber < LPC176X_TIMER_COUNT && vector != NULL ) {
    functions_vector[ tnumber ].funct_vector = vector;

    status_code = lpc176x_timer_init( tnumber );
    status_code = rtems_interrupt_handler_install(
      LPC176X_TIMER_VECTOR_NUMBER( tnumber ),
      isrname,
      RTEMS_INTERRUPT_UNIQUE,
      lpc176x_timer_isr,
      (void *) tnumber );
  }

  return status_code;
}