/*
 *  This file contains the MVME147 console IO package.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  MVME147 port for TNI - Telecom Bretagne
 *  by Dominique LE CAMPION (Dominique.LECAMPION@enst-bretagne.fr)
 *  May 1996
 *
 *  This file was taken from the DMV152 bsp
 */

#define M147_INIT

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/zilog/z8530.h>

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

  status = rtems_io_register_name(
    "/dev/console",
    major,
    (rtems_device_minor_number) 0
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
  char *ch
)
{
  uint8_t         rr_0;

  for ( ; ; ) {
    Z8x30_READ_CONTROL( CONSOLE_CONTROL, RR_0, rr_0 );
    if ( !(rr_0 & RR_0_RX_DATA_AVAILABLE) )
      return false;

    Z8x30_READ_DATA( CONSOLE_DATA, *ch );
    return true;
  }
}

/*  inbyte
 *
 *  This routine reads a character from the SCC.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    character read from SCC
 */

char inbyte( void )
{
  uint8_t         rr_0;
  char ch;

  for ( ; ; ) {
    Z8x30_READ_CONTROL( CONSOLE_CONTROL, RR_0, rr_0 );
    if ( (rr_0 & RR_0_RX_DATA_AVAILABLE) != 0 )
      break;
  }

  Z8x30_READ_DATA( CONSOLE_DATA, ch );
  return ( ch );
}

/*  outbyte
 *
 *  This routine transmits a character out the SCC.  It supports
 *  XON/XOFF flow control.
 *
 *  Input parameters:
 *    ch  - character to be transmitted
 *
 *  Output parameters:  NONE
 */

void outbyte(
  char ch
)
{
  uint8_t         rr_0;
  char            flow_control;

  for ( ; ; ) {
    Z8x30_READ_CONTROL( CONSOLE_CONTROL, RR_0, rr_0 );
    if ( (rr_0 & RR_0_TX_BUFFER_EMPTY) != 0 )
      break;
  }

  for ( ; ; ) {
    Z8x30_READ_CONTROL( CONSOLE_CONTROL, RR_0, rr_0 );
    if ( (rr_0 & RR_0_RX_DATA_AVAILABLE) == 0 )
      break;

    Z8x30_READ_DATA( CONSOLE_DATA, flow_control );

    if ( flow_control == XOFF )
      do {
        do {
          Z8x30_READ_CONTROL( CONSOLE_CONTROL, RR_0, rr_0 );
        } while ( (rr_0 & RR_0_RX_DATA_AVAILABLE) == 0 );
        Z8x30_READ_DATA( CONSOLE_DATA, flow_control );
      } while ( flow_control != XON );
  }

  Z8x30_WRITE_DATA( CONSOLE_DATA, ch );
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

  for (count = 0; count < maximum; count++) {
    buffer[ count ] = inbyte();
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

  for (count = 0; count < maximum; count++) {
    if ( buffer[ count ] == '\n') {
      outbyte('\r');
    }
    outbyte( buffer[ count ] );
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
