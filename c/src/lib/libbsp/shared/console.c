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
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>
#include <termios.h>

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
  Callbacks.stopRemoteTx         = cptr->pDeviceFlow->deviceStopRemoteTx;
  Callbacks.startRemoteTx        = cptr->pDeviceFlow->deviceStartRemoteTx;
  Callbacks.outputUsesInterrupts = cptr->pDeviceFns->deviceOutputUsesInterrupts;

  /* XXX what about 
   *        Console_Port_Tbl[minor].ulMargin,
   *        Console_Port_Tbl[minor].ulHysteresis);
   */

  status = rtems_termios_open ( major, minor, arg, &Callbacks );
  Console_Port_Data[minor].termios_data = args->iop->data1;

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

  if ( (args->iop->flags&LIBIO_FLAGS_READ) &&
        Console_Port_Tbl[minor].pDeviceFlow &&
        Console_Port_Tbl[minor].pDeviceFlow->deviceStopRemoteTx) {
    Console_Port_Tbl[minor].pDeviceFlow->deviceStopRemoteTx(minor);
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
  rtems_device_minor_number  minor_arg,
  void                      *arg
)
{
  rtems_status_code          status;
  rtems_device_minor_number  minor;

  /*
   * initialize the termio interface.
   */

  rtems_termios_initialize();

  for (minor=0; minor < Console_Port_Count ; minor++) {
    /*
     * First perform the configuration dependent probe, then the
     * device dependent probe
     */

    if ((!Console_Port_Tbl[minor].deviceProbe ||
         Console_Port_Tbl[minor].deviceProbe(minor)) &&
         Console_Port_Tbl[minor].pDeviceFns->deviceProbe(minor)) {
      /*
       * Use this device for the console
       */
      break;
    }
  }
  if ( minor == Console_Port_Count ) {
    /*
     * Failed to find a working device
     */
    rtems_fatal_error_occurred(RTEMS_IO_ERROR);
  }
  
  Console_Port_Minor=minor;

  /*
   * Register Device Names
   */
  status = rtems_io_register_name("/dev/console", major, Console_Port_Minor );
  if (status != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(status);
  }
  Console_Port_Tbl[minor].pDeviceFns->deviceInitialize(Console_Port_Minor);

  for (minor++;minor<Console_Port_Count;minor++) {
    /*
     * First perform the configuration dependent probe, then the
     * device dependent probe
     */

    if ( (!Console_Port_Tbl[minor].deviceProbe ||
         Console_Port_Tbl[minor].deviceProbe(minor)) &&
         Console_Port_Tbl[minor].pDeviceFns->deviceProbe(minor)) {
      status = rtems_io_register_name(
        Console_Port_Tbl[minor].sDeviceName,
        major,
        minor );
      if (status != RTEMS_SUCCESSFUL) {
        rtems_fatal_error_occurred(status);
      }

      /*
       * Initialize the hardware device.
       */

      Console_Port_Tbl[minor].pDeviceFns->deviceInitialize(minor);

    }
  }

  return RTEMS_SUCCESSFUL;
}



