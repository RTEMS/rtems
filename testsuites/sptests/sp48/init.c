/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Verify creation of semaphores with unlimited attribute works.
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
