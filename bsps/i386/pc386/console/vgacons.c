/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This file contains the termios TTY driver for the i386
 *  vga.
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <rtems.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <libchip/serial.h>
#include <rtems/vgacons.h>
#include <rtems/keyboard.h>
#include <libchip/sersupp.h>
#include <bsp/irq.h>
#include <bsp.h>
#include <crt.h>
#include <assert.h>
#include <rtems/keyboard.h>

#define VGACONS_STATIC static

/*
 *  vgacons_init
 *
 *  This function initializes the VGA console to a quiecsent state.
 */
VGACONS_STATIC void vgacons_init(int minor)
{
  /*
   * Note:  We do not initialize the KBD interface here since
   *        it was initialized regardless of whether the
   *        vga is available or not.  Therefore it is initialized
   *        in bsp_start.
   */
}

/*
 *  vgacons_open
 *
 *  This function opens a port for communication.
 *
 *  Default state is 9600 baud, 8 bits, No parity, and 1 stop bit.
 */
VGACONS_STATIC int vgacons_open(
  int      major,
  int      minor,
  void    *arg
)
{
  return RTEMS_SUCCESSFUL;
}

/*
 *  vgacons_close
 *
 *  This function shuts down the requested port.
 */
VGACONS_STATIC int vgacons_close(
  int      major,
  int      minor,
  void    *arg
)
{
  return(RTEMS_SUCCESSFUL);
}

/*
 *  vgacons_write_polled
 *
 *  This routine polls out the requested character.
 */
VGACONS_STATIC void vgacons_write_polled(
  int   minor,
  char  c
)
{
  _IBMPC_outch( c );
  if( c == '\n')
    _IBMPC_outch( '\r' );            /* LF = LF + CR */
}

/*
 *  vgacons_write_support_polled
 *
 *  Console Termios output entry point when using polled output.
 *
 */
VGACONS_STATIC ssize_t vgacons_write_support_polled(
  int         minor,
  const char *buf,
  size_t      len
)
{
  int nwrite = 0;

  /*
   * poll each byte in the string out of the port.
   */
  while (nwrite < len) {
    vgacons_write_polled(minor, *buf++);
    nwrite++;
  }

  /*
   * return the number of bytes written.
   */
  return nwrite;
}

/*
 *  vgacons_inbyte_nonblocking_polled
 *
 *  Console Termios polling input entry point.
 */
VGACONS_STATIC int vgacons_inbyte_nonblocking_polled(
  int minor
)
{
  if( rtems_kbpoll() ) {
    int c = getch();
    return c;
  }

  return -1;
}

/*
 *  vgacons_set_attributes
 *
 *  This function sets the UART channel to reflect the requested termios
 *  port settings.
 */
VGACONS_STATIC int vgacons_set_attributes(
  int minor,
  const struct termios *t
)
{
  return 0;
}

bool vgacons_probe(
  int minor
)
{
  rtems_status_code status;
  static bool firstTime = true;

  /*
   * See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=99578#c16
   */
  const uint8_t* volatile nb_max_row = (const uint8_t*) NB_MAX_ROW_ADDR;
  const uint16_t* volatile nb_max_col = (const uint16_t*) NB_MAX_COL_ADDR;
  if ((*nb_max_row == 0) && (*nb_max_col == 0)) {
    return false;
  }

  /*
   *  If there is a video card, let's assume there is also a keyboard.
   *  The means that we need the ISR installed in case someone wants to
   *  use the Keyboard or PS2 Mouse.  With Microwindows, the console
   *  can be COM1 and you can still use the mouse/VGA for graphics.
   */
  if ( firstTime ) {
    status = rtems_interrupt_handler_install(
      BSP_KEYBOARD,
      "vgacons",
      RTEMS_INTERRUPT_UNIQUE,
      keyboard_interrupt,
      NULL
    );
    assert(status == RTEMS_SUCCESSFUL);
  }
  firstTime = false;

  return true;
}

const console_fns vgacons_fns =
{
  libchip_serial_default_probe,        /* deviceProbe */
  vgacons_open,                        /* deviceFirstOpen */
  vgacons_close,                       /* deviceLastClose */
  vgacons_inbyte_nonblocking_polled,   /* deviceRead */
  vgacons_write_support_polled,        /* deviceWrite */
  vgacons_init,                        /* deviceInitialize */
  vgacons_write_polled,                /* deviceWritePolled */
  vgacons_set_attributes,              /* deviceSetAttributes */
  FALSE,                               /* deviceOutputUsesInterrupts */
};
