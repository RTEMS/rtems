/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * 
 * @brief CAT Shell Command Implementation
 */

/*
 * Copyright (C) 2001 Fernando Ruiz Casas <fruizcasas@gmail.com>
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
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <sys/types.h>
#include <stddef.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

static int rtems_shell_main_cat(int argc, char *argv[])
{
  int n;
  int sc;

  for ( n=1; n < argc ; n++) {
    sc = rtems_shell_cat_file(stdout, argv[n]);
    if ( sc == -1 ) {
      fprintf(stderr, "%s: %s: %s\n", argv[0], argv[n], strerror(errno));
      return -1;
    }
  }
  return 0;
}

rtems_shell_cmd_t rtems_shell_CAT_Command = {
  .name = "cat",
  .usage = "cat n1 [n2 [n3...]] # show the ascii contents",
  .topic = "files",
  .command = rtems_shell_main_cat ,
  .alias = NULL,
  .next = NULL
};
