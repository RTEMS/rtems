/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2012.
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

#include "tmacros.h"
#include <pthread.h>
#include <errno.h>

const char rtems_test_name[] = "PSXAUTOINIT 2";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

void *POSIX_Init(
  void *argument
)
{
  int            sc;
  pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
  pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
  pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER;
  pthread_cond_t cond4 = PTHREAD_COND_INITIALIZER;
  pthread_cond_t cond5 = PTHREAD_COND_INITIALIZER;
  pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
  struct timespec to;

  TEST_BEGIN();

  puts( "Init - pthread_cond_broadcast - auto initialize - OK" );
  sc = pthread_cond_broadcast( &cond1 );
  fatal_posix_service_status( sc, 0, "cond broadcast OK" );

  puts( "Init - pthread_cond_signal - auto initialize - OK" );
  sc = pthread_cond_signal( &cond2 );
  fatal_posix_service_status( sc, 0, "cond signal OK" );

  puts( "Init - pthread_cond_init - auto initialize - OK" );
  sc = pthread_cond_init( &cond3, NULL );
  fatal_posix_service_status( sc, 0, "cond init OK" );

  puts( "Init - pthread_mutex_lock - OK" );
  sc = pthread_mutex_lock( &mtx );
  fatal_posix_service_status( sc, 0, "mtx lock OK" );

  puts( "Init - pthread_cond_timedwait - auto initialize - OK" );
  to.tv_sec = 1;
  to.tv_nsec = 1;
  sc = pthread_cond_timedwait( &cond4, &mtx, &to );
  fatal_posix_service_status( sc, ETIMEDOUT, "cond timedwait OK" );

  puts( "Init - pthread_mutex_unlock - OK" );
  sc = pthread_mutex_unlock( &mtx );
  fatal_posix_service_status( sc, 0, "mtx unlock OK" );

  puts( "Init - pthread_mutex_destroy - OK" );
  sc = pthread_mutex_destroy( &mtx );
  fatal_posix_service_status( sc, 0, "mtx destroy OK" );

  puts( "Init - pthread_cond_destroy - OK" );
  sc = pthread_cond_destroy( &cond5 );
  fatal_posix_service_status( sc, 0, "cond destroy OK" );

  puts( "Init - pthread_cond_destroy - EINVAL" );
  sc = pthread_cond_destroy( &cond5 );
  fatal_posix_service_status( sc, EINVAL, "cond destroy EINVAL" );

  puts( "Init - pthread_cond_destroy - OK" );
  sc = pthread_cond_destroy( &cond4 );
  fatal_posix_service_status( sc, 0, "cond destroy OK" );

  puts( "Init - pthread_cond_destroy - OK" );
  sc = pthread_cond_destroy( &cond3 );
  fatal_posix_service_status( sc, 0, "cond destroy OK" );

  puts( "Init - pthread_cond_destroy - OK" );
  sc = pthread_cond_destroy( &cond2 );
  fatal_posix_service_status( sc, 0, "cond destroy OK" );

  puts( "Init - pthread_cond_destroy - OK" );
  sc = pthread_cond_destroy( &cond1 );
  fatal_posix_service_status( sc, 0, "cond destroy OK" );

  TEST_END();
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS 1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
