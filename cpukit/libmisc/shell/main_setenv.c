/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Set an environment variable
 */

/*
 * COPYRIGHT (C) 2009 Chris Johns <chrisj@rtems.org>
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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

static int rtems_shell_main_setenv(int argc, char *argv[])
{
  char* env = NULL;
  char* string = NULL;
  int   len = 0;
  int   arg;
  char* p;

  if (argc <= 2) {
    printf("error: no variable or string\n");
    return 1;
  }

  env = argv[1];

  for (arg = 2; arg < argc; arg++)
    len += strlen(argv[arg]);

  len += argc - 2 - 1;

  string = malloc(len + 1);

  if (!string) {
    printf("error: no memory\n");
    return 1;
  }

  for (arg = 2, p = string; arg < argc; arg++) {
    strcpy(p, argv[arg]);
    p += strlen(argv[arg]);
    if (arg < (argc - 1)) {
      *p = ' ';
      p++;
    }
  }

  if (setenv(env, string, 1) < 0) {
    printf( "error: %s\n", strerror(errno) );
    free( string );
    return 1;
  }

  free( string );
  return 0;
}

rtems_shell_cmd_t rtems_shell_SETENV_Command = {
  .name = "setenv",
  .usage = "setenv [var] [string]",
  .topic = "misc",
  .command = rtems_shell_main_setenv,
  .alias = NULL,
  .next = NULL
};
