/*
 *  console.c
 *
 *  This file contains the Sparc Instruction Simulator Console driver.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space 
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995. 
 *  European Space Agency.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>

/*
 *  Define RDB_BREAK_IN if you need to be able to break in to the
 *  program with a ctrl-c during remote target debugging. If so,
 *  UART B will not be accessible from rtems during remote debugging
 *  if interrupt driven console is used. Does not affect UART A, polled 
 *  mode or when the program runs without remote debugging.
 */

#define RDB_BREAK_IN

/*
 *  This is a kludge so the tests run without stopping to ask for input.
 *  It makes it possible to run the RTEMS tests without human intervention.
 */

/*
#define CONSOLE_FAKE_INPUT
*/

/*
 *  Should we use a polled or interrupt drived console?
 *  
 *  NOTE: Define only one of these by default.
 *
 *  WARNING:  As of sis 1.6, it did not appear that the UART interrupts
 *            worked in a desirable fashion.  Immediately upon writing
 *            a character into the TX buffer, an interrupt was generated.
 *            This did not allow enough time for the program to put more
 *            characters in the buffer.  So every character resulted in
 *            "priming" the transmitter.   This effectively results in 
 *            in a polled console with a useless interrupt per character
 *            on output.  It is reasonable to assume that input does not
 *            share this problem although it was not investigated.
 */

#ifdef CONSOLE_USE_POLLED
#define OUTBYTE console_outbyte_polled
#define INBYTE  console_inbyte_polled
#else
#define OUTBYTE console_outbyte_interrupts
#define INBYTE  console_inbyte_interrupts
#endif

void console_initialize_interrupts( void );

/*  console_initialize
 *
 *  This routine initializes the console IO driver.
 *
 *  Input parameters:
 *    major - console device major number
 *    minor - console device minor number
 *    arg   - pointer to optional device driver arguments
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    rtems_device_driver status code
 */
 
rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_status_code status;
 
  status = rtems_io_register_name(
    "/dev/console",
    major,
    (rtems_device_minor_number) 0
  );
 
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

#ifdef CONSOLE_USE_INTERRUPTS
  console_initialize_interrupts();
#endif
 
  return RTEMS_SUCCESSFUL;
}

/*  console_inbyte_polled
 *
 *  This routine reads a character from the UART.
 *
 *  Input parameters:
 *    port - port to read character from
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    character read from UART
 */

char console_inbyte_polled( int port )
{
  int UStat;

  if ( port == 0 ) {
    while (((UStat = ERC32_MEC.UART_Status) & ERC32_MEC_UART_STATUS_DRA) == 0 )
	if (UStat & ERC32_MEC_UART_STATUS_ERRA) {
	    ERC32_MEC.UART_Status = ERC32_MEC_UART_STATUS_CLRA;
	    ERC32_MEC.Control = ERC32_MEC.Control;
	}
    return (int) ERC32_MEC.UART_Channel_A;
  } 

  while (((UStat = ERC32_MEC.UART_Status) & ERC32_MEC_UART_STATUS_DRB) == 0 )
	if (UStat & ERC32_MEC_UART_STATUS_ERRB) {
	    ERC32_MEC.UART_Status = ERC32_MEC_UART_STATUS_CLRB;
	    ERC32_MEC.Control = ERC32_MEC.Control;
	}
  return (int) ERC32_MEC.UART_Channel_B;
}


/*  console_outbyte_polled
 *
 *  This routine transmits a character out.
 *
 *  Input parameters:
 *    port - port to transmit character to
 *    ch  - character to be transmitted
 *
 *  Output parameters:  NONE
 *
 *  Return values: NONE
 */

void console_outbyte_polled(
  int  port,
  char ch
)
{
  if ( port == 0 ) {
    while ( (ERC32_MEC.UART_Status & ERC32_MEC_UART_STATUS_THEA) == 0 );
    ERC32_MEC.UART_Channel_A = (int) ch;
    return;
  }

  while ( (ERC32_MEC.UART_Status & ERC32_MEC_UART_STATUS_THEB) == 0 );
  ERC32_MEC.UART_Channel_B = (int) ch;
}

