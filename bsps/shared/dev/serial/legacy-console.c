/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup Console
 *
 * @brief Extension of the generic libchip console driver shell
 */

/*
 *  COPYRIGHT (c) 1989-2011, 2016.
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

#include <bsp.h>
#include <bsp/fatal.h>
#include <rtems/libio.h>
#include <rtems/console.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <termios.h>

#include <rtems/termiostypes.h>
#include <libchip/serial.h>
#include "legacy-console.h"

unsigned long               Console_Port_Count  = 0;
console_tbl               **Console_Port_Tbl    = NULL;
console_data               *Console_Port_Data   = NULL;
rtems_device_minor_number   Console_Port_Minor  = 0;
static bool                 console_initialized = false;

/*
 *  console_find_console_entry
 *
 *  This method is used to search the console entries for a
 *  specific device entry.
 */
console_tbl* console_find_console_entry(
  const char                *match,
  size_t                     length,
  rtems_device_minor_number *match_minor
)
{
  rtems_device_minor_number  minor;

  /*
   * The the match name is NULL get the minor number entry.
   */
  if (match == NULL) {
    if (*match_minor < Console_Port_Count)
      return Console_Port_Tbl[*match_minor];
    return NULL;
  }

  for (minor=0; minor < Console_Port_Count ; minor++) {
    console_tbl  *cptr = Console_Port_Tbl[minor];

    /*
     * Console table entries include /dev/ prefix, device names passed
     * in on command line do not.
     */
    if ( !strncmp( cptr->sDeviceName, match, length ) ) {
      *match_minor = minor;
      return cptr;
    }
  }

  return NULL;
}

/*
 *  console_initialize_data
 *
 *  This method is used to initialize the table of pointers to the
 *  serial port configuration structure entries.
 */
void console_initialize_data(void)
{
  int i;

  if ( Console_Port_Tbl )
    return;

  /*
   * Allocate memory for the table of device pointers.
   */
  Console_Port_Count = Console_Configuration_Count;
  Console_Port_Tbl   = malloc( Console_Port_Count * sizeof( console_tbl * ) );
  if (Console_Port_Tbl == NULL)
    bsp_fatal( BSP_FATAL_CONSOLE_NO_MEMORY_0 );

  /*
   * Allocate memory for the table of device specific data pointers.
   */
  Console_Port_Data  = calloc( Console_Port_Count, sizeof( console_data ) );
  if ( Console_Port_Data == NULL ) {
    bsp_fatal( BSP_FATAL_CONSOLE_NO_MEMORY_3 );
  }

  /*
   * Fill in the Console Table
   */
  for (i=0 ; i < Console_Port_Count ; i++) {
    Console_Port_Tbl[i] = &Console_Configuration_Ports[i];
  }
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

  /*
   * Initialize the console data elements
   */
  console_initialize_data();

  /*
   *  console_initialize() has been invoked so it is now too late to
   *  register devices.
   */
  if ( console_initialized ) {
    bsp_fatal( BSP_FATAL_CONSOLE_MULTI_INIT );
  }

  /*
   *  Allocate memory for the console port extension
   */
  old_number_of_ports = Console_Port_Count;
  Console_Port_Count += number_of_ports;
  Console_Port_Tbl = realloc(
    Console_Port_Tbl,
    Console_Port_Count * sizeof(console_tbl *)
  );
  if ( Console_Port_Tbl == NULL ) {
    bsp_fatal( BSP_FATAL_CONSOLE_NO_MEMORY_1 );
  }

  /*
   * Since we can only add devices before console_initialize(),
   * the data area will contain no information and must be zero
   * before it is used. So extend the area and zero it out.
   */
  Console_Port_Data = realloc(
    Console_Port_Data,
    Console_Port_Count * sizeof(console_data)
  );
  if ( Console_Port_Data == NULL ) {
    bsp_fatal( BSP_FATAL_CONSOLE_NO_MEMORY_2 );
  }
  memset(Console_Port_Data, '\0', Console_Port_Count * sizeof(console_data));

  /*
   *  Now add the new devices at the end.
   */
  for (i=0 ; i < number_of_ports ; i++) {
    Console_Port_Tbl[old_number_of_ports + i]  = &new_ports[i];
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
  if (cptr->pDeviceFns->deviceOutputUsesInterrupts) {
    Callbacks.outputUsesInterrupts = TERMIOS_IRQ_DRIVEN;
  } else {
    Callbacks.outputUsesInterrupts = TERMIOS_POLLED;
  }

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
      IoctlArgs.command = TIOCGETA;
      IoctlArgs.buffer  = &Termios;
      rtems_termios_ioctl( &IoctlArgs );

      Termios.c_lflag   = ICANON;
      IoctlArgs.command = TIOCSETA;
      rtems_termios_ioctl( &IoctlArgs );
    }
  }

  if (rtems_libio_iop_is_readable(args->iop) &&
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
  (void) major;

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
    if (rtems_libio_iop_is_readable(args->iop) &&
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
  (void) major;
  (void) minor_arg;
  (void) arg;

  rtems_status_code          status;
  rtems_device_minor_number  minor;
  console_tbl               *port;

  /*
   * If we have no devices which were registered earlier then we
   * must still initialize pointers for Console_Port_Tbl and
   * Console_Port_Data.
   */
  console_initialize_data();

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
          bsp_fatal( BSP_FATAL_CONSOLE_REGISTER_DEV_0 );
        }
      }

      if (minor == Console_Port_Minor) {
        status = rtems_io_register_name( CONSOLE_DEVICE_NAME, major, minor );
        if (status != RTEMS_SUCCESSFUL) {
          bsp_fatal( BSP_FATAL_CONSOLE_REGISTER_DEV_1 );
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

/*
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
  (void) major;
  (void) minor;

  return rtems_termios_read (arg);
}

/*
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
  (void) major;
  (void) minor;

  return rtems_termios_write (arg);
}
