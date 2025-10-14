/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * 
 * @brief RM Shell Command Implementation
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
#include <fcntl.h>

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/bdbuf.h>
#include <rtems/blkdev.h>
#include "internal.h"

static int rtems_shell_main_blksync(
  int argc,
  char *argv[]
)
{
  const char* driver = NULL;
  int         arg;
  int         fd;

  for (arg = 1; arg < argc; arg++) {
    if (argv[arg][0] == '-') {
      fprintf( stderr, "%s: invalid option: %s\n", argv[0], argv[arg]);
      return 1;
    } else {
      if (!driver)
        driver = argv[arg];
      else {
        fprintf( stderr, "%s: only one driver name allowed: %s\n",
          argv[0], argv[arg]);
        return 1;
      }
    }
  }

  fd = open (driver, O_WRONLY, 0);
  if (fd < 0) {
    fprintf( stderr, "%s: driver open failed: %s\n", argv[0], strerror (errno));
    return 1;
  }

  if (rtems_disk_fd_sync (fd) < 0) {
    fprintf( stderr, "%s: driver sync failed: %s\n", argv[0], strerror (errno));
    close (fd);
    return 1;
  }

  close (fd);
  return 0;
}

rtems_shell_cmd_t rtems_shell_BLKSYNC_Command = {
  .name = "blksync",
  .usage = "blksync driver # sync the block driver",
  .topic = "files",
  .command = rtems_shell_main_blksync,
  .alias = NULL,
  .next = NULL
};
