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

#include <rtems/termiostypes.h>

struct console_dev;

#define CONSOLE_FLAG_SYSCON             0x01
#define CONSOLE_FLAG_SYSCON_GRANT       0x02

struct console_dev {
	rtems_termios_device_context base;
	/* Set CONSOLE_FLAG_SYSCON to request this device to be system console
	 * and/or debug console. CONSOLE_FLAG_SYSCON_GRANT will be set on the
	 * device which was selected as system console.
	 */
	int flags;
	char *fsname; /* File system prefix */
	const rtems_termios_device_handler *handler;
};

extern void console_dev_register(struct console_dev *dev);
#if 0
extern void console_dev_unregister(struct console_dev *dev);
#endif

#endif
