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

#include <pthread.h>
#include <errno.h>
#include "tmacros.h"
#include "pmacros.h"

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

void *POSIX_Init(
  void *ignored
)
{
  pthread_key_t           key;
  int                     sc;
  bool                    sb;
  Heap_Information_block  start;
  Heap_Information_block  info;
  size_t                  to_alloc;
  void                   *alloced;

  puts( "\n\n*** TEST KEY 02 ***" );

  puts( "Init - rtems_workspace_get_information - OK" );
  sb = rtems_workspace_get_information( &start );
  rtems_test_assert( sb );

  #if 0
    printf( "Init - workspace free = %d\n", start.Free.largest );
    printf( "Init - workspace free blocks = %d\n", start.Free.number );
  #endif
  rtems_test_assert( start.Free.number == 1 );
  to_alloc = start.Free.largest;

  /* find the largest we can actually allocate */
  while ( 1 ) {
    sb = rtems_workspace_allocate( to_alloc, &alloced );
    if ( sb )
      break;
    to_alloc -= 4;
  }

  rtems_workspace_free( alloced );

  #if 0
    printf( "Init - start with to_alloc of = %d\n", to_alloc );
  #endif

  /*
   * Verify heap is still in same shape if we couldn't allocate a task
   */
  sb = rtems_workspace_get_information( &info );
  rtems_test_assert( sb );
  rtems_test_assert( info.Free.largest == start.Free.largest );
  rtems_test_assert( info.Free.number  == start.Free.number  );

  puts( "Init - pthread_key_create - ENOMEM" );
  while (1) {

    sb = rtems_workspace_allocate( to_alloc, &alloced );
    rtems_test_assert( sb );

    sc = pthread_key_create( &key, NULL );

    /* free the memory we snagged, then check the status */
    rtems_workspace_free( alloced );

    if ( !sc )
      break;

    if ( sc != ENOMEM ) {
      printf( "key create returned %s\n", strerror(sc) );
      rtems_test_exit(0);
    }

    /*
     * Verify heap is still in same shape if we couldn't allocate a task
     */
    sb = rtems_workspace_get_information( &info );
    #if 0
      printf( "Init - workspace free/blocks = %d/%d\n",
        info.Free.largest, info.Free.number );
    #endif
    rtems_test_assert( sb );
    rtems_test_assert( info.Free.largest == start.Free.largest );
    rtems_test_assert( info.Free.number  == start.Free.number  );

    to_alloc -= 8;
    if ( to_alloc == 0 )
     break;
  }

  if ( sc )
    rtems_test_exit(0);

  /*
   * Verify heap is still in same shape after we free the task
   */
  puts( "Init - pthread_key_delete - OK" );
  sc = pthread_key_delete( key );
  rtems_test_assert( sc == 0 );

  puts( "Init - verify workspace has same memory" );
  sb = rtems_workspace_get_information( &info );
  #if 0
    printf( "Init - workspace free/blocks = %d/%d\n",
      info.Free.largest, info.Free.number );
  #endif
  rtems_test_assert( sb );
  rtems_test_assert( info.Free.largest == start.Free.largest );
  rtems_test_assert( info.Free.number  == start.Free.number  );

  puts( "*** END OF TEST KEY 02 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS  1
#define CONFIGURE_MAXIMUM_POSIX_KEYS     1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
