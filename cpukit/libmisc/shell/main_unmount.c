/*
 *   Shell Command Implmentation
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
  "unmount",                     /* name */
  "unmount path # unmount disk", /* usage */
  "files",                       /* topic */
  rtems_shell_main_unmount,      /* command */
  NULL,                          /* alias */
  NULL                           /* next */
};
