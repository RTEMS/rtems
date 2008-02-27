/*
 *  MWDUMP Shell Command Implmentation
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
#include <inttypes.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

int rtems_shell_main_mwdump(
  int   argc,
  char *argv[]
)
{
  unsigned char   n, m, max=20;
  uintptr_t       addr=0;
  unsigned short *pw;
  unsigned char  *p;

  if (argc>1)
    addr = rtems_shell_str2int(argv[1]);

  if (argc>2) {
    max = rtems_shell_str2int(argv[2]);
    max /= 16;
  }

  if (!max)
    max = 1;

  for (m=0;m<max;m++) {
    printf("0x%08" PRIXPTR " ",addr);
    p = (unsigned char *) addr;
    pw = (unsigned short*) addr;
    for (n=0;n<8;n++)
      printf("%04X%c",pw[n],n==3?'-':' ');
    for (n=0;n<16;n++) {
      printf("%c",isprint(p[n])?p[n]:'.');
    }
    printf("\n");
    addr += 16;
  }
  return 0;
}

rtems_shell_cmd_t rtems_shell_WDUMP_Command = {
  "wdump",                                      /* name */
  "wdump [address [length]]",                   /* usage */
  "mem",                                        /* topic */
  rtems_shell_main_mwdump,                      /* command */
  NULL,                                         /* alias */
  NULL                                          /* next */
};
