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
#include <rtems/termiostypes.h>

#include <stdlib.h>

#include <libchip/serial.h>
#include <libchip/sersupp.h>

#include <bsp.h>
#include <bsp/fbcons.h>
#include <bsp/vc.h>
#include <bsp/rpi-fb.h>

/*
 *  fbcons_open
 *
 *  This function opens a port for communication.
 *
 *  Default state is 9600 baud, 8 bits, No parity, and 1 stop bit.
 */
static bool fbcons_open(
  struct rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  return true;
}

/*
 *  fbcons_close
 *
 *  This function shuts down the requested port.
 */
static void fbcons_close(
  struct rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  rtems_libio_open_close_args_t *args
)
{
}

/*
 *  fbcons_write_polled
 *
 *  This routine polls out the requested character.
 */
void fbcons_write_polled(
  rtems_termios_device_context *base,
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
static void fbcons_write_support_polled(
  rtems_termios_device_context *base,
  const char *buf,
  size_t      len
)
{
  int nwrite = 0;

  /*
   * poll each byte in the string out of the port.
   */
  while ( nwrite < len ) {
    fbcons_write_polled( base, *buf++ );
    nwrite++;
  }
}

/*
 *  fbcons_inbyte_nonblocking_polled
 *
 *  Console Termios polling input entry point.
 */
static int fbcons_inbyte_nonblocking_polled(
  rtems_termios_device_context *base
)
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
static bool fbcons_set_attributes(
  rtems_termios_device_context *base,
  const struct termios *t
)
{
  return true;
}

bool fbcons_probe(
  rtems_termios_device_context *context
)
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

const rtems_termios_device_handler fbcons_fns =
{
  .first_open = fbcons_open,
  .last_close = fbcons_close,
  .poll_read = fbcons_inbyte_nonblocking_polled,
  .write = fbcons_write_support_polled,
  .set_attributes = fbcons_set_attributes,
  .mode = TERMIOS_POLLED
};