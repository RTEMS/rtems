/*
 *  Split string into argc/argv style argument list
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <ctype.h>
#include <rtems/shell.h>

int rtems_shell_make_args(
  char  *commandLine,
  int   *argc_p,
  char **argv_p,
  int    max_args
)
{
  int   argc;
  char *ch;
  int   status = 0;
 
  argc = 0;
  ch = commandLine;

  while ( *ch ) {

    while ( isspace((unsigned char)*ch) ) ch++;

    if ( *ch == '\0' )
      break;

    if ( *ch == '"' ) {
      argv_p[ argc ] = ++ch;
      while ( ( *ch != '\0' ) && ( *ch != '"' ) ) ch++;
    } else {
      argv_p[ argc ] = ch;
      while ( ( *ch != '\0' ) && ( !isspace((unsigned char)*ch) ) ) ch++;
    }

    argc++;

    if ( *ch == '\0' )
      break;

    *ch++ = '\0';

    if ( argc == (max_args-1) ) {
        status = -1;
        break;
    }


  }
  argv_p[ argc ] = NULL;
  *argc_p = argc;
  return status;
}

