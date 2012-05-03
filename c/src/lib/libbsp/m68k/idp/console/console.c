/*
 *  This file contains the Motorola IDP console IO package.
 *
 *  Written by Doug McBride, Colorado Space Grant College
 *  Based off of the board support packages of RTEMS
 *
 *  Updated to RTEMS 3.2.0 by Joel Sherrill.
 */

#define MIDP_INIT

#include <bsp.h>
#include <rtems/libio.h>

#include <rtems/ringbuf.h>

Ring_buffer_t  Console_Buffer[ 2 ];

rtems_isr C_Receive_ISR(rtems_vector_number vector);

/*  console_initialize
 *
 *  This routine initializes the console IO driver.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 */

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_status_code status;

  Ring_buffer_Initialize( &Console_Buffer[ 0 ] );
  Ring_buffer_Initialize( &Console_Buffer[ 1 ] );

  init_pit();

  status = rtems_io_register_name(
    "/dev/console",
    major,
    (rtems_device_minor_number) 0
  );

  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  status = rtems_io_register_name(
    "/dev/tty00",
    major,
    (rtems_device_minor_number) 0
  );

  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  status = rtems_io_register_name(
    "/dev/tty01",
    major,
    (rtems_device_minor_number) 1
  );

  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  return RTEMS_SUCCESSFUL;
}

/*  is_character_ready
 *
 *  This routine returns TRUE if a character is available.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 */

bool is_character_ready(
  char *ch,
  int   port
)
{
  if ( Ring_buffer_Is_empty( &Console_Buffer[ port ] ) )
    return false;

  Ring_buffer_Remove_character( &Console_Buffer[ port ], *ch );
  return true;
}

/*  quick_char_check
 *
 *  This routine returns TRUE if a character is available.
 *  It is different from above because it does not disturb the ring buffer
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 */

bool quick_char_check(
  int   port
)
{
  if ( Ring_buffer_Is_empty( &Console_Buffer[ port ] ) )
    return false;

  return true;
}

/*  inbyte
 *
 *  This routine reads a character from the UART through a buffer.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    character read from UART
 */

char inbyte(
  int port
)
{
  char tmp_char;

  /* If you come into this routine without checking is_character_ready() first
     and you want nonblocking code, then it's your own fault */

  while ( !is_character_ready( &tmp_char, port ) );

  return tmp_char;
}

/*  outbyte
 *
 *  This routine transmits a character out the M68681.  It supports
 *  XON/XOFF flow control.
 *
 *  Input parameters:
 *    ch  - character to be transmitted
 *
 *  Output parameters:  NONE
 */

void outbyte(
  char ch,
  int  port
)
{
  switch ( port ) {
    case 0:
      transmit_char( ch );
      break;
    case 1:
      transmit_char_portb( ch );
      break;
  }

}

/*
 *  Open entry point
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
 *  Close entry point
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
 * read bytes from the serial port. We only have stdin.
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

  if ( minor > 1 )
    return RTEMS_INVALID_NUMBER;

  for (count = 0; count < maximum; count++) {
    buffer[ count ] = inbyte( minor );
    if (buffer[ count ] == '\n' || buffer[ count ] == '\r') {
      buffer[ count++ ]  = '\n';
      break;
    }
  }

  rw_args->bytes_moved = count;
  return (count >= 0) ? RTEMS_SUCCESSFUL : RTEMS_UNSATISFIED;
}

/*
 * write bytes to the serial port. Stdout and stderr are the same.
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

  if ( minor > 1 )
    return RTEMS_INVALID_NUMBER;

  for (count = 0; count < maximum; count++) {
    if ( buffer[ count ] == '\n') {
      outbyte('\r', minor );
    }
    outbyte( buffer[ count ], minor  );
  }

  rw_args->bytes_moved = maximum;
  return 0;
}

/*
 *  IO Control entry point
 */

rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return RTEMS_SUCCESSFUL;
}
