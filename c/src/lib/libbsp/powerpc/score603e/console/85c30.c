/*
 *  This file contains the console driver chip level routines for the
 *  z85c30 chip.
 *
 *  Currently only polled mode is supported.
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/libio.h>
#include <assert.h>

#include "85c30.h"
#include "consolebsp.h"

#define STATUS_REGISTER     0x00
#define DATA_REGISTER       0x08

#define Z8530_Status_Is_RX_character_available( _status ) \
  ( (_status) & 0x01 )

#define Z8530_Status_Is_TX_buffer_empty( _status ) \
  ( (_status) & 0x04 )

#define Z8530_Status_Is_break_abort( _status ) \
  ( (_status) & 0x80 )

typedef struct {
  unsigned char read_setup;
  unsigned char write_setup;
  unsigned char mask_value;
} char_size_info;

static const char_size_info Char_size_85c30[] = {
  { Z8530_READ_CHARACTER_BITS_8, Z8530_WRITE_CHARACTER_BITS_8, 0xFF },
  { Z8530_READ_CHARACTER_BITS_7, Z8530_WRITE_CHARACTER_BITS_7, 0x7F },
  { Z8530_READ_CHARACTER_BITS_6, Z8530_WRITE_CHARACTER_BITS_6, 0x3F },
  { Z8530_READ_CHARACTER_BITS_5, Z8530_WRITE_CHARACTER_BITS_5, 0x1F }
};

static const unsigned char Clock_speed_85c30[] = {
  Z8530_x1_CLOCK, Z8530_x16_CLOCK, Z8530_x32_CLOCK,  Z8530_x64_CLOCK };

static const unsigned char Stop_bit_85c30[] = {
  Z8530_STOP_BITS_1, Z8530_STOP_BITS_1_AND_A_HALF, Z8530_STOP_BITS_2 };

static const unsigned char Parity_85c30[] = {
  Z8530_PARITY_NONE, Z8530_PARITY_ODD, Z8530_PARITY_EVEN };

/* PAGE
 *
 * Read_85c30_register
 *
 * Read a Z85c30 register
 */
static unsigned char Read_85c30_register(
  volatile unsigned char *csr,                        /* IN  */
  unsigned char  register_number                      /* IN  */
)
{
  unsigned char Data;

  *csr = register_number;

  rtems_bsp_delay_in_bus_cycles( 40 );

  Data = *csr;

  rtems_bsp_delay_in_bus_cycles( 40 );

  return Data;
}

/*
 * Write_85c30_register
 *
 *  Write a Z85c30 register
 */
static void  Write_85c30_register(
  volatile unsigned char  *csr,                     /* IN  */
  unsigned char  register_number,                   /* IN  */
  unsigned char  data                               /* IN  */
)
{
  *csr = register_number;

  rtems_bsp_delay_in_bus_cycles( 40 );
  *csr = data;
  rtems_bsp_delay_in_bus_cycles( 40 );
}

/* PAGE
 *
 *  Reset_85c30_chip
 *
 *  Reset a 85c30 chip.  The pointers for the control registers for both
 *  ports on the chip are used as input.
 */
void Reset_85c30_chip(
  volatile unsigned char *ctrl_0,                     /* IN  */
  volatile unsigned char *ctrl_1                      /* IN  */
)
{
  Write_85c30_register( ctrl_0, 0x09, 0x80 );
  Write_85c30_register( ctrl_1, 0x09, 0x40 );
}

/* PAGE
 *
 * initialize_85c30_port
 *
 * initialize a z85c30 Port
 */
