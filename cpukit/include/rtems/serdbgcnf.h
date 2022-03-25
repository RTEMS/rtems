/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Adds a GDB remote Debug Stub to an RTEMS System
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

#ifndef _SERDBGCNF_H
#define _SERDBGCNF_H

#include <rtems/serdbg.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIGURE_INIT

/*
 * fallback for baud rate to use
 */
#ifndef CONFIGURE_SERDBG_BAUDRATE
#define CONFIGURE_SERDBG_BAUDRATE 9600
#endif

/*
 * fallback for device name to use
 */
#ifndef CONFIGURE_SERDBG_DEVNAME
#define CONFIGURE_SERDBG_DEVNAME "/dev/tty01"
#endif

/*
 * fill in serdbg_conf structure
 */
serdbg_conf_t serdbg_conf = {
  CONFIGURE_SERDBG_BAUDRATE,

#ifdef CONFIGURE_SERDBG_CALLOUT
  CONFIGURE_SERDBG_CALLOUT,
#else
  NULL,
#endif

#ifdef CONFIGURE_SERDBG_USE_POLLED_TERMIOS
  serdbg_open,
#else
  NULL,
#endif

  CONFIGURE_SERDBG_DEVNAME,

#ifdef CONFIGURE_SERDBG_SKIP_INIT_BKPT
  true,
#else
  false,
#endif
};

int serdbg_init(void) {
#ifdef CONFIGURE_USE_SERDBG
  return serdbg_init_dbg();
#else
  return 0;
#endif
}

#endif /* CONFIGURE_INIT */

#ifdef __cplusplus
}
#endif

#endif /* _SERDBGCNF_H */
