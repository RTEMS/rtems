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
#include <rtems/libio.h>
#include "internal.h"

static bool print_filesystem(const rtems_filesystem_table_t *entry, void *arg)
{
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
            "%s: -t needs a type of file-system;; see -L.\n",
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
            "%s: -o needs a list if filesystem options.\n",
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
  "mount",                                     /* name */
  "mount [-t type] [-r] [-L] source target",   /* usage */
  "files",                                     /* topic */
  rtems_shell_main_mount,                      /* command */
  NULL,                                        /* alias */
  NULL                                         /* next */
};
