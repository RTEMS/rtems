/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief open_dev_console - open /dev/console
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
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
#include <rtems/libio.h>
#include <rtems/console.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

/*
 *  This is a replaceable stub which opens the console, if present.
 */
void rtems_libio_post_driver(void)
{
  int fd = 0;
  /*
   * Attempt to open /dev/console.
   */
  if ( ( fd = open( CONSOLE_DEVICE_NAME, O_RDONLY, 0 ) ) != STDIN_FILENO ) {
    /*
     * There may not be a console driver so this is OK.
     */
    if ( fd < 0 ) {
      return;
    }

    /*
     * If open succeeds, but doesn't give us the stdin fileno we expect, bail out...
     */
    _Internal_error( INTERNAL_ERROR_LIBIO_STDIN_FD_OPEN_FAILED );
  }

  /*
   *  But if we find /dev/console once, we better find it twice more
   *  or something is REALLY wrong.
   */
  if ( open( CONSOLE_DEVICE_NAME, O_WRONLY, 0 ) != STDOUT_FILENO ) {
    _Internal_error( INTERNAL_ERROR_LIBIO_STDOUT_FD_OPEN_FAILED );
  }

  if ( open( CONSOLE_DEVICE_NAME, O_WRONLY, 0 ) != STDERR_FILENO ) {
    _Internal_error( INTERNAL_ERROR_LIBIO_STDERR_FD_OPEN_FAILED );
  }

  atexit(rtems_libio_exit);
}

