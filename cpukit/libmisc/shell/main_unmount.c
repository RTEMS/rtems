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
#include <rtems/fsmount.h>
#include "internal.h"

static int rtems_shell_main_unmount(
  int   argc,
  char *argv[]
)
{
  char* mount_point = NULL;
  int   arg;

  for (arg = 1; arg < argc; arg++) {
    if (!mount_point)
      mount_point = argv[arg];
    else {
      fprintf (stderr, "error: only one mount path require: %s\n", argv[arg]);
      return 1;
    }
  }

  if (!mount_point) {
    fprintf (stderr, "error: no mount point\n");
    return 1;
  }

  /*
   * Unmount the disk.
   */

  if (unmount (mount_point) < 0) {
    fprintf (stderr, "error: unmount failed: %s: %s\n",
             mount_point, strerror (errno));
    return 1;
  }

  printf ("unmounted %s\n", mount_point);

  return 0;
}

rtems_shell_cmd_t rtems_shell_UNMOUNT_Command = {
  .name = "unmount",
  .usage = "unmount path # unmount disk",
  .topic = "files",
  .command = rtems_shell_main_unmount,
  .alias = NULL,
  .next = NULL
};
