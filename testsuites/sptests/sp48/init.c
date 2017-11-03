/*
 *  Verify creation of semaphores with unlimited attribute works.
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <rtems/libcsupport.h>

const char rtems_test_name[] = "SP 48";

rtems_task Init(rtems_task_argument ignored);

#define SEMA_COUNT 5000
rtems_id Semaphores[SEMA_COUNT];

rtems_task Init(rtems_task_argument ignored)
{
  rtems_status_code sc;
  int               i;
  int               created;

  TEST_BEGIN();

  printf(
    "Largest C program heap block available: %zu\n",
    malloc_free_space()
  );
  for (i=0 ; i<SEMA_COUNT ; i++ ) {
    sc = rtems_semaphore_create(
      rtems_build_name('s', 'e', 'm', ' '),
      1,
      RTEMS_DEFAULT_ATTRIBUTES,
      0,
      &Semaphores[i]
    );
    /* printf("Creating %i id=0x%08x\n", i, Semaphores[i]); */

    if (sc == RTEMS_TOO_MANY) {
      printf("We run out at %i!\n", i);
      break;
    }
    if (sc != RTEMS_SUCCESSFUL) {
      printf("FAILED creating at %i\n", i);
      directive_failed( sc, "rtems_semaphore_create " );
      rtems_test_exit( 0 );
    }
  }

  created = i;
  if ( created == SEMA_COUNT )
    puts( "Created all semaphores allowed in this test" );

  printf( "%d semaphores created\n", i );
  printf(
    "Largest C program heap block available: %zu\n",
    malloc_free_space()
  );

  for ( i-- ; i ; i-- ) {
    sc = rtems_semaphore_delete( Semaphores[i] );
    if (sc != RTEMS_SUCCESSFUL) {
      printf("FAILED deleting at %i\n", i);
      directive_failed( sc, "rtems_semaphore_delete " );
      rtems_test_exit( 0 );
    }
  }

  printf( "%d semaphores successfully deleted\n", created );
  printf(
    "Largest C program heap block available: %zu\n",
    malloc_free_space()
  );

  TEST_END();
  rtems_test_exit( 0 );
}

/* configuration stuff */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS        1
#define CONFIGURE_MAXIMUM_SEMAPHORES   rtems_resource_unlimited(5)
#if 1
  #define CONFIGURE_UNIFIED_WORK_AREAS
#else
  #define CONFIGURE_MEMORY_OVERHEAD    1024
#endif

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
