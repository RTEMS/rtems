/*
 *  MFILL Shell Command Implmentation
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

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/stringto.h>
#include "internal.h"

static int rtems_shell_main_mfill(
  int   argc,
  char *argv[]
)
{
  unsigned long  tmp;
  void          *addr;
  size_t         size;
  unsigned char  value;

  if ( argc != 4 ) {
    fprintf(stderr,"%s: too few arguments\n", argv[0]);
    return -1;
  }

  /*
   *  Convert arguments into numbers
   */
  if ( rtems_string_to_pointer(argv[1], &addr, NULL) ) {
    printf( "Address argument (%s) is not a number\n", argv[1] );
    return -1;
  }

  if ( rtems_string_to_unsigned_long(argv[2], &tmp, NULL, 0) ) {
    printf( "Size argument (%s) is not a number\n", argv[2] );
    return -1;
  }
  size = (size_t) tmp;

  if ( rtems_string_to_unsigned_char(argv[3], &value, NULL, 0) ) {
    printf( "Value argument (%s) is not a number\n", argv[3] );
    return -1;
  }

  /*
   *  Now fill the memory.
   */
  memset(addr, size, value);

  return 0;
}

rtems_shell_cmd_t rtems_shell_MFILL_Command = {
  "mfill",                                      /* name */
  "mfill address size value",                   /* usage */
  "mem",                                        /* topic */
  rtems_shell_main_mfill,                       /* command */
  NULL,                                         /* alias */
  NULL                                          /* next */
};