/*
 *  Interrupt driven console IO
 */

#ifdef CONSOLE_USE_INTERRUPTS

/*
 *  Buffers between task and ISRs
 */

#include <ringbuf.h>
 
Ring_buffer_t  TX_Buffer[ 2 ];
Ring_buffer_t  RX_Buffer[ 2 ];
boolean        Is_TX_active[ 2 ];
 
/*
 *  console_isr_a
 *
 *  This routine is the console interrupt handler for Channel A.
 *
 *  Input parameters:
 *    vector - vector number 
 *
 *  Output parameters: NONE
 *
 *  Return values:     NONE
 */

rtems_isr console_isr_a(
  rtems_vector_number vector
)
{ 
  char ch;
  int UStat;
 
  if ( (UStat = ERC32_MEC.UART_Status) & ERC32_MEC_UART_STATUS_DRA ) {
    if (UStat & ERC32_MEC_UART_STATUS_ERRA) {
      ERC32_MEC.UART_Status = ERC32_MEC_UART_STATUS_CLRA;
      ERC32_MEC.Control = ERC32_MEC.Control;
    }
    ch = ERC32_MEC.UART_Channel_A;
    if ( !Ring_buffer_Is_full( &RX_Buffer[ 0 ] ) )
      Ring_buffer_Add_character( &RX_Buffer[ 0 ], ch );
    /* else toss it */
  }
 
  if ( ERC32_MEC.UART_Status & ERC32_MEC_UART_STATUS_THEA ) {
    if ( !Ring_buffer_Is_empty( &TX_Buffer[ 0 ] ) ) {
      Ring_buffer_Remove_character( &TX_Buffer[ 0 ], ch );
      ERC32_MEC.UART_Channel_A = (unsigned32) ch;
    } else
     Is_TX_active[ 0 ] = FALSE;
  }
 
  ERC32_Clear_interrupt( ERC32_INTERRUPT_UART_A_RX_TX );
}

/*
 *  console_isr_b
 *
 *  This routine is the console interrupt handler for Channel B.
 *
 *  Input parameters:
 *    vector - vector number 
 *
 *  Output parameters: NONE
 *
 *  Return values:     NONE
 */
 
rtems_isr console_isr_b(
  rtems_vector_number vector
)
{
  char ch;
  int UStat;

  if ( (UStat = ERC32_MEC.UART_Status) & ERC32_MEC_UART_STATUS_DRB ) {
    if (UStat & ERC32_MEC_UART_STATUS_ERRB) {
      ERC32_MEC.UART_Status = ERC32_MEC_UART_STATUS_CLRB;
      ERC32_MEC.Control = ERC32_MEC.Control;
    }
    ch = ERC32_MEC.UART_Channel_B;
    if ( !Ring_buffer_Is_full( &RX_Buffer[ 1 ] ) )
      Ring_buffer_Add_character( &RX_Buffer[ 1 ], ch );
    /* else toss it */
  }

  if ( ERC32_MEC.UART_Status & ERC32_MEC_UART_STATUS_THEB ) {
    if ( !Ring_buffer_Is_empty( &TX_Buffer[ 1 ] ) ) {
      Ring_buffer_Remove_character( &TX_Buffer[ 1 ], ch );
      ERC32_MEC.UART_Channel_B = (unsigned32) ch;
    } else
     Is_TX_active[ 1 ] = FALSE;
  }

  ERC32_Clear_interrupt( ERC32_INTERRUPT_UART_B_RX_TX );
}

/*
 *  console_exit
 *
 *  This routine allows the console to exit by masking its associated interrupt
 *  vectors.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters: NONE
 *
 *  Return values:     NONE
 */

