/*
 *  RMDIR Shell Command Implmentation
 *
 *  Author: Fernando RUIZ CASAS
 *  Work: fernando.ruiz@ctv.es
 *  Home: correo@fernando-ruiz.com
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

int main_rmdir (int argc, char *argv[])
{
  char *dir;
  int n;

  n=1;
  while (n<argc) {
    dir=argv[n++];
    if (rmdir(dir))
      fprintf(stdout, "rmdir '%s' failed:%s\n", dir, strerror(errno));
  }
  return errno;
}

shell_cmd_t Shell_RMDIR_Command = {
  "rmdir",                                      /* name */
  "rmdir  dir   # remove directory",            /* usage */
  "files",                                      /* topic */
  main_rmdir,                                   /* command */
  NULL,                                         /* alias */
  NULL                                          /* next */
};
