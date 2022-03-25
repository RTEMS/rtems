/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file declares intialization functions to add
 * printk polled output via termios polled drivers.
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

#ifndef _TERMIOS_PRINTK_H
#define _TERMIOS_PRINTK_H

#include <rtems.h>
#include <termios.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint32_t   baudrate;      /* debug baud rate, e.g. 57600            */
  void (*callout)(void);    /* callout pointer during polling         */
  const char *devname;      /* debug device, e.g. "/dev/tty01"        */
} termios_printk_conf_t;

/*
 * must be defined in init module...
 */
extern termios_printk_conf_t termios_printk_conf;

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void termios_printk_outputchar
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|    send one character to serial port                                      |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
(
 char c  /* character to print */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int termios_printk_inputchar
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|    wait for one character from serial port                                |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
(
 void  /* none */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    received character                                                     |
\*=========================================================================*/


/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int termios_printk_open

/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|    try to open given serial debug port                                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
(
 const char *dev_name, /* name of device to open */
 uint32_t   baudrate   /* baud rate to use       */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    0 on success, -1 and errno otherwise                                   |
\*=========================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _TERMIOS_PRINTK_H */
