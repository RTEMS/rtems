/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief CAN controller for the mbed lpc1768 board.
 */

/*
 * Copyright (c) 2014 Taller Technologies.
 *
 * @author  Diaz Marcos (marcos.diaz@tallertechnologies.com)
 * @author  Daniel Chicco  (daniel.chicco@tallertechnologies.com)
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/status-checks.h>
#include <bsp/irq.h>
#include <bsp/can.h>
#include <bsp/can-defs.h>
#include <bsp/mbed-pinmap.h>
#include <string.h>

/**
 * @brief The standard isr to be installed for all the can devices.
 *
 * @param arg unused.
 */
static void can_isr( void *arg );

/**
 * @brief Vector of isr for the can_driver .
 */
lpc176x_can_isr_vector isr_vector;

/**
 * @brief Represents all the can devices, and useful things for initialization.
 */
static const can_driver_entry can_driver_table[ CAN_DEVICES_NUMBER ] =
{
  {
    .device = (can_device *) CAN1_BASE_ADDR,
    .module = LPC176X_MODULE_CAN_0,
    .pconp_pin = LPC176X_SCB_PCONP_CAN_1,
    .pins = { DIP9, DIP10 },
    .pinfunction = LPC176X_PIN_FUNCTION_01
  },
  {
    .device = (can_device *) CAN2_BASE_ADDR,
    .module = LPC176X_MODULE_CAN_1,
    .pconp_pin = LPC176X_SCB_PCONP_CAN_2,
    .pins = { DIP30, DIP29 },
    .pinfunction = LPC176X_PIN_FUNCTION_10
  }
};

/**
 * @brief The CAN acceptance filter.
 */
can_acceptance_filter *const acceptance_filter_device =
  (can_acceptance_filter *) CAN_ACCEPT_BASE_ADDR;

/**
 * @brief Sets RX and TX pins for the passed can device number.
 *
 * @param cannumber CAN controller to be used.
 */
static inline void setpins( const lpc176x_can_number cannumber )
{
  const can_driver_entry *const can_driver = &can_driver_table[ cannumber ];

  lpc176x_pin_select( can_driver->pins[ CAN_TX_PIN ],
    can_driver->pinfunction );
  lpc176x_pin_select( can_driver->pins[ CAN_RX_PIN ],
    can_driver->pinfunction );
}

rtems_status_code can_close( const lpc176x_can_number minor )
{
  rtems_status_code sc = RTEMS_INVALID_NUMBER;

  if ( CAN_DRIVER_IS_MINOR_VALID( minor ) ) {
    sc = RTEMS_SUCCESSFUL;
    const can_driver_entry *const can_driver = &can_driver_table[ minor ];
    lpc176x_module_disable( can_driver->module );
  }

  /*else wrong parameters. return RTEMS_INVALID_NUMBER*/

  return sc;
}

/**
 * @brief Enables CAN device.
 *
 * @param obj The device to be enabled.
 */
static inline void can_enable( const can_driver_entry *const obj )
{
  if ( obj->device->MOD & CAN_MOD_RM ) {
    obj->device->MOD &= ~( CAN_MOD_RM );
  }
}

/**
 * @brief Disables CAN device to set parameters, and returns the previous value
 * of the MOD register.
 *
 * @param obj The device to disable.
 * @return The previous status of MOD register.
 */
static inline uint32_t can_disable( const can_driver_entry *const obj )
{
  const uint32_t sm = obj->device->MOD;

  obj->device->MOD |= CAN_MOD_RM;

  return sm;
}

/**
 * @brief Resets the error count.
 *
 * @param obj which device reset.
 */
static inline void can_reset( const can_driver_entry *const obj )
{
  can_disable( obj );
  obj->device->GSR = 0;   /* Reset error counter when CANxMOD is in reset*/
}

/**
 * @brief This table has the sampling points as close to 75% as possible.
 * The first value is TSEG1, the second is TSEG2.
 */
