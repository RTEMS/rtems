/*
 *  This file contains the TTY driver for the PPCn_60x
 *
 *  This driver uses the termios pseudo driver.
 *
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *
 * THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 * AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 * You are hereby granted permission to use, copy, modify, and distribute
 * this file, provided that this notice, plus the above copyright notice
 * and disclaimer, appears in all copies. Radstone Technology will provide
 * no support for this code.
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

#include "console.h"

/*
 * Load configuration table
 */
#include "config.c"

#define NUM_CONSOLE_PORTS (sizeof(Console_Port_Tbl)/sizeof(console_tbl))

console_data	Console_Port_Data[NUM_CONSOLE_PORTS];
unsigned long	Console_Port_Count;
rtems_device_minor_number  Console_Port_Minor;
		
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
	rtems_libio_ioctl_args_t IoctlArgs;
	struct termios	Termios;
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

	/*
	 * Patch in flow control routines
	 */
/* XXX */
#if 0 
	if((status==RTEMS_SUCCESSFUL) &&
	   (Console_Port_Tbl[minor].pDeviceFlow))
	{
		status=rtems_termios_flow_control(
			major, minor, arg,
			Console_Port_Tbl[minor].pDeviceFlow->
				deviceStartRemoteTx,
			Console_Port_Tbl[minor].pDeviceFlow->deviceStopRemoteTx,
			Console_Port_Tbl[minor].ulMargin,
			Console_Port_Tbl[minor].ulHysteresis);
	}
#endif

	if(minor!=Console_Port_Minor)
	{
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

	if((args->iop->flags&LIBIO_FLAGS_READ) &&
	   Console_Port_Tbl[minor].pDeviceFlow &&
	   Console_Port_Tbl[minor].pDeviceFlow->deviceStartRemoteTx)
	{
		Console_Port_Tbl[minor].pDeviceFlow->deviceStartRemoteTx(minor);
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
	Console_Port_Tbl[minor].pDeviceFns->deviceInitialize(
		Console_Port_Minor);

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
			Console_Port_Tbl[minor].pDeviceFns->deviceInitialize(
				minor);

		}
	}

	return RTEMS_SUCCESSFUL;
}

/* PAGE
 *
 *  DEBUG_puts
 *
 *  This should be safe in the event of an error.  It attempts to ensure
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
	unsigned32	Irql;

	rtems_interrupt_disable(Irql);

	for ( s = string ; *s ; s++ ) 
	{
		Console_Port_Tbl[Console_Port_Minor].pDeviceFns->
			deviceWritePolled(Console_Port_Minor, *s);
	}

	rtems_interrupt_enable(Irql);
}

/* PAGE
 *
 *  DEBUG_puth
 *
 *  This should be safe in the event of an error.  It attempts to ensure
 *  that no TX empty interrupts occur while it is doing polled IO.  Then
 *  it restores the state of that external interrupt.
 *
 *  Input parameters:
 *    ulHexNum - value to display
 *
 *  Output parameters:  NONE
 *
 *  Return values:      NONE
 */
void
DEBUG_puth(
    unsigned32 ulHexNum
    )
{
	unsigned long i,d;
	unsigned32 Irql;

	rtems_interrupt_disable(Irql);
	
	Console_Port_Tbl[Console_Port_Minor].pDeviceFns->
		deviceWritePolled(Console_Port_Minor, '0');
	Console_Port_Tbl[Console_Port_Minor].pDeviceFns->
		deviceWritePolled(Console_Port_Minor, 'x');

	for(i=32;i;)
	{
		i-=4;
		d=(ulHexNum>>i)&0xf;
		Console_Port_Tbl[Console_Port_Minor].pDeviceFns->
			deviceWritePolled(Console_Port_Minor,
					  (d<=9) ? d+'0' : d+'a'-0xa);
	}

	rtems_interrupt_enable(Irql);
}

