/*
 *  MEDIT Shell Command Implmentation
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

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

extern int rtems_shell_main_mdump(int, char *);

int rtems_shell_main_medit(
  int   argc,
  char *argv[]
)
{
  unsigned char * pb;
  int n,i;

  if (argc<3) {
    fprintf(stderr,"%s: too few arguments\n", argv[0]);
    return -1;
  }

  pb = (unsigned char*)rtems_shell_str2int(argv[1]);
  i = 2;
  n = 0;
  while (i<=argc) {
    pb[n++] = rtems_shell_str2int(argv[i++]) % 0x100;
  }

  return 0;
}

rtems_shell_cmd_t rtems_shell_MEDIT_Command = {
  "medit",                                      /* name */
  "medit address value1 [value2 ...]",          /* usage */
  "mem",                                        /* topic */
  rtems_shell_main_medit,                       /* command */
  NULL,                                         /* alias */
  NULL                                          /* next */
};
