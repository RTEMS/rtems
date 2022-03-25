/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Adds printk Support via Polled termios
 */

/*
 * Copyright (c) 2002 IMD Ingenieurbuero fuer Microcomputertechnik
 * All rights reserved.
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

#ifndef _TERMIOS_PRINTK_CNF_H
#define _TERMIOS_PRINTK_CNF_H

#include <rtems/termios_printk.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIGURE_INIT

/*
 * fallback for baud rate to use
 */
#ifndef CONFIGURE_TERMIOS_PRINTK_BAUDRATE
#define CONFIGURE_TERMIOS_PRINTK_BAUDRATE 9600
#endif

/*
 * fallback for device name to use
 */
#ifndef CONFIGURE_TERMIOS_PRINTK_DEVNAME
#define CONFIGURE_TERMIOS_PRINTK_DEVNAME "/dev/console"
#endif

#ifdef CONFIGURE_USE_TERMIOS_PRINTK
/*
 * fill in termios_printk_conf structure
 */
termios_printk_conf_t termios_printk_conf = {
  CONFIGURE_TERMIOS_PRINTK_BAUDRATE,

#ifdef CONFIGURE_TERMIOS_PRINTK_CALLOUT
  CONFIGURE_TERMIOS_PRINTK_CALLOUT,
#else
  NULL,
#endif
  CONFIGURE_TERMIOS_PRINTK_DEVNAME,
};
#endif

int termios_printk_init(void) {
#ifdef CONFIGURE_USE_TERMIOS_PRINTK
  return termios_printk_open(termios_printk_conf.devname,
			     termios_printk_conf.baudrate);
#else
  return 0;
#endif
}

#endif /* CONFIGURE_INIT */

#ifdef __cplusplus
}
#endif

#endif /* _TERMIOS_PRINTK_CNF_H */
