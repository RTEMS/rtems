/*
 *  Console declarations
 *
 *
 *  MPC5xx port sponsored by Defence Research and Development Canada - Suffield
 *  Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _MPC5XX_CONSOLE_H
#define _MPC5XX_CONSOLE_H

#include <rtems/libio.h>
#include <rtems/termiostypes.h>

void m5xx_uart_initialize(int minor);

/* Termios callbacks */
int m5xx_uart_firstOpen(int maj, int min, void *arg);
int m5xx_uart_lastClose(int maj, int min, void *arg);
int m5xx_uart_pollRead(int minor);
ssize_t m5xx_uart_pollWrite(int minor, const char* buf, size_t len);
ssize_t m5xx_uart_write    (int minor, const char *buf, size_t len);
int m5xx_uart_setAttributes(int, const struct termios* t);

#define NUM_PORTS	2	/* number of serial ports */

#define SCI1_MINOR	0
#define SCI2_MINOR	1

#endif /* _MPC5XX_CONSOLE_H */