static const unsigned int timing_pts[ MAX_TSEG1_TSEG2_BITS +
                                      1 ][ CAN_NUMBER_OF_TSEG ] = {
  { 0x0, 0x0 },      /* 2,  50%*/
  { 0x1, 0x0 },      /* 3,  67%*/
  { 0x2, 0x0 },      /* 4,  75%*/
  { 0x3, 0x0 },      /* 5,  80%*/
  { 0x3, 0x1 },      /* 6,  67%*/
  { 0x4, 0x1 },      /* 7,  71%*/
  { 0x5, 0x1 },      /* 8,  75%*/
  { 0x6, 0x1 },      /* 9,  78%*/
  { 0x6, 0x2 },      /* 10, 70%*/
  { 0x7, 0x2 },      /* 11, 73%*/
  { 0x8, 0x2 },      /* 12, 75%*/
  { 0x9, 0x2 },      /* 13, 77%*/
  { 0x9, 0x3 },      /* 14, 71%*/
  { 0xA, 0x3 },      /* 15, 73%*/
  { 0xB, 0x3 },      /* 16, 75%*/
  { 0xC, 0x3 },      /* 17, 76%*/
  { 0xD, 0x3 },      /* 18, 78%*/
  { 0xD, 0x4 },      /* 19, 74%*/
  { 0xE, 0x4 },      /* 20, 75%*/
  { 0xF, 0x4 },      /* 21, 76%*/
  { 0xF, 0x5 },      /* 22, 73%*/
  { 0xF, 0x6 },      /* 23, 70%*/
  { 0xF, 0x7 },      /* 24, 67%*/
};

/**
 * @brief Checks if divisor is a divisor of value.
 *
 * @param value The number to be divided.
 * @param divisor The divisor to check.
 *
 * @return true if "number" is divided by "divisor"; false otherwise.
 */
static inline bool is_divisor(
  const uint32_t value,
  const uint16_t divisor
)
{
  return ( ( value % divisor ) == 0 );
}

/**
 * @brief Gets the size of the two tseg values added according to the given
 * bitwidth and brp (The CAN prescaler).
 *
 * @param bitwidth The total bitwidth of a CAN bit (in pclk clocks).
 * @param brp The CAN clock prescaler.
 *
 * @return The value of tseg1 + tseg2  of the CAN bit. It is useful
 *  to serve for index for timing_pts array).
 */
static inline uint32_t get_tseg_bit_size(
  const uint32_t bitwidth,
  const uint16_t brp
)
{
  return ( ( bitwidth / ( brp + CAN_BRP_EXTRA_BIT ) ) - CAN_TSEG_EXTRA_BITS );
}

/**
 * @brief Gets the brp and tsegbitsize in order to achieve the desired bitwidth.
 * @details The following must be fullfilled:
 *(brp + CAN_BRP_EXTRA_BIT) * (tsegbitsize + CAN_TSEG_EXTRA_BITS) == bitwidth
 *
 * @param bitwidth The bitwidth that we need to achieve.
 * @param brp Here it returns the calculated brp value.
 * @param tsegbitsize Here it returns the calculated tseg bit size value.
 * @return true if brp and tsegbitsize have been calculated.
 */
static inline bool get_brp_and_bitsize(
  const uint32_t  bitwidth,
  uint16_t *const brp,
  uint32_t *const tsegbitsize
)
{
  bool hit = false;

  while ( ( !hit ) && ( *brp < bitwidth / MIN_NUMBER_OF_CAN_BITS ) ) {
    if ( ( is_divisor( bitwidth, *brp + CAN_BRP_EXTRA_BIT ) )
         && ( get_tseg_bit_size( bitwidth, *brp ) < MAX_TSEG1_TSEG2_BITS ) ) {
      hit = true;
      *tsegbitsize = get_tseg_bit_size( bitwidth, *brp );
    } else { /*Correct values not found, keep looking*/
      ( *brp )++;
    }
  }

  return hit;
}

/**
 * @brief Constructs the btr register with the passed arguments.
 *
 * @param tsegbitsize The size tseg bits to set.
 * @param psjw The sjw to set.
 * @param brp The prescaler value to set.
 * @return The constructed btr register.
 */
static inline uint32_t get_btr(
  const uint32_t      tsegbitsize,
  const unsigned char psjw,
  const uint32_t      brp
)
{
  const uint32_t tseg2_value_masked =
    ( ( timing_pts[ tsegbitsize ][ CAN_TSEG2 ] << CAN_BTR_TSEG2_SHIFT ) &
      CAN_BTR_TSEG2_MASK );
  const uint32_t tseg1_value_masked =
    ( ( timing_pts[ tsegbitsize ][ CAN_TSEG1 ] <<
        CAN_BTR_TSEG1_SHIFT ) & CAN_BTR_TSEG1_MASK );
  const uint32_t psjw_value_masked =
    ( ( psjw << CAN_BTR_SJW_SHIFT ) & CAN_BTR_SJW_MASK );
  const uint32_t brp_value_masked =
    ( ( brp << CAN_BTR_BRP_SHIFT ) & CAN_BTR_BRP_MASK );

  return tseg1_value_masked | tseg2_value_masked |
         psjw_value_masked | brp_value_masked;
}

