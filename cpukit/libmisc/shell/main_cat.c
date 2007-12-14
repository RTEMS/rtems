/*
 *  CAT Shell Command Implmentation
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
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <sys/types.h>
#include <stddef.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

int main_cat(int argc, char *argv[])
{
   int n;
   n=1;

   while (n<argc)
     cat_file(stdout,argv[n++]);
   return 0;
}

shell_cmd_t Shell_CAT_Command = {
  "cat",                                           /* name */
  "cat n1 [n2 [n3...]] # show the ascii contents", /* usage */
  "files",                                         /* topic */
  main_cat ,                                       /* command */
  NULL,                                            /* alias */
  NULL                                             /* next */
};
