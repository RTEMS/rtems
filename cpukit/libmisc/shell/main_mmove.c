/*
 *  MMOVE Shell Command Implmentation
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

static int rtems_shell_main_mmove(
  int   argc,
  char *argv[]
)
{
  unsigned long  tmp;
  void          *src;
  void          *dst;
  size_t         length;

  if ( argc < 4 ) {
    fprintf(stderr,"%s: too few arguments\n", argv[0]);
    return -1;
   }

  /*
   *  Convert arguments into numbers
   */
  if ( rtems_string_to_pointer(argv[1], &dst, NULL) ) {
    printf( "Destination argument (%s) is not a number\n", argv[1] );
    return -1;
  }

  if ( rtems_string_to_pointer(argv[2], &src, NULL) ) {
    printf( "Source argument (%s) is not a number\n", argv[2] );
    return -1;
  }

  if ( rtems_string_to_unsigned_long(argv[3], &tmp, NULL, 0) ) {
    printf( "Length argument (%s) is not a number\n", argv[3] );
    return -1;
  }
  length = (size_t) tmp;

  /*
   *  Now copy the memory.
   */
  memcpy(dst, src, length);

 return 0;
}

rtems_shell_cmd_t rtems_shell_MMOVE_Command = {
  "mmove",                                      /* name */
  "mmove dst src length",                       /* usage */
  "mem",                                        /* topic */
  rtems_shell_main_mmove,                       /* command */
  NULL,                                         /* alias */
  NULL                                          /* next */
};
