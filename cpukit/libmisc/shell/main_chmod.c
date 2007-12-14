/*
 *  CHMOD Shell Command Implmentation
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

int main_chmod(int argc,char *argv[])
{
  int n;
  mode_t mode;

  if (argc > 2){
    mode = str2int(argv[1])&0777;
    n = 2;
    while (n<argc)
      chmod(argv[n++], mode);
  }
  return 0;
}

shell_cmd_t Shell_CHMOD_Command = {
  "chmod",                                      /* name */
  "chmod 0777 n1 n2... # change filemode",      /* usage */
  "files",                                      /* topic */
  main_chmod,                                   /* command */
  NULL,                                         /* alias */
  NULL                                          /* next */
};
