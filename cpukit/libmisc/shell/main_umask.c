/*
 *  UMASK Shell Command Implmentation
 *
 *  Author: Fernando RUIZ CASAS
 *  Work: fernando.ruiz@ctv.es
 *  Home: correo@fernando-ruiz.com
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
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/stringto.h>
#include "internal.h"

static int rtems_shell_main_umask(
  int   argc,
  char *argv[]
)
{
  unsigned long tmp;
  mode_t        msk = umask(0);

  if (argc == 2) {
    if ( rtems_string_to_unsigned_long(argv[1], &tmp, NULL, 0) ) {
      printf( "Mask argument (%s) is not a number\n", argv[1] );
      return -1;
    }
    msk = (mode_t) tmp;

  }
  umask(msk);

  msk = umask(0);
  printf("0%o\n", (unsigned int) msk);
  umask(msk);
  return 0;
}

rtems_shell_cmd_t rtems_shell_UMASK_Command = {
  "umask",                                    /* name */
  "umask [new_umask]",                        /* usage */
  "misc",                                     /* topic */
  rtems_shell_main_umask,                     /* command */
  NULL,                                       /* alias */
  NULL                                        /* next */
};
