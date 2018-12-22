/*  This file contains the TTY driver for the serial ports. The driver
 *  is layered so that different UART hardware can be used. It is implemented
 *  using the Driver Manager.
 *
 *  This driver uses the termios pseudo driver.
 *
 *  COPYRIGHT (c) 2010.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <stdlib.h>
#include <string.h>
#include <grlib/cons.h>
#include <rtems/console.h>

#ifdef RTEMS_DRVMGR_STARTUP

/* Note that it is not possible to use the interrupt mode of the driver
 * together with the "old" APBUART and -u to GRMON. However the new
 * APBUART core (from 1.0.17-b2710) has the GRMON debug bit and can 
 * handle interrupts.
 */

static int console_initialized = 0;

#define FLAG_SYSCON 0x01
struct console_priv {
	int flags; /* 0x1=SystemConsole */
	int minor;
	struct console_dev *dev;
};

#define CONSOLE_MAX BSP_NUMBER_OF_TERMIOS_PORTS
struct console_priv cons[CONSOLE_MAX] = {{0,0},};

/* Install Console in TERMIOS layer */
static void console_dev_init(struct console_priv *con)
{
	char name[16], *fsname;
	rtems_status_code status;
	int minor;

	minor = con->minor;
	if (!con->dev->fsname) {
		strcpy(name, "/dev/console_a");
		/* Special console name and MINOR for SYSTEM CONSOLE */
		if (minor == 0)
			name[12] = '\0'; /* /dev/console */
		name[13] += minor; /* when minor=0, this has no effect... */
		fsname = name;
	} else {
		fsname = con->dev->fsname;
	}
	status = rtems_termios_device_install(
		fsname,
		con->dev->handler,
		NULL,
		&con->dev->base
	);
	if (status != RTEMS_SUCCESSFUL) {
		rtems_fatal_error_occurred(status);
	}
}

/* Called by device driver to register itself to the cons interface. */
void console_dev_register(struct console_dev *dev)
{
	int i, minor = 0;
	struct console_priv *con = NULL;

	if ((dev->flags & CONSOLE_FLAG_SYSCON) && !cons[0].dev) {
		con = &cons[0];
		con->flags = FLAG_SYSCON;
	} else {
		for (i=1; i<CONSOLE_MAX; i++) {
			if (!cons[i].dev) {
				con = &cons[i];
				con->flags = 0;
				minor = i;
				break;
			}
		}
	}
	if (con == NULL) {
		/* Not enough console structures */
		return;
	}
	dev->flags &= ~CONSOLE_FLAG_SYSCON_GRANT;
	if (con->flags & FLAG_SYSCON) {
		dev->flags |= CONSOLE_FLAG_SYSCON_GRANT;
	}

	/* Assign Console */
	con->dev = dev;
	con->minor = minor;

	if (console_initialized) {
		/* Console layer is already initialized, that means that we can
		 * register termios interface directly.
		 */
		console_dev_init(con);
	}
}

#if 0
void console_dev_unregister(struct console_dev *dev)
{

}
#endif

rtems_device_driver console_initialize(
	rtems_device_major_number	major,
	rtems_device_minor_number	minor,
	void				*arg)
{
	int i;

	rtems_termios_initialize();

	/* Register all Console a file system device node */
	for (i=0; i<CONSOLE_MAX; i++) {
		if (cons[i].dev)
			console_dev_init(&cons[i]);
	}

	console_initialized = 1;

	return RTEMS_SUCCESSFUL;
}

#endif
