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
#include <string.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

/*----------------------------------------------------------------------------*
 * RAM MEMORY COMMANDS
 *----------------------------------------------------------------------------*/

int main_mdump(int argc,char * argv[]) {
  unsigned char  n,m,max=0;
  uintptr_t      addr;
  unsigned char *pb;

  addr = current_shell_env->mdump_addr;
  if (argc>1)
    addr = str2int(argv[1]);
  if (argc>2)
    max = str2int(argv[2]);

  max /= 16;

  if (!max)
    max = 20;

  for (m=0;m<max;m++) {
    fprintf(stdout,"0x%08X ",addr);
    pb = (unsigned char*) addr;
    for (n=0;n<16;n++)
      fprintf(stdout,"%02X%c",pb[n],n==7?'-':' ');
    for (n=0;n<16;n++) {
      fprintf(stdout,"%c",isprint(pb[n])?pb[n]:'.');
    }
    fprintf(stdout,"\n");
    addr += 16;
  }
  current_shell_env->mdump_addr = addr;
  return 0;
}

shell_cmd_t Shell_MDUMP_Command = {
  "mdump",                                      /* name */
  "mdump [addr [size]]",                        /* usage */
  "mem",                                        /* topic */
  main_mdump,                                   /* command */
  NULL,                                         /* alias */
  NULL                                          /* next */
};


/*----------------------------------------------------------------------------*/
int main_mwdump(int argc,char * argv[]) {
  unsigned char n,m,max=0;
  int addr=current_shell_env->mdump_addr;
  unsigned short * pw;

  if (argc>1)
    addr = str2int(argv[1]);
  if (argc>2)
    max = str2int(argv[2]);

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
  current_shell_env->mdump_addr = addr;
  return 0;
}

shell_cmd_t Shell_WDUMP_Command = {
  "wdump",                                      /* name */
  "wdump [addr [size]]",                        /* usage */
  "mem",                                        /* topic */
  main_mwdump,                                  /* command */
  NULL,                                         /* alias */
  NULL                                          /* next */
};

/*----------------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------------*/
int main_mfill(int argc,char * argv[]) {
  int  addr;
  int  size;
  unsigned char value;

  if (argc<4) {
    fprintf(stdout,"too few arguments\n");
    return 0;
  }
  addr = str2int(argv[1]);
  size = str2int(argv[2]);
  value= str2int(argv[3]) % 0x100;
  memset((unsigned char*)addr,size,value);
  current_shell_env->mdump_addr = addr;
 return main_mdump(0,NULL);
}

shell_cmd_t Shell_MFILL_Command = {
  "mfill",                                      /* name */
  "mfill addr size value",                      /* usage */
  "mem",                                        /* topic */
  main_mfill,                                   /* command */
  NULL,                                         /* alias */
  NULL                                          /* next */
};

/*----------------------------------------------------------------------------*/
int main_mmove(int argc,char * argv[]) {
 int  src;
 int  dst;
 int  size;

 if (argc<4) {
  fprintf(stdout,"too few arguments\n");
  return 0;
 }
 dst  = str2int(argv[1]);
 src  = str2int(argv[2]);
 size = str2int(argv[3]);
 memcpy((unsigned char*)dst,(unsigned char*)src,size);
 current_shell_env->mdump_addr = dst;
 return main_mdump(0,NULL);
}

shell_cmd_t Shell_MMOVE_Command = {
  "mmove",                                      /* name */
  "mmove dst src size",                         /* usage */
  "mem",                                        /* topic */
  main_mmove,                                   /* command */
  NULL,                                         /* alias */
  NULL                                          /* next */
};
