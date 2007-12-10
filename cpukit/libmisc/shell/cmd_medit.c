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

extern int main_mdump(int, char *);

int main_medit(int argc,char * argv[]) {
  unsigned char * pb;
  int n,i;

  if (argc<3) {
    fprintf(stdout,"too few arguments\n");
    return 0;
  }

  pb = (unsigned char*)str2int(argv[1]);
  i = 2;
  n = 0;
  while (i<=argc) {
    pb[n++] = str2int(argv[i++])%0x100;
  }
  current_shell_env->mdump_addr = (int)pb;

  return main_mdump(0,NULL);
}

shell_cmd_t Shell_MEDIT_Command = {
  "medit",                                      /* name */
  "medit addr value [value ...]",               /* usage */
  "mem",                                        /* topic */
  main_medit,                                   /* command */
  NULL,                                         /* alias */
  NULL                                          /* next */
};
