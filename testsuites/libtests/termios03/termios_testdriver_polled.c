/*
 *  This file contains a test fixture termios device driver
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include "tmacros.h"
#include <rtems/libio.h>
#include <stdlib.h>
#include <termios.h>
#include <rtems/termiostypes.h>
#include "termios_testdriver_polled.h"
#include <unistd.h>

int termios_test_driver_inbyte_nonblocking( int port )
{
  return -1;
}

void termios_test_driver_outbyte_polled(
  int  port,
  char ch
)
{
  write( 2, &ch, 1 );
}

ssize_t termios_test_driver_write_support (int minor, const char *buf, size_t len)
{
  size_t nwrite = 0;

  while (nwrite < len) {
    termios_test_driver_outbyte_polled( minor, *buf++ );
    nwrite++;
  }
  return nwrite;
}


/*
 *  Set Attributes Handler
 */
int termios_test_driver_set_attributes(
  int                   minor,
  const struct termios *t
)
{
  return 0;
}

/*
 *  Test Device Driver Entry Points
 */
rtems_device_driver termios_test_driver_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_termios_initialize();

  /*
   *  Register Device Names
   */
  (void) rtems_io_register_name( TERMIOS_TEST_DRIVER_DEVICE_NAME, major, 0 );

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver termios_test_driver_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_status_code sc;
  static const rtems_termios_callbacks Callbacks = {
    NULL,                                    /* firstOpen */
    NULL,                                    /* lastClose */
    termios_test_driver_inbyte_nonblocking,  /* pollRead */
    termios_test_driver_write_support,       /* write */
    termios_test_driver_set_attributes,      /* setAttributes */
    NULL,                                    /* stopRemoteTx */
    NULL,                                    /* startRemoteTx */
    0                                        /* outputUsesInterrupts */
  };

  if ( minor > 2 ) {
    puts( "ERROR - Termios_testdriver - only 1 minor supported" );
    rtems_test_exit(0);
  }

  sc = rtems_termios_open (major, minor, arg, &Callbacks);

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver termios_test_driver_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_close (arg);
}

rtems_device_driver termios_test_driver_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_read (arg);
}

rtems_device_driver termios_test_driver_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_write (arg);
}

rtems_device_driver termios_test_driver_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_ioctl (arg);
}
