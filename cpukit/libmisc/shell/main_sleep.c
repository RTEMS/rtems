/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief sleep Shell Command Implementation
 */

/*
 * COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
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

#include <stdio.h>
#include <time.h>

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/stringto.h>
#include "internal.h"

static int rtems_shell_main_sleep(
  int   argc,
  char *argv[]
)
{
  struct timespec delay;
  unsigned long   tmp;

  if ((argc != 2) && (argc != 3)) {
    fprintf( stderr, "%s: Usage seconds [nanoseconds]\n", argv[0] );
    return -1;
  }

  /*
   *  Convert the seconds argument to a number
   */
  if ( rtems_string_to_unsigned_long(argv[1], &tmp, NULL, 0) ) {
    printf( "Seconds argument (%s) is not a number\n", argv[1] );
    return -1;
  }
  delay.tv_sec = (time_t) tmp;

  /*
   *  If the user specified a nanoseconds argument, convert it
   */
  delay.tv_nsec = 0;
  if (argc == 3) {
    if ( rtems_string_to_unsigned_long(argv[2], &tmp, NULL, 0) ) {
      printf( "Seconds argument (%s) is not a number\n", argv[1] );
      return -1;
    }
    delay.tv_nsec = tmp;
  }

  /*
   *  Now sleep as requested.
   */
  nanosleep( &delay, NULL );
  return 0;
}

rtems_shell_cmd_t rtems_shell_SLEEP_Command = {
  .name = "sleep",
  .usage = "sleep seconds [nanoseconds]",
  .topic = "misc",
  .command = rtems_shell_main_sleep,
  .alias = NULL,
  .next = NULL
};
