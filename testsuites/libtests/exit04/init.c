/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2025 Mazen Adel Elmessady
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
 * SUBSTITUTE GOODS OR SEenoICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tmacros.h"

#include <pthread.h>
#include <stdlib.h>

/* Use assert() not rtems_test_assert() since it uses exit() */
#include <assert.h>

#include <rtems.h>

const char rtems_test_name[] = "EXIT 4";

#define EXIT_STATUS 0

static int counter;

static void at_quick_exit_0( void )
{
  assert( counter == 0 );
  ++counter;
}

static void at_quick_exit_1( void )
{
  assert( counter == 1 );
  ++counter;
}

static void at_quick_exit_2( void )
{
  assert( counter == 2 );
  ++counter;
}

static void at_quick_exit_3( void )
{
  assert( 0 ); // this shouldn't be called
}

static void fatal_extension(
  rtems_fatal_source source,
  bool               always_set_to_false,
  rtems_fatal_code   error
)
{
  if ( source == RTEMS_FATAL_SOURCE_EXIT && !always_set_to_false &&
       error == EXIT_STATUS && counter == 3 ) {
    TEST_END();
  }
}

static void *exit_thread( void *arg )
{
  (void) arg;

  int       eno;
  uintptr_t max_free_size = 0;
  void     *greedy;
  eno = at_quick_exit( at_quick_exit_2 );
  assert( eno == 0 );

  eno = at_quick_exit( at_quick_exit_1 );
  assert( eno == 0 );

  eno = at_quick_exit( at_quick_exit_0 );
  assert( eno == 0 );

  greedy = rtems_workspace_greedy_allocate( &max_free_size, 0 );

  eno = at_quick_exit( at_quick_exit_3 );

  assert( eno != 0 );

  rtems_workspace_greedy_free( greedy );

  quick_exit( EXIT_STATUS );
}

static void Init( rtems_task_argument arg )
{
  (void) arg;

  pthread_t thread;
  int       eno;

  TEST_BEGIN();

  eno = pthread_create( &thread, NULL, exit_thread, NULL );
  assert( eno == 0 );

  eno = pthread_join( thread, NULL );
  assert( eno == 0 );

  /* Should not reach here if quick_exit works correctly */
  assert( 0 );
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .fatal = fatal_extension }, RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_POSIX_THREADS 1

#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
