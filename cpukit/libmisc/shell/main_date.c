/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * 
 * @brief DATE Shell Command Implementation
 */

/*
 * Copyright (C) 2001 Fernando Ruiz Casas <fruizcasas@gmail.com>
 * Copyright (C) 2008 Joel Sherrill <joel.sherrill@oarcorp.com>.
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

#define _XOPEN_SOURCE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

static int rtems_shell_main_date(
  int   argc,
  char *argv[]
)
{
  /*
   *  Print the current date and time in default format.
   */
  if ( argc == 1 ) {
    time_t t;

    time(&t);
    printf("%s", ctime(&t));
    return 0;
  }

  /*
   *  Set the current date and time
   */
  if ( argc == 3 ) {
    char buf[128];
    struct tm TOD;
    struct timespec timesp;
    char *result;

    snprintf( buf, sizeof(buf), "%s %s", argv[1], argv[2] );
    result = strptime(
      buf,
      "%Y-%m-%d %T",
      &TOD
    );
    if ( result && !*result ) {
      timesp.tv_sec = mktime( &TOD );
      timesp.tv_nsec = 0;
      clock_settime( CLOCK_REALTIME, &timesp );
      return 0;
    }
  }

  fprintf( stderr, "%s: Usage: [YYYY-MM-DD HH:MM:SS]\n", argv[0] );
  return -1;
}

rtems_shell_cmd_t rtems_shell_DATE_Command = {
  .name = "date",
  .usage = "date [YYYY-MM-DD HH:MM:SS]",
  .topic = "misc",
  .command = rtems_shell_main_date,
  .alias = NULL,
  .next = NULL
};
