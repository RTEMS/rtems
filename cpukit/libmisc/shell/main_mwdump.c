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
#include <rtems/stringto.h>
#include "internal.h"

static int rtems_shell_main_mwdump(
  int   argc,
  char *argv[]
)
{
  unsigned char  n;
  unsigned char  m;
  int            max;
  int            res;
  void          *addr = 0;
  unsigned char *pb;

  if ( argc > 1 ) {
    if ( rtems_string_to_pointer(argv[1], &addr, NULL) ) {
      printf( "Address argument (%s) is not a number\n", argv[1] );
      return -1;
    }
  }

  if ( argc > 2 ) {
    if ( rtems_string_to_int(argv[2], &max, NULL, 0) ) {
      printf( "Address argument (%s) is not a number\n", argv[1] );
      return -1;
    }

    if (max <= 0) {
      max = 1;      /* print 1 item if 0 or neg. */
      res = 0;
    } else {
      max--;
      res = max & 0xf;/* num bytes in last row */
      max >>= 4;      /* div by 16 */
      max++;          /* num of rows to print */
      if (max > 20) { /* limit to 20 */
        max = 20;
        res = 0xf;    /* 16 bytes print in last row */
      }
    }
  } else {
    max = 20;
    res = 0xf;
  }

  pb = addr;
  for (m=0;m<max;m++) {
    printf("%10p ", pb);
    for (n=0;n<=(m==(max-1)?res:0xf);n+=2)
      printf("%04X%c",*((unsigned short*)(pb+n)),n==6?'-':' ');
    for (;n<=0xf;n+=2)
      printf("    %c", n==6?'-':' ');
    for (n=0;n<=(m==(max-1)?res:0xf);n++) {
      printf("%c", isprint(pb[n]) ? pb[n] : '.');
    }
    printf("\n");
    pb += 16;
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
