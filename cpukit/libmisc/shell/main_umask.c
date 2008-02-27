/*
 *  UMASK Shell Command Implmentation
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
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <rtems.h>
#include <rtems/monitor.h>
#include <rtems/shell.h>
#include "internal.h"

int rtems_shell_main_umask(
  int   argc,
  char *argv[]
)
{
  mode_t msk = umask(0);

  if (argc == 2)
    msk = rtems_shell_str2int(argv[1]);
  umask(msk);

  msk = umask(0);
  printf("0%o\n", (unsigned int) msk);
  umask(msk);
  return 0;
}

rtems_shell_cmd_t rtems_shell_UMASK_Command = {
  "umask",                                    /* name */
  "umask [new_umask]",                        /* usage */
  "misc",                                     /* topic */
  rtems_shell_main_umask,                     /* command */
  NULL,                                       /* alias */
  NULL                                        /* next */
};
