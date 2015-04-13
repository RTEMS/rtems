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
#include <rtems/libio.h>
#include <rtems/bspIo.h>
#include <bsp/cons.h>

#ifdef RTEMS_DRVMGR_STARTUP

/* Note that it is not possible to use the interrupt mode of the driver
 * together with the "old" APBUART and -u to GRMON. However the new
 * APBUART core (from 1.0.17-b2710) has the GRMON debug bit and can 
 * handle interrupts.
 */

int console_initialized = 0;
rtems_device_major_number console_major = 0;

#define FLAG_SYSCON 0x01
struct console_priv {
	unsigned char flags; /* 0x1=SystemConsole */
	unsigned char minor;
	struct console_dev *dev;
};

#define CONSOLE_MAX BSP_NUMBER_OF_TERMIOS_PORTS
struct console_priv cons[CONSOLE_MAX] = {{0,0},};

/* Register Console to TERMIOS layer and initialize it */
static void console_dev_init(struct console_priv *con, int minor)
{
	char name[16], *fsname;
	rtems_status_code status;

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
	status = rtems_io_register_name(fsname, console_major, minor);
	if ((minor == 0) && (status != RTEMS_SUCCESSFUL))
		rtems_fatal_error_occurred(status);
}

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

	/* Assign Console */
	con->dev = dev;
	con->minor = minor;

	/* Console layer is already initialized, that means that we can
	 * register termios interface directly.
	 */
	if (console_initialized)
		console_dev_init(con, minor);
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

	console_major = major;

	rtems_termios_initialize();

	/* Register all Console a file system device node */
	for (i=0; i<CONSOLE_MAX; i++) {
		if (cons[i].dev)
			console_dev_init(&cons[i], i);
	}

	console_initialized = 1;

	return RTEMS_SUCCESSFUL;
}

rtems_device_driver console_open(
	rtems_device_major_number	major,
	rtems_device_minor_number	minor,
	void				*arg)
{
	rtems_status_code status;
	struct termios term;

	if ((minor >= CONSOLE_MAX) || !cons[minor].dev)
		return RTEMS_INVALID_NUMBER;

	status = rtems_termios_open(
			major,
			(int)cons[minor].dev,
			arg,
			cons[minor].dev->callbacks);

	/* Inherit UART hardware parameters from bootloader on system console */
	if ((status == RTEMS_SUCCESSFUL) && (cons[minor].flags & FLAG_SYSCON) &&
	    (cons[minor].dev->ops.get_uart_attrs != NULL)) {
		if (tcgetattr(STDIN_FILENO, &term) >= 0) {
			cons[minor].dev->ops.get_uart_attrs(cons[minor].dev,
								&term);
			term.c_oflag |= ONLCR;
			tcsetattr(STDIN_FILENO, TCSANOW, &term);
		}
	}

	return status;
}

rtems_device_driver console_close(
	rtems_device_major_number	major,
	rtems_device_minor_number	minor,
	void				*arg)
{
	return rtems_termios_close(arg);
}

rtems_device_driver console_read(
	rtems_device_major_number	major,
	rtems_device_minor_number	minor,
	void				*arg)
{
	return rtems_termios_read(arg);
}

rtems_device_driver console_write(
	rtems_device_major_number	major,
	rtems_device_minor_number	minor,
	void				*arg)
{
	return rtems_termios_write(arg);
}

rtems_device_driver console_control(
	rtems_device_major_number	major,
	rtems_device_minor_number	minor,
	void				*arg)
{
	return rtems_termios_ioctl(arg);
}

#endif
