/*  pthread_1_through_3
 *
 *  This routine serves as a test task.  It verifies the basic task
 *  switching capabilities of the executive.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include "system.h"
#include <sys/utsname.h>

void * pthread_1_through_3( void *argument )
{
  int status;
  struct timespec tv;
  struct timespec tr;
  time_t remaining;

  remaining = sleep( 10 );
  assert( !remaining );
    
  while( FOREVER ) {
    status = clock_gettime( CLOCK_REALTIME, &tv );
    posix_service_failed( status, "clock_gettime" );
    
    printf( "pthread: ID is 0x%08x\n", pthread_self() );
    printf( " - clock_gettime - %s\n", ctime( &tv.tv_sec ) );

    remaining = sleep( 30 );
    assert( !remaining );
  }
  
  return NULL; /* just so the compiler thinks we returned something */
}
