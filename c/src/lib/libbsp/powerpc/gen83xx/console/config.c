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
| this file contains the console driver configuration tables      |
\*===============================================================*/
/* derived from: */
/*
 *  This file contains the TTY driver table for the EP1A
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

#include <libchip/serial.h>
#include "ns16550cfg.h"
#include <bsp.h>
#include <libcpu/io.h>
#include <mpc83xx/mpc83xx.h>

/*
 *  Based on BSP configuration information decide whether to do polling IO
 *  or interrupt driven IO.
 */

#define NS16550_FUNCTIONS &ns16550_fns_polled

/*
 * The following table configures the console drivers used in this BSP.
 *
 * The first entry which, when probed, is available, will be named /dev/console,
 * all others being given the name indicated.
 *
 * Each field is interpreted thus:
 *
 * sDeviceName	This is the name of the device.
 * pDeviceFns	This is a pointer to the set of driver routines to use.
 * pDeviceFlow	This is a pointer to the set of flow control routines to
 *		use. Serial device drivers will typically supply RTSCTS
 *		and DTRCTS handshake routines for DCE to DCE communication,
 *		however for DCE to DTE communication, no such routines
 *		should be necessary as RTS will be driven automatically
 *		when the transmitter is active.
 * ulMargin	The high water mark in the input buffer is set to the buffer
 *		size less ulMargin. Once this level is reached, the driver's
 *		flow control routine used to stop the remote transmitter will
 *		be called. This figure should be greater than or equal to
 *		the number of stages of FIFO between the transmitter and
 *		receiver.
 * ulHysteresis	After the high water mark specified by ulMargin has been
 *		reached, the driver's routine to re-start the remote
 *		transmitter will be called once the level in the input
 *		buffer has fallen by ulHysteresis bytes.
 * pDeviceParams This contains either device specific data or a pointer to a
 *		device specific structure containing additional information
 *		not provided in this table.
 * ulCtrlPort1	This is the primary control port number for the device. This
 *		may be used to specify different instances of the same device
 *		type.
 * ulCtrlPort2	This is the secondary control port number, of use when a given
 *		device has more than one available channel.
 * ulDataPort	This is the port number for the data port of the device
 * ulIntVector	This encodes the interrupt vector of the device.
 *
 */
console_tbl	Console_Port_Tbl[] = {
        /*
         *  NS16550 Chips provide first ttyS0/1 Ports.
         */
	{
		"/dev/ttyS0",			/* sDeviceName */
		SERIAL_NS16550,			/* deviceType */
		NS16550_FUNCTIONS,		/* pDeviceFns */
		NULL,				/* deviceProbe */
		&ns16550_flow_RTSCTS,		/* pDeviceFlow */
		16,				/* ulMargin */
		8,				/* ulHysteresis */
		(void *)9600,	/* baud rate */	/* pDeviceParams */
                (uint32_t)&(mpc83xx.duart[0]),  /* ulCtrlPort1e */
		0,				/* ulCtrlPort2 */
                (uint32_t)&(mpc83xx.duart[0]),  /* ulDataPort */
		Read_ns16550_register,		/* getRegister */
		Write_ns16550_register,		/* setRegister */
		NULL,				/* getData */
		NULL,				/* setData */
		0,		                /* ulClock (filled in init) */
                0				/* ulIntVector */
	}
#if BSP_USE_UART2
        ,{
                "/dev/ttyS1",                   /* sDeviceName */
                SERIAL_NS16550,                 /* deviceType */
                NS16550_FUNCTIONS,              /* pDeviceFns */
                NULL,                           /* deviceProbe */
                &ns16550_flow_RTSCTS,           /* pDeviceFlow */
                16,                             /* ulMargin */
                8,                              /* ulHysteresis */
                (void *)9600,   /* baud rate */ /* pDeviceParams */
                (uint32_t)&(mpc83xx.duart[1]),  /* ulCtrlPort1-Filled in at runtime */
                0,                              /* ulCtrlPort2 */
                (uint32_t)&(mpc83xx.duart[1]),  /* ulDataPort-Filled in at runtime*/
                Read_ns16550_register,          /* getRegister */
                Write_ns16550_register,         /* setRegister */
                NULL,                           /* getData */
                NULL,                           /* setData */
		0,		                /* ulClock (filled in init) */
                0                               /* ulIntVector */
        }
#endif
};

