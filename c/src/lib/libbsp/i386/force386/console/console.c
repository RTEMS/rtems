/*
 *  This file contains the Force CPU386 console IO package.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#define F386_INIT

#include <bsp.h>
#include <rtems/libio.h>

#include <stdlib.h>

/*  console_cleanup
 *
 *  This routine is called at exit to clean up the console hardware.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 */

void console_cleanup( void )
{
  register rtems_unsigned8 ignored;
  /*
   *  FORCE technical support mentioned that it may be necessary to
   *  read the DUSCC RX_BUFFER port four times to remove all junk.
   *  This code is a little more paranoid.
   */

   inport_byte( RX_BUFFER, ignored );
   inport_byte( RX_BUFFER, ignored );
   inport_byte( RX_BUFFER, ignored );
   inport_byte( RX_BUFFER, ignored );
   inport_byte( RX_BUFFER, ignored );
}

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

  /*
   *  flush the console now and at exit.  Just in case.
   */

  console_cleanup();

  status = rtems_io_register_name(
    "/dev/console",
    major,
    (rtems_device_minor_number) 0
  );
 
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  atexit( console_cleanup );

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

rtems_boolean is_character_ready(
  char *ch
)
{
  register rtems_unsigned8 status;

  inport_byte( RX_STATUS, status );

  if ( Is_rx_ready( status ) ) {
     inport_byte( RX_BUFFER,  status );
     *ch = status;
     return TRUE;
  }
  return FALSE;
}

/*  inbyte
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

char inbyte( void )
{
  register rtems_unsigned8 status;
  char                     ch;

  do {
    inport_byte( RX_STATUS, status );
  } while ( !Is_rx_ready( status ) );

#if ( PORTB == 1 )
  /*
   *  Force example code resets the Channel B Receiver here.
   *  It appears to cause XON's to be lost.
   */

   /* outport_byte( RX_STATUS, 0x10 );  */
#endif

  inport_byte( RX_BUFFER, ch );

  return ch;
}

/*  outbyte
 *
 *  This routine transmits a character out the port.  It supports
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
  rtems_unsigned8 status;

  do {
    inport_byte( TX_STATUS, status );
  } while ( !Is_tx_ready( status ) );

#if 0
  while ( is_character_ready( &status ) == TRUE ) {      /* must be an XOFF */
    if ( status == XOFF )
      do {
        while ( is_character_ready( &status ) == FALSE ) ;
      } while ( status != XON );
  }
#endif

  outport_byte( TX_BUFFER, ch );
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
      buffer[ count ]  = 0;
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

