/**
 * @file
 *
 * @ingroup Console
 *
 * @brief Extension of the generic libchip console driver shell
 */

/*
 *  COPYRIGHT (c) 1989-2011.
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
#include <termios.h>

#include <rtems/termiostypes.h>
#include <rtems/error.h>  /* rtems_panic */
#include <libchip/serial.h>
#include "console_private.h"

unsigned long               Console_Port_Count  = 0;
console_tbl               **Console_Port_Tbl    = NULL;
console_data               *Console_Port_Data   = NULL;
rtems_device_minor_number   Console_Port_Minor  = 0;
static bool                 console_initialized = false;

/*
 *  console_initialize_pointers
 *
 *  This method is used to initialize the table of pointers to the
 *  serial port configuration structure entries.
 */
static void console_initialize_pointers(void)
{
  int i;

  if ( Console_Port_Tbl )
    return;

  Console_Port_Count = Console_Configuration_Count;
  Console_Port_Tbl   = malloc( Console_Port_Count * sizeof( console_tbl * ) );
  if (Console_Port_Tbl == NULL)
    rtems_panic("No memory for console pointers");

  for (i=0 ; i < Console_Port_Count ; i++)
    Console_Port_Tbl[i] = &Console_Configuration_Ports[i];
}

/*
 *  console_register_devices
 *
 *  This method is used to add dynamically discovered devices to the
 *  set of serial ports supported.
 */
