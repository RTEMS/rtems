/*
 *  Given Name or ID String, give Id
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
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

#include <rtems.h>
#include <rtems/stringto.h>

#ifndef METHOD_NAME
  #define METHOD_NAME lookup_task
#endif
#ifndef RTEMS_IDENT_NAME
  #define RTEMS_IDENT_NAME rtems_task_ident
#endif

int METHOD_NAME(
  const char *string,
  rtems_id   *id
)
{
  char               name[5];
  rtems_status_code  status;
  unsigned long      tmp;

  if ( string[0] != '0' ) {
    memset( name, '\0', sizeof(name) );
    strncpy( name, string, 4 );
    status = RTEMS_IDENT_NAME( 
      rtems_build_name( name[0], name[1], name[2], name[3] ),
      OBJECTS_SEARCH_ALL_NODES,
      id
    );
    if ( status != RTEMS_SUCCESSFUL )
      return 1;
  } else {
    if ( rtems_string_to_unsigned_long( string, &tmp, NULL, 0) ) {
      fprintf( stderr, "Argument (%s) is not a number\n", string );
      return 1;
    }
    *id = (rtems_id) tmp;
  }

  return 0;
}
