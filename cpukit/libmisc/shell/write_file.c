/*
 *
 *  Write buffer to a file
 *
 *  Author:
 *
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
#include <unistd.h>
#include <string.h>

#include <rtems/shell.h>

void rtems_shell_write_file(
  const char *name,
  const char *content
)
{
  FILE * fd;

  fd = fopen(name,"w");
  if ( !fd ) {
    fprintf( stderr, "Unable to write %s\n", name );
  }

  if (fd) {
    fwrite(content,1,strlen(content),fd);
    fclose(fd);
  }
}


