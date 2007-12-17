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

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

int rtems_shell_main_mwdump(int argc,char * argv[]) {
  unsigned char n,m,max=0;
  int addr=rtems_current_shell_env->mdump_addr;
  unsigned short * pw;

  if (argc>1)
    addr = rtems_shell_str2int(argv[1]);
  if (argc>2)
    max = rtems_shell_str2int(argv[2]);

  max /= 16;

  if (!max)
    max = 20;

  for (m=0;m<max;m++) {
    fprintf(stdout,"0x%08X ",addr);
    pw = (unsigned short*) addr;
    for (n=0;n<8;n++)
      fprintf(stdout,"%02X %02X%c",pw[n]/0x100,pw[n]%0x100,n==3?'-':' ');
    for (n=0;n<8;n++) {
      fprintf(stdout,"%c",isprint(pw[n]/0x100)?pw[n]/0x100:'.');
      fprintf(stdout,"%c",isprint(pw[n]%0x100)?pw[n]%0x100:'.');
    }
    fprintf(stdout,"\n");
    addr += 16;
  }
  rtems_current_shell_env->mdump_addr = addr;
  return 0;
}

rtems_shell_cmd_t rtems_shell_WDUMP_Command = {
  "wdump",                                      /* name */
  "wdump [addr [size]]",                        /* usage */
  "mem",                                        /* topic */
  rtems_shell_main_mwdump,                      /* command */
  NULL,                                         /* alias */
  NULL                                          /* next */
};
