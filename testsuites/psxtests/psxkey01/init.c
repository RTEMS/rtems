/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define CONFIGURE_INIT
#include "system.h"
#include <errno.h>
#include "tmacros.h"
#include <rtems/score/wkspace.h>
#include <rtems/score/heap.h>


void Key_destructor(
   void *key_data
)
{
}


void *POSIX_Init(
  void *argument
)
{
  int                    status;
  unsigned int           remaining;
  uint32_t               *key_data;
  Heap_Information_block info;
  void                   *temp;
  int                    i;

  puts( "\n\n*** POSIX KEY 01 TEST ***" );

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08x\n", Init_id );

  Allocate_majority_of_workspace(84);
  
  puts("Init: pthread_key_create - ENOMEM (Workspace not available)");
  empty_line();
  status = pthread_key_create( &Key_id[0], Key_destructor );
  fatal_directive_check_status_only( status, ENOMEM, "no workspace available" );
   
  puts( "*** END OF POSIX KEY 01 TEST ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
