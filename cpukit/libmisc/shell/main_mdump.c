/*
 *  MDUMP Shell Command Implmentation
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
#include <inttypes.h>
#include <string.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

/*----------------------------------------------------------------------------*
 * RAM MEMORY COMMANDS
 *----------------------------------------------------------------------------*/

int rtems_shell_main_mdump(
  int   argc,
  char *argv[]
)
{
  unsigned char  n, m;
  int            max;
  int            res;
  uintptr_t      addr = 0;
  unsigned char *pb;

  if (argc>1)
    addr = rtems_shell_str2int(argv[1]);

  if (argc>2) {
    max = rtems_shell_str2int(argv[2]);
    if (max <= 0) {
      max = 1;      /* print 1 item if 0 or neg. */ 
      res = 0;
    }
    else {
      max--;
      res = max & 0xf;/* num bytes in last row */
      max >>= 4;      /* div by 16 */
      max++;          /* num of rows to print */
      if (max > 20) { /* limit to 20 */
        max = 20;
        res = 0xf;    /* 16 bytes print in last row */
      }
    }
  }
  else {
    max = 20;
    res = 0xf;
  }

  for (m=0; m<max; m++) {
    printf("0x%08" PRIXPTR " ", addr);
    pb = (unsigned char*) addr;
    for (n=0;n<=(m==(max-1)?res:0xf);n++)
      printf("%02X%c",pb[n],n==7?'-':' ');
    for (;n<=0xf;n++)
      printf("  %c",n==7?'-':' ');
    for (n=0;n<=(m==(max-1)?res:0xf);n++) {
      printf("%c", isprint(pb[n]) ? pb[n] : '.');
    }
    printf("\n");
    addr += 16;
  }
  return 0;
}

rtems_shell_cmd_t rtems_shell_MDUMP_Command = {
  "mdump",                                      /* name */
  "mdump [address [length]]",                   /* usage */
  "mem",                                        /* topic */
  rtems_shell_main_mdump,                       /* command */
  NULL,                                         /* alias */
  NULL                                          /* next */
};

