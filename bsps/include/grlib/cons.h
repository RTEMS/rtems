/* SPDX-License-Identifier: BSD-2-Clause */

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
