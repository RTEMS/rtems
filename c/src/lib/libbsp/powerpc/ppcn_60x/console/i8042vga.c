/*
 *  This file contains the TTY driver for the VGA/i8042 console
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
 *  This driver uses the termios pseudo driver.
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/libio.h>

#include "console.h"
#include "vga_p.h"
#include "i8042_p.h"

console_fns i8042vga_fns =
{
	i8042_probe,			/* deviceProbe */
	NULL,				/* deviceFirstOpen */
	NULL,				/* deviceLastClose */
#if CONSOLE_USE_INTERRUPTS
	NULL,				/* deviceRead */
	vga_write_support,		/* deviceWrite */
	i8042_initialize_interrupts,	/* deviceInitialize */
#else
	i8042_inbyte_nonblocking_polled,	/* deviceRead */
	vga_write_support,		/* deviceWrite */
	i8042_init,			/* deviceInitialize */
#endif
	vga_write,			/* deviceWritePolled */
	FALSE,				/* deviceOutputUsesInterrupts */
};
