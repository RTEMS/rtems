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
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <rtems/shellconfig.h>

#include "internal.h"

static int usage(void)
{
  puts(rtems_shell_CMDCHOWN_Command.usage);

  return -1;
}

static void error(const char *s, int eno)
{
  printf("%s: %s\n", s, strerror(eno));
}

static int rtems_shell_main_cmdchown(int argc, char **argv)
{
  if (argc >= 2) {
    const char *s = argv[1];
    unsigned new_uid = UINT_MAX;
    unsigned new_gid = UINT_MAX;
    bool change_uid = false;
    bool change_gid = false;
    uid_t task_uid;
    int i;

    if (strcmp(s, ":") != 0) {
      int n = sscanf(argv[1], "%u:%u", &new_uid, &new_gid);

      if (n == 2) {
        change_uid = true;
        change_gid = true;
      } else if (n == 1) {
        change_uid = true;
      } else {
        n = sscanf(argv[1], ":%u", &new_gid);

        if (n == 1) {
          change_gid = true;
        } else {
          return usage();
        }
      }
    }

    task_uid = getuid();

    for (i = 2; i < argc; ++i) {
      const char *cmd = argv[i];
      rtems_shell_cmd_t *shell_cmd = rtems_shell_lookup_cmd(cmd);

      if (shell_cmd != NULL) {
        if (task_uid == 0 || task_uid == shell_cmd->uid) {
          if (change_uid) {
            shell_cmd->uid = new_uid;
          }

          if (change_gid) {
            shell_cmd->gid = new_gid;
          }
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

rtems_shell_cmd_t rtems_shell_CMDCHOWN_Command = {
  .name = "cmdchown",
  .usage = "cmdchown [OWNER][:[GROUP]] COMMAND...",
  .topic = "misc",
  .command = rtems_shell_main_cmdchown
};
