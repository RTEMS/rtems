/*
 *  This file contains the template for a console IO package.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>
#include "concntl.h"
/* #include "pcimsgreg.h" XXX JRS */

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
 
     if ( console_pmr_init(*(unsigned32*)arg) )
        return RTEMS_INVALID_NUMBER;

 
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
  return(console_pmr_kbhit());
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
  return console_pmr_getc();
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
  console_pmr_putc( ch );

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
      buffer[ count ]  = 0;
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

  for (count = 0; count < maximum; count++) {
    if ( buffer[ count ] == '\n') {
      outbyte(minor,'\r');
    }
    outbyte( minor,buffer[ count ] );
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
  if (!arg)
     return RTEMS_INVALID_ADDRESS;

  switch( ((console_ioctl_request_t *)arg)->ioctl_type )
  {
     case CON_KBHIT:
        /* check if keyboard was hit */
        ((console_ioctl_request_t *)arg)->param = console_pmr_kbhit();
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
