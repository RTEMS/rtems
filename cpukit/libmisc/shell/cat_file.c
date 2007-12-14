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
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

void cat_file(FILE * out,char * name) {
  FILE * fd;
  int c;

  if (out) {
    fd = fopen(name,"r");
    if (fd) {
       while ((c=fgetc(fd))!=EOF) fputc(c,out);
     fclose(fd);
    }
  }
}


