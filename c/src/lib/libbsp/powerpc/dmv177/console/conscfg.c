/*
 *  This file contains the TTY driver table for the DY-4 DMV177.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994, 1997.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  $Id$
 */

#include <bsp.h>

#include <libchip/serial.h>
#include <libchip/mc68681.h>
#include <libchip/z85c30.h>
#include "mc68681cfg.h"
#include "z85c30cfg.h"

/*
 * Configuration specific probe routines
 *
 *
 * NOTE:  There are no DMV177 specific configuration routines.  These
 *        routines could be written to dynamically determine which serial
 *        ports are on a board.  Debugging this would require access to
 *        multiple board models.
 */

/* NONE CURRENTLY PROVIDED */

/*
 * The following table configures the console drivers used in this BSP.
 *
 * The first entry which, when probed, is available, will be named /dev/console,
 * all others being given the name indicated.
 */

console_tbl	Console_Port_Tbl[] = {
	{
		"/dev/com0",			/* sDeviceName */
		&mc68681_fns,			/* pDeviceFns */
		NULL,				/* deviceProbe */
		NULL,				/* pDeviceFlow */
		16,				/* ulMargin */
		8,				/* ulHysteresis */
		(void *)9600,	/* baud rate */	/* pDeviceParams */
		MC68681_ADDR,			/* ulCtrlPort1 */
		MC68681_PORT1_ADDR,		/* ulCtrlPort2 */
		0,				/* ulDataPort */
		Read_mc68681_register,		/* getRegister */
		Write_mc68681_register,		/* setRegister */
		NULL, /* unused */		/* getData */
		NULL, /* unused */		/* setData */
                0,    /* unused */              /* ulClock */
		DMV170_DUART_IRQ		/* ulIntVector */
	},
	{
		"/dev/com1",			/* sDeviceName */
		&mc68681_fns,			/* pDeviceFns */
		NULL,				/* deviceProbe */
		NULL,				/* pDeviceFlow */
		16,				/* ulMargin */
		8,				/* ulHysteresis */
		(void *)9600,	/* baud rate */	/* pDeviceParams */
		MC68681_ADDR,			/* ulCtrlPort1 */
		MC68681_PORT2_ADDR,		/* ulCtrlPort2 */
		0,				/* ulDataPort */
		Read_mc68681_register,		/* getRegister */
		Write_mc68681_register,		/* setRegister */
		NULL, /* unused */		/* getData */
		NULL, /* unused */		/* setData */
                0,    /* unused */              /* ulClock */
		DMV170_DUART_IRQ		/* ulIntVector */
	},
	{
		"/dev/com3",			/* sDeviceName */
		&z85c30_fns,			/* pDeviceFns */
		NULL,				/* deviceProbe */
		NULL,				/* pDeviceFlow */
		16,				/* ulMargin */
		8,				/* ulHysteresis */
		(void *)9600,	/* baud rate */	/* pDeviceParams */
		Z85C30_CTRL_A,			/* ulCtrlPort1 */
		Z85C30_CTRL_A,			/* ulCtrlPort2 */
		Z85C30_DATA_A,			/* ulDataPort */
		Read_z85c30_register,		/* getRegister */
		Write_z85c30_register,		/* setRegister */
		Read_z85c30_data,		/* getData */
		Write_z85c30_data,		/* setData */
                Z85C30_CLOCK,                   /* ulClock */
		DMV170_SCC_IRQ			/* ulIntVector */
	},
	{
		"/dev/com4",			/* sDeviceName */
		&z85c30_fns,			/* pDeviceFns */
		NULL,				/* deviceProbe */
		NULL,				/* pDeviceFlow */
		16,				/* ulMargin */
		8,				/* ulHysteresis */
		(void *)9600,	/* baud rate */	/* pDeviceParams */
		Z85C30_CTRL_B,			/* ulCtrlPort1 */
		Z85C30_CTRL_A,			/* ulCtrlPort2 */
		Z85C30_DATA_B,			/* ulDataPort */
		Read_z85c30_register,		/* getRegister */
		Write_z85c30_register,		/* setRegister */
		Read_z85c30_data,		/* getData */
		Write_z85c30_data,		/* setData */
                Z85C30_CLOCK,                   /* ulClock */
		DMV170_SCC_IRQ			/* ulIntVector */
	}
};

/*
 *  Declare some information used by the console driver
 */

#define NUM_CONSOLE_PORTS (sizeof(Console_Port_Tbl)/sizeof(console_tbl))

unsigned long  Console_Port_Count = NUM_CONSOLE_PORTS;

console_data  Console_Port_Data[NUM_CONSOLE_PORTS];

rtems_device_minor_number  Console_Port_Minor;
