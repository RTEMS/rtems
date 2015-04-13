/*  Console driver interface to UART drivers
 *
 *  - First console device that has System Console flag set will be
 *    system console.
 *  - If none of the registered console devices has system console set,
 *    the first is registered device is used, unless it has 
 *
 *  COPYRIGHT (c) 2010.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __CONS_H__
#define __CONS_H__

struct console_dev;

#define CONSOLE_FLAG_SYSCON      0x01

struct console_cons_ops {
	void (*get_uart_attrs)(struct console_dev *, struct termios *t);
};

struct console_dev {
	/* Set to non-zero if this UART should be system console and/or
	 * debug console.
	 */
	int flags;
	char *fsname; /* File system prefix */
	const struct rtems_termios_callbacks *callbacks; /* TERMIOS Callbacks */
	struct console_cons_ops ops;
};

extern void console_dev_register(struct console_dev *dev);
#if 0
extern void console_dev_unregister(struct console_dev *dev);
#endif

#endif
