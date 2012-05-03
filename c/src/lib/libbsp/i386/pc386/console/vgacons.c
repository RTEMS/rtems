/*
 *  This file contains the termios TTY driver for the i386 
 *  vga.
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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

#define VGACONS_STATIC static

static int isr_is_on(const rtems_irq_connect_data *irq)
{
  return BSP_irq_enabled_at_i8259s(irq->name);
}

static rtems_irq_connect_data keyboard_isr_data = {
  BSP_KEYBOARD,
  keyboard_interrupt,
  0,
  NULL,
  NULL,
  isr_is_on
};

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
  int         status;
  static bool firstTime = true;

  if ((*(unsigned char*) NB_MAX_ROW_ADDR == 0) &&
      (*(unsigned short*)NB_MAX_COL_ADDR == 0)) {
    return false;
  }

  /*
   *  If there is a video card, let's assume there is also a keyboard.
   *  The means that we need the ISR installed in case someone wants to
   *  use the Keyboard or PS2 Mouse.  With Microwindows, the console
   *  can be COM1 and you can still use the mouse/VGA for graphics.
   */
  if ( firstTime ) {
    status = BSP_install_rtems_irq_handler(&keyboard_isr_data);
    if (!status) {
      printk("Error installing keyboard interrupt handler!\n");
      rtems_fatal_error_occurred(status);
    }
  }
  firstTime = false;

  return true;
}

console_fns vgacons_fns =
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
