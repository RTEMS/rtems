/* 
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#define CONFIGURE_INIT
#include "system.h"
#include <sched.h>

void *POSIX_Init(
  void *argument
)
{

  puts( "\n\n*** POSIX MESSAGE QUEUE TEST ***" );

  puts( "*** END OF POSIX MESSAGE QUEUE TEST ***" );
  exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
