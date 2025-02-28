/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief chmod Shell Command Implementation
 */

/*
 * Copyright (C) 2014 embedded brains GmbH & Co. KG
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

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <rtems/shellconfig.h>
#include <rtems/stringto.h>

#include "internal.h"

static int usage(void)
{
  puts(rtems_shell_CMDCHMOD_Command.usage);

  return -1;
}

static void error(const char *s, int eno)
{
  printf("%s: %s\n", s, strerror(eno));
}

static int rtems_shell_main_cmdchmod(int argc, char **argv)
{
  if (argc >= 2) {
    unsigned long mode;
    rtems_status_code sc;
    uid_t task_uid;
    int i;

    sc = rtems_string_to_unsigned_long(argv[1], &mode, NULL, 8);
    if (sc != RTEMS_SUCCESSFUL) {
      return usage();
    }

    task_uid = getuid();

    for (i = 2; i < argc; ++i) {
      const char *cmd = argv[i];
      rtems_shell_cmd_t *shell_cmd = rtems_shell_lookup_cmd(cmd);

      if (shell_cmd != NULL) {
        if (task_uid == 0 || task_uid == shell_cmd->uid) {
          shell_cmd->mode = mode
            & (S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        } else if (rtems_shell_can_see_cmd(shell_cmd)) {
          error(cmd, EACCES);
        } else {
          error(cmd, ENOENT);
        }
      } else {
        error(cmd, ENOENT);
      }
    }
  } else {
    return usage();
  }

  return 0;
}

rtems_shell_cmd_t rtems_shell_CMDCHMOD_Command = {
  .name = "cmdchmod",
  .usage = "cmdchmod OCTAL-MODE COMMAND...",
  .topic = "misc",
  .command = rtems_shell_main_cmdchmod
};
