/*
 *  This file contains the template for a console IO package.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <bsp.h>

#if defined(SERIAL_INPUT) && defined(HIF_INPUT)
#error SERIAL_INPUT and HIF_INPUT cannot both be defined!!!
#endif

#include <rtems/libio.h>
#include "serial.h"
#include "concntl.h"

#ifndef lint
static char _sccsid[] = "@(#)console.c 09/12/96     1.13\n";
#endif

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
 
  if ( arg )
  {
     if ( console_duartinit(minor,*(unsigned32*)arg) )
        return RTEMS_INVALID_NUMBER;
  }
  else
  {
     if ( console_duartinit(1,9600) || console_duartinit(0,9600) )
     {
        return RTEMS_INVALID_NUMBER;
     }
  }
 
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

rtems_boolean is_character_ready(
  char *ch
)
{
  *ch = '\0';   /* return NULL for no particular reason */
  return(TRUE);
}

/*  inbyte
 *
 *  This routine reads a character from the SOURCE.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    character read from SOURCE
 */

char inbyte( unsigned int minor )
{
  /*
   *  If polling, wait until a character is available.
   */
#ifdef HIF_INPUT
  char retch;
  _read( 1, &retch, 1 );
  return retch;
#endif
#ifdef SERIAL_INPUT
  return console_sps_getc( minor );
#endif
}

/*  outbyte
 *
 *  This routine transmits a character out the SOURCE.  It may support
 *  XON/XOFF flow control.
 *
 *  Input parameters:
 *    ch  - character to be transmitted
 *
 *  Output parameters:  NONE
 */

void outbyte( unsigned int minor,
  char ch
)
{
  /*
   *  If polling, wait for the transmitter to be ready.
   *  Check for flow control requests and process.
   *  Then output the character.
   */

#ifdef SERIAL_OUTPUT
  console_sps_putc( minor, ch );
#endif

  /*
   *  Carriage Return/New line translation.
   */

  if ( ch == '\n' )
    outbyte( minor, '\r' );
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
  unsigned8 *buffer;
  unsigned32 maximum;
  unsigned32 count = 0;
 
  rw_args = (rtems_libio_rw_args_t *) arg;

  buffer = rw_args->buffer;
  maximum = rw_args->count;

  for (count = 0; count < maximum; count++) {
    buffer[ count ] = inbyte(minor);
    if (buffer[ count ] == '\n' || buffer[ count ] == '\r') {
      buffer[ count++ ]  = '\n';
      outbyte( minor, '\n' ); /* newline */
      break;
    }
    else if (buffer[ count ] == '\b' && count > 0 )
    {
      outbyte( minor, '\b' ); /* move back one space */
      outbyte( minor, ' ' ); /* erase the character */
      outbyte( minor, '\b' ); /* move back one space */
      count-=2;
    }
    else
      outbyte( minor, buffer[ count ] ); /* echo the character */
  }

  rw_args->bytes_moved = count;
  return (count > 0) ? RTEMS_SUCCESSFUL : RTEMS_UNSATISFIED;
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
  unsigned8 *buffer;

  rw_args = (rtems_libio_rw_args_t *) arg;

  buffer = rw_args->buffer;
  maximum = rw_args->count;

#ifdef HIF_OUTPUT
  _write( 0, buffer, maximum );
#endif
#ifdef SERIAL_OUTPUT
  for (count = 0; count < maximum; count++) {
    if ( buffer[ count ] == '\n') {
      outbyte(minor,'\r');
    }
    outbyte( minor,buffer[ count ] );
  }
#endif

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
  if (!arg)
     return RTEMS_INVALID_ADDRESS;

  switch( ((console_ioctl_request_t *)arg)->ioctl_type )
  {
     case CON_KBHIT:
        /* check if keyboard was hit */
        ((console_ioctl_request_t *)arg)->param = console_sps_kbhit(minor);
        break;

     case CON_GET_RAW_BYTE:
        ((console_ioctl_request_t *)arg)->param = inbyte(minor);
        break;

     case CON_SEND_RAW_BYTE:
        outbyte(minor, ((console_ioctl_request_t *)arg)->param);
        break;

     default:
        break;
  }

  return RTEMS_SUCCESSFUL;
}