void console_exit()
{
  rtems_unsigned32 port;
  rtems_unsigned32 ch;

  /*
   *  Although the interrupts for the UART are unmasked, the PIL is set to
   *  disable all external interrupts.  So we might as well do this first.
   */

  ERC32_Mask_interrupt( ERC32_INTERRUPT_UART_A_RX_TX );
  ERC32_Mask_interrupt( ERC32_INTERRUPT_UART_B_RX_TX );

  for ( port=0 ; port <= 1 ; port++ ) {
    while ( !Ring_buffer_Is_empty( &TX_Buffer[ port ] ) ) {
      Ring_buffer_Remove_character( &TX_Buffer[ port ], ch );
      console_outbyte_polled( port, ch );
    }
  }

  /*
   *  Now wait for all the data to actually get out ... the send register
   *  should be empty.
   */
 
  while ( (ERC32_MEC.UART_Status & ERC32_MEC_UART_STATUS_THEA) != 
          ERC32_MEC_UART_STATUS_THEA );

  while ( (ERC32_MEC.UART_Status & ERC32_MEC_UART_STATUS_THEB) != 
          ERC32_MEC_UART_STATUS_THEB );

}

#define CONSOLE_UART_A_TRAP  ERC32_TRAP_TYPE( ERC32_INTERRUPT_UART_A_RX_TX )
#define CONSOLE_UART_B_TRAP  ERC32_TRAP_TYPE( ERC32_INTERRUPT_UART_B_RX_TX )

/*
 *  console_initialize_interrupts
 *
 *  This routine initializes the console's receive and transmit
 *  ring buffers and loads the appropriate vectors to handle the interrupts.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters: NONE
 *
 *  Return values:     NONE
 */

#ifdef RDB_BREAK_IN
  extern unsigned32 trap_table[];
#endif

void console_initialize_interrupts()
{
  Ring_buffer_Initialize( &RX_Buffer[ 0 ] );
  Ring_buffer_Initialize( &RX_Buffer[ 1 ] );

  Ring_buffer_Initialize( &TX_Buffer[ 0 ] );
  Ring_buffer_Initialize( &TX_Buffer[ 1 ] );

  Is_TX_active[ 0 ] = FALSE;
  Is_TX_active[ 1 ] = FALSE;

  atexit( console_exit );

  set_vector( console_isr_a, CONSOLE_UART_A_TRAP, 1 );
#ifdef RDB_BREAK_IN
  if (trap_table[0x150/4] == 0x91d02000)
#endif
  set_vector( console_isr_b, CONSOLE_UART_B_TRAP, 1 );
}

/*
 *  console_inbyte_interrupts
 *
 *  This routine reads a character from the UART.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    character read from UART
 */
 
char console_inbyte_interrupts( int port )
{
  char ch;

  while ( Ring_buffer_Is_empty( &RX_Buffer[ port ] ) );
 
  Ring_buffer_Remove_character( &RX_Buffer[ port ], ch );
  return ch;
}
 
/*
 *  console_outbyte_interrupts
 *
 *  This routine transmits a character out.
 *
 *  Input parameters:
 *    port - port to transmit character to
 *    ch  - character to be transmitted
 *
 *  Output parameters:  NONE
 *
 *  Return values:      NONE
 */
 
void console_outbyte_interrupts(
  int  port,
  char ch
)
{
  /*
   *  If this is the first character then we need to prime the pump
   */

  if ( Is_TX_active[ port ] == FALSE ) {
    Is_TX_active[ port ] = TRUE;
    console_outbyte_polled( port, ch );
    return;
  }

  while ( Ring_buffer_Is_full( &TX_Buffer[ port ] ) );
 
  Ring_buffer_Add_character( &TX_Buffer[ port ], ch );
}

#endif /* CONSOLE_USE_INTERRUPTS */

