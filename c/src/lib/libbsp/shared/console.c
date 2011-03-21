/*
 *  This file contains the generic console driver shell used
 *  by all console drivers using libchip.
 *
 *  This driver uses the termios pseudo driver.
 *
 *  COPYRIGHT (c) 1989-1997.
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
#include <rtems/console.h>
#include <stdlib.h>
#include <assert.h>
#include <termios.h>

#include <rtems/termiostypes.h>
#include <libchip/serial.h>

/*
 *  Configuration Information
 */

extern console_data  Console_Port_Data[];
extern unsigned long  Console_Port_Count;
extern rtems_device_minor_number  Console_Port_Minor;

/*PAGE
 *
 *  console_open
 *
 *  open a port as a termios console.
 *
 */

rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_status_code              status;
  rtems_libio_open_close_args_t *args = arg;
  rtems_libio_ioctl_args_t       IoctlArgs;
  struct termios                 Termios;
  rtems_termios_callbacks        Callbacks;
  console_tbl                   *cptr;
  struct rtems_termios_tty      *current_tty;

  /*
   * Verify the port number is valid.
   */
  if ( minor > Console_Port_Count ) {
    return RTEMS_INVALID_NUMBER;
  }

  /*
   * Open the port as a termios console driver.
   */

  cptr = &Console_Port_Tbl[minor];
  Callbacks.firstOpen            = cptr->pDeviceFns->deviceFirstOpen;
  Callbacks.lastClose            = cptr->pDeviceFns->deviceLastClose;
  Callbacks.pollRead             = cptr->pDeviceFns->deviceRead;
  Callbacks.write                = cptr->pDeviceFns->deviceWrite;
  Callbacks.setAttributes        = cptr->pDeviceFns->deviceSetAttributes;
  if (cptr->pDeviceFlow != NULL) {
    Callbacks.stopRemoteTx = cptr->pDeviceFlow->deviceStopRemoteTx;
    Callbacks.startRemoteTx = cptr->pDeviceFlow->deviceStartRemoteTx;
  } else {
    Callbacks.stopRemoteTx = NULL;
    Callbacks.startRemoteTx = NULL;
  }
  Callbacks.outputUsesInterrupts = cptr->pDeviceFns->deviceOutputUsesInterrupts;

  /* XXX what about
   *        Console_Port_Tbl[minor].ulMargin,
   *        Console_Port_Tbl[minor].ulHysteresis);
   */

  status = rtems_termios_open ( major, minor, arg, &Callbacks );
  Console_Port_Data[minor].termios_data = args->iop->data1;

  /* Get tty pointur from the Console_Port_Data */
  current_tty = Console_Port_Data[minor].termios_data;

  if ( (current_tty->refcount == 1) ) {

    /*
     *  If this BSP has a preferred default rate, then use that.
     */
    #if defined(BSP_DEFAULT_BAUD_RATE)
      rtems_termios_set_initial_baud( current_tty, BSP_DEFAULT_BAUD_RATE );
    #endif

    /*
     * If it's the first open, modified, if need, the port parameters
     */
    if (minor!=Console_Port_Minor) {
      /*
       * If this is not the console we do not want ECHO and
       * so forth
       */
      IoctlArgs.iop=args->iop;
      IoctlArgs.command=RTEMS_IO_GET_ATTRIBUTES;
      IoctlArgs.buffer=&Termios;
      rtems_termios_ioctl(&IoctlArgs);
      Termios.c_lflag=ICANON;
      IoctlArgs.command=RTEMS_IO_SET_ATTRIBUTES;
      rtems_termios_ioctl(&IoctlArgs);
    }
  }

  if ( (args->iop->flags&LIBIO_FLAGS_READ) &&
      Console_Port_Tbl[minor].pDeviceFlow &&
      Console_Port_Tbl[minor].pDeviceFlow->deviceStartRemoteTx) {
    Console_Port_Tbl[minor].pDeviceFlow->deviceStartRemoteTx(minor);
  }

  return status;
}

/*PAGE
 *
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
  rtems_libio_open_close_args_t *args = arg;
  struct rtems_termios_tty      *current_tty;

  /* Get tty pointeur from the Console_Port_Data */
  current_tty = Console_Port_Data[minor].termios_data;

  /* Get the tty refcount to determine if we need to do deviceStopRemoteTx.
   * Stop only if it's the last one opened.
   */
  if ( (current_tty->refcount == 1) ) {
    if ( (args->iop->flags&LIBIO_FLAGS_READ) &&
          Console_Port_Tbl[minor].pDeviceFlow &&
          Console_Port_Tbl[minor].pDeviceFlow->deviceStopRemoteTx) {
      Console_Port_Tbl[minor].pDeviceFlow->deviceStopRemoteTx(minor);
    }
  }

  return rtems_termios_close (arg);
}

/*PAGE
 *
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

/*PAGE
 *
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

/*PAGE
 *
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

/*PAGE
 *
 *  console_initialize
 *
 *  Routine called to initialize the console device driver.
 */

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  bool first = true;

  rtems_termios_initialize();

  for (minor = 0; minor < Console_Port_Count; ++minor) {
    const console_tbl *device = &Console_Port_Tbl [minor];

    if (
      (device->deviceProbe == NULL || device->deviceProbe(minor))
        && device->pDeviceFns->deviceProbe(minor)
    ) {
      device->pDeviceFns->deviceInitialize(minor);
      if (first) {
        first = false;
        Console_Port_Minor = minor;
        sc = rtems_io_register_name(CONSOLE_DEVICE_NAME, major, minor);
        if (sc != RTEMS_SUCCESSFUL) {
          rtems_fatal_error_occurred(sc);
        }
      }
      if (device->sDeviceName != NULL) {
        sc = rtems_io_register_name(device->sDeviceName, major, minor);
        if (sc != RTEMS_SUCCESSFUL) {
          rtems_fatal_error_occurred(sc);
        }
      }
    }
  }

  if (first) {
    /*
     * Failed to find a working device
     */
    rtems_fatal_error_occurred(RTEMS_IO_ERROR);
  }

  return RTEMS_SUCCESSFUL;
}
