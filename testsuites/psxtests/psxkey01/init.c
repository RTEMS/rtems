/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"
#include <errno.h>
#include "tmacros.h"

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void Key_destructor(void *key_data);

void Key_destructor(void *key_data)
{
}

void *POSIX_Init(
  void *argument
)
{
  int    status;

  puts( "\n\n*** POSIX KEY 01 TEST ***" );

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08" PRIxpthread_t "\n", Init_id );

  rtems_workspace_greedy_allocate( NULL, 0 );

  puts("Init: pthread_key_create - ENOMEM (Workspace not available)");
  empty_line();
  status = pthread_key_create( &Key_id[0], Key_destructor );
  fatal_directive_check_status_only( status, ENOMEM, "no workspace available" );

  puts( "*** END OF POSIX KEY 01 TEST ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