void initialize_85c30_port(
  const Port_85C30_info *Port
)
{
  uint16_t                value;
  volatile unsigned char *ctrl;
  Console_Protocol        *Setup;
  uint16_t                baud_constant;

  Setup = Port->Protocol;
  ctrl  = Port->ctrl;

  baud_constant = _Score603e_Z8530_Baud( Port->Chip->clock_frequency,
    Port->Chip->clock_x, Setup->baud_rate );

  /*
   * Using register 4
   * Set up the clock rate.
   */
  value = Clock_speed_85c30[ Port->Chip->clock_speed ] |
          Stop_bit_85c30[ Setup->stop_bits ] |
          Parity_85c30[ Setup->parity ];
  Write_85c30_register( ctrl, 0x04, value );

  /*
   *  Set Write Register 1 to disable all interrupts
   */
  Write_85c30_register( ctrl, 1, 0 );

#if CONSOLE_USE_INTERRUPTS
  /*
   *  Set Write Register 2 to contain the interrupt vector
   */
  Write_85c30_register( ctrl, 2, Port->Chip->vector );
#endif

  /*
   *  Set Write Register 3 to disable the Receiver
   */
  Write_85c30_register( ctrl, 0x03, 0x00 );

  /*
   *  Set Write Register 5 to disable the Transmitter
   */
  Write_85c30_register( ctrl, 5, 0x00 );

  /* WR 6 -- unneeded in asynchronous mode */

  /* WR 7 -- unneeded in asynchronous mode */

  /*
   *  Set Write Register 9 to disable all interrupt sources
   */
  Write_85c30_register( ctrl, 9, 0x00 );

  /*
   *  Set Write Register 10 for simple Asynchronous operation
   */
  Write_85c30_register( ctrl, 0x0a, 0x00 );

  /*
   * Setup the source of the receive and xmit
   * clock as BRG output and the transmit clock
   * as the output source for TRxC pin via register 11
   */
  Write_85c30_register( ctrl, 0x0b, 0x56 );

  value = baud_constant;

  /*
   * Setup the lower 8 bits time constants = 1E.
   * If the time constans = 1E, then the desire
   * baud rate will be equilvalent to 9600, via register 12.
   */
  Write_85c30_register( ctrl, 0x0c, value & 0xff );

  /*
   * using register 13
   * Setup the upper 8 bits time constants = 0
   */
  Write_85c30_register( ctrl, 0x0d, value>>8 );

  /*
   * Set the DTR/REQ pin goes low when transmit
   * buffer becomes empty and enable the baud
   * rate generator enable with clock from the
   * SCC's PCLK input via register 14.
   */
  Write_85c30_register( ctrl, 0x0e, 0x07 );

  /*
   *  Set Write Register 3 : Base Value is xx00_000x
   *     D6 - D7 : Receive Character Length               (configured)
   *     D5      : Auto Enable                            (forced value)
   *     D4      : Enter Hunt Phase                       (forced value)
   *     D3      : Receive CRC Enable                     (forced value)
   *     D2      : Address Search Mode (0 if not SDLC)    (forced value)
   *     D1      : Sync Character Load Inhibit            (forced value)
   *     D0      : Receiver Enable                        (configured)
   */
  value = 0x01;
  value = value | Char_size_85c30[ Setup->read_char_bits ].read_setup;

  Write_85c30_register( ctrl, 0x03, value );

  /*
   *  Set Write Register 5 : Base Value is 0xx0_x000
   *     D7      : Data Terminal Ready (DTR)              (forced value)
   *     D5 - D6 : Transmit Character Length              (configured)
   *     D4      : Send Break                             (forced value)
   *     D3      : Transmitter Enable                     (configured)
   *     D2      : CRC Select                             (forced value)
   *     D1      : Request to Send                        (forced value)
   *     D0      : Transmit CRC Enable                    (forced value)
   */
  value = 0x8a;
  value = value |  Char_size_85c30[ Setup->write_char_bits ].write_setup;
  Write_85c30_register( ctrl, 0x05, value );

  /*
   * Reset Tx UNDERRUN/EOM LATCH and ERROR
   * via register 0
   */
   Write_85c30_register( ctrl, 0x00, 0xf0 );

#if CONSOLE_USE_INTERRUPTS
  /*
   *  Set Write Register 1 to interrupt on Rx characters or special condition.
   */
  Write_85c30_register( ctrl, 1, 0x10 );
#endif

  /*
   *  Set Write Register 15 to disable extended functions.
   */

  Write_85c30_register( ctrl, 15, 0x00 );

  /*
   *  Set the Command Register to Reset Ext/STATUS.
   */
  Write_85c30_register( ctrl, 0x00, 0x10 );

#if CONSOLE_USE_INTERRUPTS

  /*
   *  Set Write Register 1 : Base Value is 0001_0110
   *    Enables Rx interrupt on all characters and special conditions.
   *    Enables parity as a special condition.
   *    Enables Tx interrupt.
   */
  Write_85c30_register( ctrl, 1, 0x16 );

  /*
   *  Set Write Register 9 to enable all interrupt sources
   *  Changed from 0 to a
   */
  Write_85c30_register( ctrl, 9, 0x0A );

  /* XXX */

  /*
   *  Issue reset highest Interrupt Under Service (IUS) command.
   */
  Write_85c30_register( Port->ctrl, STATUS_REGISTER, 0x38 );

#endif

}

