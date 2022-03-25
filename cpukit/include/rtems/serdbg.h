/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS remote gdb over serial line
 *
 * This file declares intialization functions to add
 * a gdb remote debug stub to an RTEMS system.
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

#ifndef _SERDBG_H
#define _SERDBG_H

#include <rtems.h>
#include <termios.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint32_t   baudrate;      /* debug baud rate, e.g. 57600            */
  void (*callout)(void);    /* callout pointer during polling         */
  int  (*open_io)(const char *dev_name, uint32_t baudrate); /* I/O open fnc */
  const char *devname;      /* debug device, e.g. "/dev/tty01"        */
  bool      skip_init_bkpt; /* if TRUE, do not stop when initializing */
} serdbg_conf_t;

/*
 * must be defined in init module...
 */
extern serdbg_conf_t serdbg_conf;


/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void putDebugChar
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   send character to remote debugger                                       |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 char        c                          /* char to send                    */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int getDebugChar
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   get character from remote debugger                                      |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 void                                   /* <none>                          */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void serdbg_exceptionHandler
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   hook directly to an exception vector                                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int vecnum,                            /* vector index to hook at         */
 void *vector                           /* address of handler function     */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int serdbg_init
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   initialize remote gdb session over serial line                          |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 void
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/

/*
 * stuff from serdbgio.c
 */
/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int serdbg_open

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


extern int serdbg_init_dbg(void);

/*
 * Assumed to be provided by the BSP
 */
extern void set_debug_traps(void);
extern void breakpoint(void);
#ifdef __cplusplus
}
#endif

#endif /* _SERDBG_H */
