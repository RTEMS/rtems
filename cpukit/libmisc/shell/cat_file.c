/*
 *  CAT Command Implementation
 *
 *  Author:
 *   WORK: fernando.ruiz@ctv.es
 *   HOME: correo@fernando-ruiz.com
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <rtems/shell.h>

int rtems_shell_cat_file(FILE * out,const char * name) {
  FILE * fd;
  int c;

  if (out) {
    fd = fopen(name,"r");
    if (!fd) {
      return -1;
    }
    while ((c=fgetc(fd))!=EOF)
      fputc(c,out);
    fclose(fd);
  }
  return 0;
}


