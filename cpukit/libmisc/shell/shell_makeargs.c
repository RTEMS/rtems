/*
 *  Split string into argc/argv style argument list
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

int rtems_shell_make_args(
  char  *commandLine,
  int   *argc_p, 
  char **argv_p, 
  int    max_args
)
{
  int   argc;
  char *command;
  int   status = 0;
  
  argc = 0;
  command = commandLine;

  while ( 1 ) {
    command = strtok( command, " \t\r\n" );
    if ( command == NULL )
      break;
    argv_p[ argc++ ] = command;
    command = '\0';
    if ( argc == (max_args-1) ) {
      status = -1;
      break;
    }
  }
  argv_p[ argc ] = NULL;
  *argc_p = argc;
  return status;
}

