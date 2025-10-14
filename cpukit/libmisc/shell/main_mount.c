/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * 
 * @brief Shell Command Implementation
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
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/shellconfig.h>
#include <rtems/libio.h>
#include "internal.h"

static bool print_filesystem(const rtems_filesystem_table_t *entry, void *arg)
{
  (void) arg;

  printf("%s ", entry->type);

  return false;
}

static int rtems_shell_main_mount(
  int   argc,
  char *argv[]
)
{
  rtems_filesystem_options_t options = RTEMS_FILESYSTEM_READ_WRITE;
  char*                      type = NULL;
  char*                      source = NULL;
  char*                      target = NULL;
  char*                      fsoptions = NULL;
  int                        arg;

  for (arg = 1; arg < argc; arg++) {
    if (argv[arg][0] == '-') {
      if (argv[arg][1] == 't') {
        arg++;
        if (arg == argc) {
          fprintf(
            stderr,
            "%s: -t needs a type of file-system; see -L.\n",
            argv[0]
          );
          return 1;
        }
        type = argv[arg];
      } else if (argv[arg][1] == 'r') {
        options = RTEMS_FILESYSTEM_READ_ONLY;
      } else if (argv[arg][1] == 'L') {
        printf ("File systems: ");
        rtems_filesystem_iterate(print_filesystem, NULL);
        printf ("\n");
        return 0;
      } else if (argv[arg][1] == 'o') {
        arg++;
        if (arg == argc) {
          fprintf(
            stderr,
            "%s: -o needs a list of filesystem options.\n",
            argv[0]
          );
          return 1;
        }
        fsoptions = argv[arg];
      } else {
        fprintf (stderr, "unknown option: %s\n", argv[arg]);
        return 1;
      }
    } else {
      if (!source)
        source = argv[arg];
      else if (!target)
        target = argv[arg];
      else {
        fprintf (
          stderr, "mount: source and mount only require: %s\n", argv[arg]);
        return 1;
      }
    }
  }

  if (!type) {
    fprintf (stderr, "mount: no file-system; see the -L option\n");
    return 1;
  }

  if (!source) {
    fprintf (stderr, "mount: no source\n");
    return 1;
  }

  if (!target) {
    fprintf (stderr, "mount: no mount point\n");
    return 1;
  }

  /*
   * Mount the disk.
   */

  if (mount (source, target, type, options, fsoptions) < 0) {
    fprintf (stderr, "error: %s\n", strerror(errno));
    return 1;
  }

  printf ("mounted %s -> %s\n", source, target);

  return 0;
}

rtems_shell_cmd_t rtems_shell_MOUNT_Command = {
  .name = "mount",
  .usage = "mount [-t type] [-r] [-L] [-o options] source target",
  .topic = "files",
  .command = rtems_shell_main_mount,
  .alias = NULL,
  .next = NULL
};