/**
 * @brief Calculates and returns a bit timing register (btr) for the desired
 * canclk frequency using the passed psjw, system clock and peripheral clock.
 *
 * @param systemclk The clock of the system (in Hz).
 * @param pclkdiv The peripheral clock divisor for the can device.
 * @param canclk The desired frequency for CAN (in Hz).
 * @param psjw The desired psjw.
 * @return The btr register value if found, WRONG_BTR_VALUE otherwise.
 */
static inline unsigned int can_speed(
  const unsigned int  systemclk,
  const unsigned int  pclkdiv,
  const unsigned int  canclk,
  const unsigned char psjw
)
{
  uint32_t       btr = WRONG_BTR_VALUE;
  const uint32_t bitwidth = systemclk / ( pclkdiv * canclk );

  /* This is for the brp (prescaler) to start searching a reachable multiple.*/
  uint16_t brp = bitwidth / MAX_NUMBER_OF_CAN_BITS;
  uint32_t tsegbitsize;

  if ( get_brp_and_bitsize( bitwidth, &brp, &tsegbitsize ) ) {
    btr = get_btr( tsegbitsize, psjw, brp );
  }

  return btr;
}

/**
 * @brief Configures the desired CAN device with the desired frequency.
 *
 * @param obj The can device to configure.
 * @param f The desired frequency.
 *
 * @return RTEMS_SUCCESSFUL if could be set, RTEMS_INVALID_NUMBER otherwise.
 */
static rtems_status_code can_frequency(
  const can_driver_entry *const obj,
  const can_freq            freq
)
{
  rtems_status_code sc = RTEMS_INVALID_NUMBER;
  const uint32_t    btr = can_speed( LPC176X_CCLK, LPC176X_PCLKDIV, freq, 1 );

  if ( btr != WRONG_BTR_VALUE ) {
    sc = RTEMS_SUCCESSFUL;
    uint32_t modmask = can_disable( obj );
    obj->device->BTR = btr;
    obj->device->MOD = modmask;
  } /*else couldnt found a good timing for the desired frequency,
      return RTEMS_INVALID_NUMBER.*/

  return sc;
}

/**
 * @brief Installs the interrupt handler in rtems.
 */
static inline rtems_status_code can_initialize( void )
{
  return rtems_interrupt_handler_install(
    LPC176X_IRQ_CAN,
    "can_interrupt",
    RTEMS_INTERRUPT_UNIQUE,
    can_isr,
    NULL
  );
}

rtems_status_code can_open( const lpc176x_can_number minor, can_freq freq )
{
  const can_driver_entry *const can_driver = &can_driver_table[ minor ];
  rtems_status_code             sc = RTEMS_INVALID_NUMBER;

  if ( CAN_DRIVER_IS_MINOR_VALID( minor ) ) {
    /*Enable CAN and acceptance filter modules.*/
    sc =
      lpc176x_module_enable( can_driver->module, LPC176X_MODULE_PCLK_DEFAULT );
    RTEMS_CHECK_SC( sc, "enable can module" );
    sc = lpc176x_module_enable( LPC176X_MODULE_ACCF,
      LPC176X_MODULE_PCLK_DEFAULT );
    RTEMS_CHECK_SC( sc, "enable acceptance filter" );
    /*Set pin functions.*/
    setpins( minor );

    can_reset( can_driver );
    can_driver->device->IER = CAN_DEFAULT_INTERRUPT_CONFIGURATION;
    sc = can_frequency( can_driver, freq );
    RTEMS_CHECK_SC( sc, "Configure CAN frequency" );
    can_initialize();

    acceptance_filter_device->AFMR = CAN_ACCF_AFMR_ACCBP;     /*Bypass Filter.*/
  }

  return sc;
}

/**
 * @brief Calls the installed isrs, according to the active interrupts.
 *
 * @param vector The read vector of active interrupts.
 * @param number The CAN device to look for interruptions.
 */
static inline void call_isrs(
  const uint32_t           vector,
  const lpc176x_can_number number
)
{
  can_irq_type i;

  for ( i = IRQ_RX; i < CAN_IRQ_NUMBER; ++i ) {
    if ( ( isr_vector[ i ] != NULL ) && ( vector & ( 1 << i ) ) )
      isr_vector[ i ]( number );

    /* else this interrupt has not been raised or it hasn't got a handler,
       so do nothing.*/
  }
}

/**
 * @brief Checks if the passed CAN device is enabled and if it is checks for
 * active interrupts and calls its installed isr.
 *
 * @param number The CAN device to check for interrupts rised.
 */
