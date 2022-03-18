/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS remote gdb over serial line
 *
 * This file contains intialization and utility functions to add
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <rtems/serdbg.h>


/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int serdbg_init_dbg
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   initialize remote gdb session over serial line                          |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 void
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{
  static bool is_initialized = false;

  rtems_status_code rc = RTEMS_SUCCESSFUL;

  if (is_initialized) {
    return RTEMS_SUCCESSFUL;
  }
  is_initialized = true;
  /*
   * try to open serial device
   */
  if (rc == RTEMS_SUCCESSFUL) {
    if ((serdbg_conf.open_io != NULL) &&
	(0 > serdbg_conf.open_io(serdbg_conf.devname,serdbg_conf.baudrate))) {
      fprintf(stderr,
	      "remote_gdb_init: cannot open device %s "
	      "for gdb connection:%s\n",serdbg_conf.devname,strerror(errno));
      rc = RTEMS_IO_ERROR;
    }
  }
  /*
   * initialize gdb stub
   */
  if (rc == RTEMS_SUCCESSFUL) {
    set_debug_traps();
  }
  /*
   * now activate gdb stub
   */
  if ((rc == RTEMS_SUCCESSFUL) &&
      !serdbg_conf.skip_init_bkpt) {
    breakpoint();
  }

  /*
   * return to original function
   * this may be already unter gdb control
   */
  return rc;
}
