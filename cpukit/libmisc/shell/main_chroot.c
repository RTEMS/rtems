/*
 *  CHROOT Shell Command Implmentation
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
#include "internal.h"

static int rtems_shell_main_chroot(
  int argc,
  char * argv[]
)
{
  char *new_root = "/";

  if (argc == 2)
     new_root = argv[1];

  if ( chroot(new_root) < 0 ) {
    fprintf(stderr,"chroot %s (%s)\n", new_root, strerror(errno));
    return -1;
  }

  return 0;
}

rtems_shell_cmd_t rtems_shell_CHROOT_Command = {
  "chroot",                                     /* name */
  "chroot [dir] # change the root directory",   /* usage */
  "files",                                      /* topic */
  rtems_shell_main_chroot,                      /* command */
  NULL,                                         /* alias */
  NULL                                          /* next */
};