static inline void search_and_call_int( const lpc176x_can_number number )
{
  const can_driver_entry *const driver = &can_driver_table[ number ];

  if ( LPC176X_SCB.pconp & driver->pconp_pin ) {
    /*We must read the whole register at once because it resets when read.*/
    const uint32_t int_vector = driver->device->ICR & CAN_INTERRUPT_TYPE_MASK;
    call_isrs( int_vector, number );
  }

  /*else the device is shut down so we must do nothing.*/
}

/**
 * @brief The standard isr to be installed for all the CAN devices.
 *
 * @param arg unused.
 */
static void can_isr( void *arg )
{
  lpc176x_can_number i;

  for ( i = CAN_0; i < CAN_DEVICES_NUMBER; ++i ) {
    search_and_call_int( i );
  }
}

rtems_status_code can_read(
  const lpc176x_can_number minor,
  can_message             *message
)
{
  rtems_status_code             sc = RTEMS_IO_ERROR;
  const can_driver_entry *const can_driver = &can_driver_table[ minor ];
  can_device *const             dev = can_driver->device;
  registers_can_message *const  msg = &( message->registers );

  can_enable( can_driver );

  if ( dev->GSR & CAN_GSR_RBS_MASK ) {
    sc = RTEMS_SUCCESSFUL;
    *msg = dev->receive;
    dev->CMR = CAN_CMR_RRB_MASK;      /* release receive buffer. */
  } /* else Message not received.*/

  return sc;
}

/**
 * @brief Array of masks and control bits for the transmit buffers.
 * It's used for each transmit buffer in order to see if it's available and to
 * send data to them.
 */
static const can_transmit_info transmit_info[ CAN_NUMBER_OF_TRANSMIT_BUFFERS ]
  =
  {
  {
    .can_status_mask = 0x00000004U,
    .not_cc_cmr_value = 0x21U
  },
  {
    .can_status_mask = 0x00000400U,
    .not_cc_cmr_value = 0x41U
  },
  {
    .can_status_mask = 0x00040000U,
    .not_cc_cmr_value = 0x81U
  }
  };

rtems_status_code can_write(
  const lpc176x_can_number minor,
  const can_message *const message
)
{
  const can_driver_entry *const can_driver = &can_driver_table[ minor ];
  can_device *const             obj = can_driver->device;
  const uint32_t                CANStatus = obj->SR;

  const registers_can_message *const msg = &( message->registers );
  rtems_status_code                  sc = RTEMS_IO_ERROR;
  can_transmit_number                transmit_buffer;

  can_enable( can_driver );

  for ( transmit_buffer = CAN_TRANSMIT1;
        ( sc != RTEMS_SUCCESSFUL ) && ( transmit_buffer <
                                        CAN_NUMBER_OF_TRANSMIT_BUFFERS );
        ++transmit_buffer ) {
    if ( CANStatus & transmit_info[ transmit_buffer ].can_status_mask ) {
      sc = RTEMS_SUCCESSFUL;
      obj->transmit[ transmit_buffer ] = *msg;
      obj->CMR = transmit_info[ transmit_buffer ].not_cc_cmr_value;
    }    /*else can buffer busy, try with the next.*/
  }

  return sc;
}

/**
 * @brief Enables the interrupt type passed to the desired CAN device.
 *
 * @param number The CAN device to enable the interrupts.
 * @param type The type of interrupt to enable.
 */
static inline void can_enable_interrupt(
  const lpc176x_can_number number,
  const can_irq_type       type
)
{
  const can_driver_entry *const driver = &can_driver_table[ number ];
  const uint32_t                ier = 1 << type;

  can_disable( driver );
  driver->device->IER |= ier;
  can_enable( driver );
}

rtems_status_code can_register_isr(
  const lpc176x_can_number number,
  const can_irq_type       type,
  const lpc176x_can_isr    isr
)
{
  rtems_status_code sc = RTEMS_INVALID_NUMBER;

  if ( ( 0 <= type ) && ( type < CAN_IRQ_NUMBER ) ) {
    sc = RTEMS_SUCCESSFUL;
    isr_vector[ type ] = isr;
    can_enable_interrupt( number, type );
  }

  return sc;
}

rtems_status_code create_can_message(
  can_message *const msg,
  const int          _id,
  const char *const  _data,
  const char         _len
)
{
  rtems_status_code sc = RTEMS_INVALID_NUMBER;

  if ( ( _len <= CAN_MAXIMUM_DATA_SIZE ) && ( _id <= CAN10_MAXIMUM_ID ) ) {
    sc = RTEMS_SUCCESSFUL;
    msg->low_level.dlc = _len;
    msg->low_level.type = CANStandard;
    msg->low_level.rtr = CANData;
    msg->low_level.id = _id;
    memcpy( msg->low_level.data, _data, _len );
  }

  return sc;
}

