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

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

int rtems_shell_main_rm(int argc, char *argv[])
{
   int n;
   n = 1;

   while (n<argc) {
     if (unlink(argv[n])) {
       fprintf(stdout,"error %s:rm %s\n",strerror(errno),argv[n]);
       return -1;
     }
     n++;
   }
   return 0;
}

rtems_shell_cmd_t rtems_Shell_RM_Command = {
  "rm",                                      /* name */
  "rm n1 [n2 [n3...]] # remove files",       /* usage */
  "files",                                   /* topic */
  rtems_shell_main_rm,                       /* command */
  NULL,                                      /* alias */
  NULL                                       /* next */
};
