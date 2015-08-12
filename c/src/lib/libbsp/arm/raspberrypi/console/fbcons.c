/**
 * @file
 *
 * @ingroup raspberrypi_console
 *
 * @brief framebuffer graphic console support.
 */

/*
 * Copyright (c) 2015 Yang Qiao
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.org/license/LICENSE
 *
 */

#include <rtems.h>
#include <rtems/libio.h>

#include <stdlib.h>

#include <libchip/serial.h>
#include <libchip/sersupp.h>

#include <bsp.h>
#include <bsp/fbcons.h>
#include <bsp/vc.h>
#include <bsp/rpi-fb.h>

/*
 *  fbcons_init
 *
 *  This function initializes the fb console to a quiecsent state.
 */
static void fbcons_init( int minor )
{
}

/*
 *  fbcons_open
 *
 *  This function opens a port for communication.
 *
 *  Default state is 9600 baud, 8 bits, No parity, and 1 stop bit.
 */
static int fbcons_open(
  int   major,
  int   minor,
  void *arg
)
{
  return RTEMS_SUCCESSFUL;
}

/*
 *  fbcons_close
 *
 *  This function shuts down the requested port.
 */
static int fbcons_close(
  int   major,
  int   minor,
  void *arg
)
{
  return ( RTEMS_SUCCESSFUL );
}

/*
 *  fbcons_write_polled
 *
 *  This routine polls out the requested character.
 */
static void fbcons_write_polled(
  int  minor,
  char c
)
{
  rpi_fb_outch( c );

  if ( c == '\n' )
    rpi_fb_outch( '\r' );            /* LF = LF + CR */
}

/*
 *  fbcons_write_support_polled
 *
 *  Console Termios output entry point when using polled output.
 *
 */
static ssize_t fbcons_write_support_polled(
  int         minor,
  const char *buf,
  size_t      len
)
{
  int nwrite = 0;

  /*
   * poll each byte in the string out of the port.
   */
  while ( nwrite < len ) {
    fbcons_write_polled( minor, *buf++ );
    nwrite++;
  }

  /*
   * return the number of bytes written.
   */
  return nwrite;
}

/*
 *  fbcons_inbyte_nonblocking_polled
 *
 *  Console Termios polling input entry point.
 */
static int fbcons_inbyte_nonblocking_polled( int minor )
{
  // if( rtems_kbpoll() ) {
  //   int c = getch();
  //   return c;
  // }

  return -1;
}

/*
 *  fbcons_set_attributes
 *
 *  This function sets the UART channel to reflect the requested termios
 *  port settings.
 */
static int fbcons_set_attributes(
  int                   minor,
  const struct termios *t
)
{
  return 0;
}

bool fbcons_probe( int minor )
{
  // rtems_status_code status;
  static bool firstTime = true;
  static bool ret = false;

  /*
   *  keyboard interrupt should be registered when the keyboard is available
   */
  if ( firstTime ) {
    if ( !rpi_fb_hdmi_is_present() ) {
      ret = false;
    } else {
      ret = true;
    }
  }

  firstTime = false;

  return ret;
}

const console_fns fbcons_fns =
{
  .deviceProbe = libchip_serial_default_probe,     /* deviceProbe */
  .deviceFirstOpen = fbcons_open,                 /* deviceFirstOpen */
  .deviceLastClose = fbcons_close,                /* deviceLastClose */
  .deviceRead = fbcons_inbyte_nonblocking_polled, /* deviceRead */
  .deviceWrite = fbcons_write_support_polled,     /* deviceWrite */
  .deviceInitialize = fbcons_init,                /* deviceInitialize */
  .deviceWritePolled = fbcons_write_polled,       /* deviceWritePolled */
  .deviceSetAttributes = fbcons_set_attributes,   /* deviceSetAttributes */
  .deviceOutputUsesInterrupts = FALSE,           /* deviceOutputUsesInterrupts*/
};