void console_register_devices(
  console_tbl *new_ports,
  size_t       number_of_ports
)
{
  int  old_number_of_ports;
  int  i;

  console_initialize_pointers();

  /*
   *  console_initialize has been invoked so it is now too late to
   *  register devices.
   */
  if ( console_initialized ) {
    printk( "Attempt to register console devices after driver initialized\n" );
    rtems_fatal_error_occurred( 0xdead0001 );
  }

  /*
   *  Allocate memory for the console port extension
   */
  old_number_of_ports = Console_Port_Count;
  Console_Port_Count += number_of_ports;
  Console_Port_Tbl = realloc(
    Console_Port_Tbl,
    Console_Port_Count * sizeof( console_tbl * )
  );
  if ( Console_Port_Tbl == NULL ) {
    printk( "Unable to allocate pointer table for registering console devices\n" );
    rtems_fatal_error_occurred( 0xdead0002 );
  }

  Console_Port_Data  = calloc( Console_Port_Count, sizeof( console_data ) );
  if ( Console_Port_Data == NULL ) {
    printk( "Unable to allocate data table for console devices\n" );
    rtems_fatal_error_occurred( 0xdead0003 );
  }

  /*
   *  Now add the new devices at the end.
   */

  for (i=0 ; i < number_of_ports ; i++) {
    Console_Port_Tbl[old_number_of_ports + i] = &new_ports[i];
  }
}

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

  cptr = Console_Port_Tbl[minor];
  Callbacks.firstOpen            = cptr->pDeviceFns->deviceFirstOpen;
  Callbacks.lastClose            = cptr->pDeviceFns->deviceLastClose;
  Callbacks.pollRead             = cptr->pDeviceFns->deviceRead;
  Callbacks.write                = cptr->pDeviceFns->deviceWrite;
  Callbacks.setAttributes        = cptr->pDeviceFns->deviceSetAttributes;
  if (cptr->pDeviceFlow != NULL) {
    Callbacks.stopRemoteTx  = cptr->pDeviceFlow->deviceStopRemoteTx;
    Callbacks.startRemoteTx = cptr->pDeviceFlow->deviceStartRemoteTx;
  } else {
    Callbacks.stopRemoteTx  = NULL;
    Callbacks.startRemoteTx = NULL;
  }
  Callbacks.outputUsesInterrupts = cptr->pDeviceFns->deviceOutputUsesInterrupts;

  /* XXX what about
   *        Console_Port_Tbl[minor].ulMargin,
   *        Console_Port_Tbl[minor].ulHysteresis);
   */

  status = rtems_termios_open( major, minor, arg, &Callbacks );
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
    if ( minor != Console_Port_Minor ) {
      /*
       * If this is not the console we do not want ECHO and so forth
       */
      IoctlArgs.iop     = args->iop;
      IoctlArgs.command = RTEMS_IO_GET_ATTRIBUTES;
      IoctlArgs.buffer  = &Termios;
      rtems_termios_ioctl( &IoctlArgs );

      Termios.c_lflag   = ICANON;
      IoctlArgs.command = RTEMS_IO_SET_ATTRIBUTES;
      rtems_termios_ioctl( &IoctlArgs );
    }
  }

  if ( (args->iop->flags&LIBIO_FLAGS_READ) &&
      cptr->pDeviceFlow &&
      cptr->pDeviceFlow->deviceStartRemoteTx) {
    cptr->pDeviceFlow->deviceStartRemoteTx(minor);
  }

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
  rtems_libio_open_close_args_t *args = arg;
  struct rtems_termios_tty      *current_tty;
  console_tbl                   *cptr;

  cptr  = Console_Port_Tbl[minor];

  /* Get tty pointer from the Console_Port_Data */
  current_tty = Console_Port_Data[minor].termios_data;

  /* Get the tty refcount to determine if we need to do deviceStopRemoteTx.
   * Stop only if it's the last one opened.
   */
  if ( (current_tty->refcount == 1) ) {
    if ( (args->iop->flags&LIBIO_FLAGS_READ) &&
          cptr->pDeviceFlow &&
          cptr->pDeviceFlow->deviceStopRemoteTx) {
      cptr->pDeviceFlow->deviceStopRemoteTx(minor);
    }
  }

  return rtems_termios_close (arg);
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
  rtems_device_minor_number  minor;
  console_tbl               *port;

  /*
   * If we have no devices which were registered earlier then we
   * must still initialize pointers and set Console_Port_Data.
   */
  if ( ! Console_Port_Tbl ) {
    console_initialize_pointers();
    Console_Port_Data  = calloc( Console_Port_Count, sizeof( console_data ) );
    if ( Console_Port_Data == NULL ) {
      printk( "Unable to allocate data table for console devices\n" );
      rtems_fatal_error_occurred( 0xdead0003 );
    }
  }

  /*
   *  console_initialize has been invoked so it is now too late to
   *  register devices.
   */
  console_initialized = true;

  /*
   *  Initialize the termio interface, our table of pointers to device
   *  information structures, and determine if the user has explicitly
   *  specified which device is to be used for the console.
   */
  rtems_termios_initialize();
  bsp_console_select();

  /*
   *  Iterate over all of the console devices we know about
   *  and initialize them.
   */
  for (minor=0 ; minor < Console_Port_Count ; minor++) {
    /*
     *  First perform the configuration dependent probe, then the
     *  device dependent probe
     */
    port = Console_Port_Tbl[minor];

    if ( (!port->deviceProbe || port->deviceProbe(minor)) &&
         port->pDeviceFns->deviceProbe(minor)) {

      if (port->sDeviceName != NULL) {
        status = rtems_io_register_name( port->sDeviceName, major, minor );
        if (status != RTEMS_SUCCESSFUL) {
          printk( "Unable to register %s\n",  port->sDeviceName );
          rtems_fatal_error_occurred(status);
        }
      }

      if (minor == Console_Port_Minor) {
#if defined(RTEMS_DEBUG)
          if (port->sDeviceName != NULL)
            printk( "Register %s as the CONSOLE\n", port->sDeviceName );
#endif
        status = rtems_io_register_name( "dev/console", major, minor );
        if (status != RTEMS_SUCCESSFUL) {
          printk( "Unable to register /dev/console\n" );
          rtems_fatal_error_occurred(status);
        }
      }

      /*
       * Initialize the hardware device.
       */
      port->pDeviceFns->deviceInitialize(minor);

    }
  }

  return RTEMS_SUCCESSFUL;
}
