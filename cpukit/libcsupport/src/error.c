/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief
 */

/*
 * Copyright (C) 1995 Tony Bennett <tbennett@divnc.com>
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
#include <rtems/error.h>
#include <rtems/assoc.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/threadimpl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     /* _exit() */

int          rtems_panic_in_progress;

int rtems_verror(
  rtems_error_code_t  error_flag,
  const char         *printf_format,
  va_list             arglist
)
{
  int               local_errno = 0;
  int               chars_written = 0;
  rtems_status_code status;

  if (error_flag & RTEMS_ERROR_PANIC) {
    if (rtems_panic_in_progress++)
      _Thread_Dispatch_disable();       /* disable task switches */

    /* don't aggravate things */
    if (rtems_panic_in_progress > 2)
      return 0;
  }

  (void) fflush(stdout);            /* in case stdout/stderr same */

  status = error_flag & ~RTEMS_ERROR_MASK;
  if (error_flag & RTEMS_ERROR_ERRNO)     /* include errno? */
    local_errno = errno;

  #if defined(RTEMS_MULTIPROCESSING)
    if (_System_state_Is_multiprocessing)
      fprintf(stderr, "[%" PRIu16 "] ", rtems_object_get_local_node());
  #endif

  chars_written += vfprintf(stderr, printf_format, arglist);

  if (status)
    chars_written +=
      fprintf(stderr, " (status: %s)", rtems_status_text(status));

  if (local_errno) {
    if ((local_errno > 0) && *strerror(local_errno))
      chars_written += fprintf(stderr, " (errno: %s)", strerror(local_errno));
    else
      chars_written += fprintf(stderr, " (unknown errno=%d)", local_errno);
  }

  chars_written += fprintf(stderr, "\n");

  (void) fflush(stderr);

  return chars_written;
}

int rtems_error(
  rtems_error_code_t error_flag,
  const char *printf_format,
  ...
)
{
  va_list arglist;
  int chars_written;

  va_start(arglist, printf_format);
  chars_written = rtems_verror(error_flag, printf_format, arglist);
  va_end(arglist);

  if (error_flag & RTEMS_ERROR_PANIC) {
    rtems_error(0, "fatal error, exiting");
    _exit(errno);
  }
  if (error_flag & RTEMS_ERROR_ABORT) {
    rtems_error(0, "fatal error, aborting");
    abort();
  }

  return chars_written;
}
