/*
 *  This file contains the TTY driver for the serial ports on the DMV152.
 *  The serial ports use a Zilog Z8530.
 *
 *  NOTE: This driver uses the termios pseudo driver.
 *        This driver is polled only.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>

/*
 *  console_outbyte_polled
 *
 *  This routine transmits a character using polling.
 */

void console_outbyte_polled(
  int  port,
  char ch
)
{
  rtems_unsigned32 control;
  rtems_unsigned32 data;
  rtems_unsigned8  rr_0;

  if ( port == 0 ) {
    control = CONSOLE_CONTROL_A;
    data    = CONSOLE_DATA_A;
  } else {
    control = CONSOLE_CONTROL_B;
    data    = CONSOLE_DATA_B;
  }

  for ( ; ; ) {
    Z8x30_READ_CONTROL( control, RR_0, rr_0 );
    if ( (rr_0 & RR_0_TX_BUFFER_EMPTY) != 0 )
      break;
  }

  Z8x30_WRITE_DATA( control, ch );
}

/*
 *  console_inbyte_nonblocking 
 *
 *  This routine polls for a character.
 */

int console_inbyte_nonblocking(int port)
{
  rtems_unsigned32 control;
  rtems_unsigned32 data;
  rtems_unsigned8  rr_0;
  char             ch;

  if ( port == 0 ) {
    control = CONSOLE_CONTROL_A;
    data    = CONSOLE_DATA_A;
  } else {
    control = CONSOLE_CONTROL_B;
    data    = CONSOLE_DATA_B;
  }

  Z8x30_READ_CONTROL( control, RR_0, rr_0 );
  if ( !(rr_0 & RR_0_RX_DATA_AVAILABLE) )
    return -1;

  Z8x30_READ_DATA( data, ch );
  return (int) ch;
}

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

  /* should disable interrupts here */
    for ( s = string ; *s ; s++ ) 
      console_outbyte_polled( 0, *s );

    console_outbyte_polled( 0, '\r' );
    console_outbyte_polled( 0, '\n' );
  /* should enable interrupts here */
}


/*
 *  Console Termios Support Entry Points
 *
 */

int console_write_support (int minor, const char *buf, int len)
{
  int nwrite = 0;

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
  rtems_status_code          status;
  rtems_device_minor_number  console_minor;

  rtems_termios_initialize();

  /*
   *  Register Device Names
   */

#if (USE_CHANNEL_A == 1)
  console_minor = 0;
#elif (USE_CHANNEL_B == 1)
  console_minor = 1;
#else
#error "DMV152 Console Driver -- no console port configured!!!"
#endif

  status = rtems_io_register_name( "/dev/console", major, console_minor );
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  status = rtems_io_register_name( "/dev/console_a", major, 0 );
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  status = rtems_io_register_name( "/dev/console_b", major, 1 );
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  /*
   *  Initialize Hardware
   */
 
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
  if ( minor > 2 )
    return RTEMS_INVALID_NUMBER;

  sc = rtems_termios_open (major, minor, arg, &pollCallbacks );

  return RTEMS_SUCCESSFUL;
}
 
rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_close (arg);
}
 
rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_read (arg);
}
 
rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_write (arg);
}
 
rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_ioctl (arg);
}

