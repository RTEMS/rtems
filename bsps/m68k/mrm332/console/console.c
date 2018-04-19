/*
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <termios.h>

#include <rtems/console.h>
#include <rtems/libio.h>
#include <bsp.h>
#include "sci.h"

/*
 *  console_open
 *
 *  open a port as a termios console.
 */
rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
    rtems_status_code status;

    /* the console is opened three times at startup */
    /* for standard input, output, and error */

    /* Get correct callback structure for the device */

    /* argument of FALSE gives us interrupt driven serial io */
    /* argument of TRUE  gives us polling   based  serial io */

    /* SCI internal uart */

    status = rtems_termios_open( major, minor, arg, SciGetTermiosHandlers( FALSE ) );

    return status;
}

/*
 *  console_close
 *
 *  This routine closes a port that has been opened as console.
 */
rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_close (arg);
}

/*
 *  console_read
 *
 *  This routine uses the termios driver to read a character.
 */
rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_read (arg);
}

/*
 *  console_write
 *
 *  this routine uses the termios driver to write a character.
 */
rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_write (arg);
}

/*
 *  console_control
 *
 *  this routine uses the termios driver to process io
 */

rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_ioctl (arg);
}

/*
 *  console_initialize
 *
 *  Routine called to initialize the console device driver.
 */
rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor_arg,
  void                      *arg
)
{
  rtems_status_code          status;

  /*
   * initialize the termio interface.
   */
  rtems_termios_initialize();

  /*
   * register the SCI device name for termios
   * do this over in the sci driver init routine?
   */

  status = rtems_io_register_name( "/dev/sci", major, 0 );

  if (status != RTEMS_SUCCESSFUL)
  {
    rtems_fatal_error_occurred(status);
  }

  /*
   * Link the uart device to the console device
   */

#if 1
  status = rtems_io_register_name( "/dev/console", major, 0 );

  if (status != RTEMS_SUCCESSFUL)
  {
    rtems_fatal_error_occurred(status);
  }
#else
  if ( link( "/dev/sci", "/dev/console") < 0 )
  {
    rtems_fatal_error_occurred( RTEMS_IO_ERROR );
  }
#endif

  /*
   * Console Initialize Succesful
   */

  return RTEMS_SUCCESSFUL;
}
