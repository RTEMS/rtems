/*
 *  This file contains the hardware independent portion of a polled
 *  console device driver.  If a BSP chooses to use this, then it
 *  only has to provide a few board dependent routines.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>

/* external prototypes for monitor interface routines */

void console_outbyte_polled(
  int  port,
  char ch
);

int console_inbyte_nonblocking(
  int port
);

void console_initialize_hardware(void);

/*
 *  Console Termios Support Entry Points
 *
 */
ssize_t console_write_support (
  int         minor,
  const char *bufarg,
  size_t      len
)
{
  int nwrite = 0;
  const char *buf = bufarg;

  while (nwrite < len) {
    console_outbyte_polled( minor, *buf++ );
    nwrite++;
  }
  return nwrite;
}

/*
 *  Console Device Driver Entry Points
 *
 */

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_status_code status;

  /*
   *  Ensure Termios is initialized
   */
  rtems_termios_initialize();

  /*
   *  Make sure the hardware is initialized.
   */
  console_initialize_hardware();

  /*
   *  Register Device Names
   */
  status = rtems_io_register_name( "/dev/console", major, 0 );
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_status_code sc;
  static const rtems_termios_callbacks pollCallbacks = {
    NULL,                        /* firstOpen */
    NULL,                        /* lastClose */
    console_inbyte_nonblocking,  /* pollRead */
    console_write_support,       /* write */
    NULL,                        /* setAttributes */
    NULL,                        /* stopRemoteTx */
    NULL,                        /* startRemoteTx */
    0                            /* outputUsesInterrupts */
  };

  assert( minor <= 1 );
  if ( minor > 1 )
    return RTEMS_INVALID_NUMBER;

  sc = rtems_termios_open( major, minor, arg, &pollCallbacks );

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_close( arg );
}

rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_read( arg );
}

rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_write( arg );
}

rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_ioctl( arg );
}
