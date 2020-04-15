/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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
