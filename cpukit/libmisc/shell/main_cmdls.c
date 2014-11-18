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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <rtems/shellconfig.h>

#include "internal.h"

static void error(const char *s, int eno)
{
  printf("%s: %s\n", s, strerror(eno));
}

static void print_cmd(const rtems_shell_cmd_t *shell_cmd)
{
  if (rtems_shell_can_see_cmd(shell_cmd)) {
    mode_t m = shell_cmd->mode;

    printf(
      "%c%c%c%c%c%c%c%c%c %5u %5u %s\n",
      (m & S_IRUSR) != 0 ? 'r' : '-',
      (m & S_IWUSR) != 0 ? 'w' : '-',
      (m & S_IXUSR) != 0 ? 'x' : '-',
      (m & S_IRGRP) != 0 ? 'r' : '-',
      (m & S_IWGRP) != 0 ? 'w' : '-',
      (m & S_IXGRP) != 0 ? 'x' : '-',
      (m & S_IROTH) != 0 ? 'r' : '-',
      (m & S_IWOTH) != 0 ? 'w' : '-',
      (m & S_IXOTH) != 0 ? 'x' : '-',
      (unsigned) shell_cmd->uid,
      (unsigned) shell_cmd->gid,
      shell_cmd->name
    );
  }
}

static int rtems_shell_main_cmdls(int argc, char **argv)
{
  const rtems_shell_cmd_t *shell_cmd;

  if (argc <= 1) {
    shell_cmd = rtems_shell_first_cmd;

    while (shell_cmd != NULL) {
      print_cmd(shell_cmd);

      shell_cmd = shell_cmd->next;
    }
  } else {
    int i;

    for (i = 1; i < argc; ++i) {
      const char *cmd = argv[i];

      shell_cmd = rtems_shell_lookup_cmd(cmd);

      if (shell_cmd != NULL) {
        print_cmd(shell_cmd);
      } else {
        error(cmd, ENOENT);
      }
    }
  }

  return 0;
}

rtems_shell_cmd_t rtems_shell_CMDLS_Command = {
  .name = "cmdls",
  .usage = "cmdls COMMAND...",
  .topic = "misc",
  .command = rtems_shell_main_cmdls
};
