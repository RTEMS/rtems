/*===============================================================*\
| Project: RTEMS generic MPC83xx BSP                              |
+-----------------------------------------------------------------+
| This file has been adapted from the ep1a BSP to MPC83xx by      |
|    Thomas Doerfler <Thomas.Doerfler@embedded-brains.de>         |
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
|                                                                 |
| See the other copyright notice below for the original parts.    |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the console driver                           |
\*===============================================================*/
/* derived from: */
/*
 *  This file contains the TTY driver for the ep1a
 *
 *  This driver uses the termios pseudo driver.
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
#include <termios.h>

#include "console.h"
#include <rtems/bspIo.h>
#include <rtems/termiostypes.h>

/*
 * Load configuration table
 */
#include "config.c"

#define NUM_CONSOLE_PORTS (sizeof(Console_Port_Tbl)/sizeof(console_tbl))

console_data	Console_Port_Data[NUM_CONSOLE_PORTS];
unsigned long	Console_Port_Count;
rtems_device_minor_number  Console_Port_Minor;
bool Console_Is_Initialized = false;		
/* PAGE
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
	rtems_status_code status;
	rtems_libio_open_close_args_t *args = arg;
	rtems_termios_callbacks Callbacks;
	console_fns *c;

	/*
	 * Verify the port number is valid.
	 */
	if(minor>Console_Port_Count)
	{
		return RTEMS_INVALID_NUMBER;
	}

	/*
	 *  open the port as a termios console driver.
	 */
	c = Console_Port_Tbl[minor].pDeviceFns;
	Callbacks.firstOpen     = c->deviceFirstOpen;
	Callbacks.lastClose     = c->deviceLastClose;
	Callbacks.pollRead      = c->deviceRead;
	Callbacks.write         = c->deviceWrite;
	Callbacks.setAttributes = c->deviceSetAttributes;
	Callbacks.stopRemoteTx  = 
		Console_Port_Tbl[minor].pDeviceFlow->deviceStopRemoteTx;
	Callbacks.startRemoteTx = 
		Console_Port_Tbl[minor].pDeviceFlow->deviceStartRemoteTx;
	Callbacks.outputUsesInterrupts = c->deviceOutputUsesInterrupts;
	status = rtems_termios_open ( major, minor, arg, &Callbacks);
	Console_Port_Data[minor].termios_data = args->iop->data1;
	if (status == 0) {
		rtems_termios_set_initial_baud( Console_Port_Data [minor].termios_data, (int) Console_Port_Tbl [minor].pDeviceParams);
	}

	return status;
}
 
rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
	rtems_libio_open_close_args_t *args = arg;

	if((args->iop->flags&LIBIO_FLAGS_READ) &&
	   Console_Port_Tbl[minor].pDeviceFlow &&
	   Console_Port_Tbl[minor].pDeviceFlow->deviceStopRemoteTx)
	{
		Console_Port_Tbl[minor].pDeviceFlow->deviceStopRemoteTx(minor);
	}

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

/* PAGE
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
  rtems_status_code          status;

  /*
   * initialize the termio interface.
   */
  rtems_termios_initialize();

  Console_Port_Count=NUM_CONSOLE_PORTS;

  for(minor=0;
    minor<Console_Port_Count;
    minor++)
  {
    /*
     * transfer the real internal bus frequency into the 
     * console port table
     */
    Console_Port_Tbl[minor].ulClock = BSP_bus_frequency;
    /*
     * First perform the configuration dependant probe, then the
     * device dependant probe
     */
    if((!Console_Port_Tbl[minor].deviceProbe ||
         Console_Port_Tbl[minor].deviceProbe(minor)) &&
         Console_Port_Tbl[minor].pDeviceFns->deviceProbe(minor))
    {
      /*
       * Use this device for the console
       */
      break;
    }
  }
  if(minor==Console_Port_Count)
  {
    /*
     * Failed to find a working device
     */
    rtems_fatal_error_occurred(RTEMS_IO_ERROR);
  }
	
  Console_Port_Minor=minor;

  /*
   *  Register Device Names
   */

  status = rtems_io_register_name("/dev/console",
           major,
           Console_Port_Minor );
  if (status != RTEMS_SUCCESSFUL)
  {
    rtems_fatal_error_occurred(status);
  }
  if ( Console_Port_Tbl[Console_Port_Minor].pDeviceFns->deviceInitialize ) {
    Console_Port_Tbl[Console_Port_Minor]
      .pDeviceFns->deviceInitialize(Console_Port_Minor);
    Console_Is_Initialized = true;
  }

  for(minor++;minor<Console_Port_Count;minor++)
  {
    /*
     * First perform the configuration dependant probe, then the
     * device dependant probe
     */
    if((!Console_Port_Tbl[minor].deviceProbe ||
         Console_Port_Tbl[minor].deviceProbe(minor)) &&
         Console_Port_Tbl[minor].pDeviceFns->deviceProbe(minor))
    {
      status = rtems_io_register_name(
        Console_Port_Tbl[minor].sDeviceName,
        major,
        minor );
      if (status != RTEMS_SUCCESSFUL)
      {
        rtems_fatal_error_occurred(status);
      }

      /*
       * Initialize the hardware device.
       */
      if ( Console_Port_Tbl[minor].pDeviceFns->deviceInitialize )
        Console_Port_Tbl[minor].pDeviceFns->deviceInitialize( minor);
    }
  }

  return RTEMS_SUCCESSFUL;
}

void debug_putc_onlcr(const char c)
{
  if (Console_Is_Initialized) {
    Console_Port_Tbl[Console_Port_Minor].pDeviceFns->
      deviceWritePolled(Console_Port_Minor,c);

    if (c == '\n') {
      Console_Port_Tbl[Console_Port_Minor].pDeviceFns->
        deviceWritePolled(Console_Port_Minor,'\r');
    }
  }
}

BSP_output_char_function_type   BSP_output_char = debug_putc_onlcr;
/* const char arg to be compatible with BSP_output_char decl. */

int bsp_uart_pollRead(int minor)
{
  return Console_Port_Tbl [0].pDeviceFns->deviceRead(0);
}
