/*
 *  RM Shell Command Implmentation
 *
 *  Author: Fernando RUIZ CASAS
 *  Work: fernando.ruiz@ctv.es
 *  Home: correo@fernando-ruiz.com
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
    return 1;
  }

  close (fd);
  return 0;
}

rtems_shell_cmd_t rtems_shell_BLKSYNC_Command = {
  "blksync",                                 /* name */
  "blksync driver # sync the block driver",  /* usage */
  "files",                                   /* topic */
  rtems_shell_main_blksync,                  /* command */
  NULL,                                      /* alias */
  NULL                                       /* next */
};