/*
 *  DEBUG_puts
 *
 *  This should be safe in the event of an error.  It attempts to insure
 *  that no TX empty interrupts occur while it is doing polled IO.  Then
 *  it restores the state of that external interrupt.
 *
 *  Input parameters:
 *    string  - pointer to debug output string
 *
 *  Output parameters:  NONE
 *
 *  Return values:      NONE
 */

void DEBUG_puts(
  char *string
)
{
  char *s;
  unsigned32 old_level;

  ERC32_Disable_interrupt( ERC32_INTERRUPT_UART_A_RX_TX, old_level );
    for ( s = string ; *s ; s++ ) 
      console_outbyte_polled( 0, *s );

    console_outbyte_polled( 0, '\r' );
    console_outbyte_polled( 0, '\n' );
  ERC32_Restore_interrupt( ERC32_INTERRUPT_UART_A_RX_TX, old_level );
}


/*
 *  console_open
 *
 *  This routine is the console device driver open entry point. 
 *
 *  Input parameters:
 *    major - console device major number
 *    minor - console device minor number
 *    arg   - pointer to optional device driver arguments
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    rtems_device_driver status code
 */
 
rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return RTEMS_SUCCESSFUL;
}
 
/*
 *  console_close
 *
 *  This routine is the console device driver close entry point.
 *
 *  Input parameters:
 *    major - console device major number
 *    minor - console device minor number
 *    arg   - pointer to optional device driver arguments
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    rtems_device_driver status code
 */
 
rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return RTEMS_SUCCESSFUL;
}
 
/*
 *  console_read
 *
 *  This routine is the console device driver read entry point.
 *
 *  Input parameters:
 *    major - console device major number
 *    minor - console device minor number
 *    arg   - pointer to optional device driver arguments
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    rtems_device_driver status code
 *
 *  NOTE:  Read bytes from the serial port. We only have stdin.
 */
 
rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_libio_rw_args_t *rw_args;
  char *buffer;
  int maximum;
  int count = 0;
 
  rw_args = (rtems_libio_rw_args_t *) arg;
 
  buffer = rw_args->buffer;
  maximum = rw_args->count;
 
#ifdef CONSOLE_FAKE_INPUT
    count = 0;
    buffer[ count++ ]  = '\n';
    buffer[ count ]  = 0;
#else
  for (count = 0; count < maximum; count++) {
    buffer[ count ] = INBYTE( minor );
    if (buffer[ count ] == '\n' || buffer[ count ] == '\r') {
      buffer[ count++ ]  = '\n';
      buffer[ count ]  = 0;
      break;
    }
  }
#endif
 
  rw_args->bytes_moved = count;
  return (count >= 0) ? RTEMS_SUCCESSFUL : RTEMS_UNSATISFIED;
}
 
/*
 *  console_write
 *
 *  This routine is the console device driver write entry point.
 *
 *  Input parameters:
 *    major - console device major number
 *    minor - console device minor number
 *    arg   - pointer to optional device driver arguments
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    rtems_device_driver status code
 *
 *  NOTE:  Write bytes to the serial port. Stdout and stderr are the same.
 */
 
rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  int count;
  int maximum;
  rtems_libio_rw_args_t *rw_args;
  char *buffer;
 
  rw_args = (rtems_libio_rw_args_t *) arg;
 
  buffer = rw_args->buffer;
  maximum = rw_args->count;
 
  for (count = 0; count < maximum; count++) {
    OUTBYTE( minor, buffer[ count ] );
    if ( buffer[ count ] == '\n') {
      OUTBYTE( minor, '\r');
    }
  }
 
  rw_args->bytes_moved = maximum;
  return RTEMS_SUCCESSFUL;
}
 
/*
 *  console_control
 *
 *  This routine is the console device driver control entry point.
 *
 *  Input parameters:
 *    major - console device major number
 *    minor - console device minor number
 *    arg   - pointer to optional device driver arguments
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    rtems_device_driver status code
 */
 
rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return RTEMS_SUCCESSFUL;
}