/* PAGE
 *
 *  outbyte_polled_85c30
 *
 *  This routine transmits a character using polling.
 */

void outbyte_polled_85c30(
  volatile unsigned char  *csr,                     /* IN  */
  char ch                                           /* IN  */
)
{
  unsigned char       z8530_status;
  uint32_t            isrlevel;

  rtems_interrupt_disable( isrlevel );

  /*
   * Wait for the Transmit buffer to indicate that it is empty.
   */
  do {
    z8530_status = Read_85c30_register( csr, STATUS_REGISTER );
  } while ( !Z8530_Status_Is_TX_buffer_empty( z8530_status ) );

  /*
   * Write the character.
   */
  Write_85c30_register( csr, DATA_REGISTER, (unsigned char) ch );

  rtems_interrupt_enable( isrlevel );
}

/* PAGE
 *
 *  inbyte_nonblocking_85c30
 *
 *  This routine polls for a character.
 */

int inbyte_nonblocking_85c30(
  const Port_85C30_info      *Port
)
{
  volatile unsigned char  *csr;
  unsigned char   z8530_status;
  uint8_t         data;

  csr = Port->ctrl;

  /*
   * return -1 if a character is not available.
   */
  z8530_status = Read_85c30_register( csr, STATUS_REGISTER );
  if ( !Z8530_Status_Is_RX_character_available( z8530_status ) )
    return -1;

  /*
   * Return the character read.
   */
  data = Read_85c30_register( csr, DATA_REGISTER );
  data &= Char_size_85c30[ Port->Protocol->read_char_bits ].mask_value;

  return data;
}

/*
 *  Interrupt driven console IO
 */

#if CONSOLE_USE_INTERRUPTS

/*PAGE
 *
 *  Z8530_Async_Channel_ISR
 *
 */
/* RR0 */

rtems_isr ISR_85c30_Async(
   const Port_85C30_info   *Port
)
{
  uint16_t                   status;
  volatile Console_Protocol *Protocol;
  unsigned char              data;
  bool                       did_something = false;

  Protocol = Port->Protocol;

  status = Read_85c30_register( Port->ctrl, 0x00 );

  /*
   *  Was this a RX interrupt?  If so, then process it.
   */

  if ( Z8530_Status_Is_RX_character_available( status ) ) {
    data = Read_85c30_register( Port->ctrl, DATA_REGISTER );
    data &= Char_size_85c30[ Port->Protocol->read_char_bits ].mask_value;

    rtems_termios_enqueue_raw_characters( Port->Protocol->console_termios_data,
       &data, 1 );
    did_something = true;
  }

  /*
   *  Was this a TX empty interrupt?  If so, then process it.
   */

  if (Z8530_Status_Is_TX_buffer_empty( status ) ) {
    if ( !Ring_buffer_Is_empty( &Protocol->TX_Buffer ) ) {
      Ring_buffer_Remove_character( &Protocol->TX_Buffer, data );
      Write_85c30_register( Port->ctrl, DATA_REGISTER, data );

    } else {
      Protocol->Is_TX_active = false;
      Write_85c30_register( Port->ctrl, STATUS_REGISTER, 0x28 );
    }

    did_something = true;
  }

  /*
   *  Issue reset highest Interrupt Under Service (IUS) command.
   */

  /*
   if ( did_something )
   */
     Write_85c30_register( Port->ctrl, STATUS_REGISTER, 0x38 );
}

#endif
