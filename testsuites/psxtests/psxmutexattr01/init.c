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

#include <tmacros.h>
#include <errno.h>
#include <pthread.h>

const char rtems_test_name[] = "PSXMUTEXATTR 1";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

#if defined(_UNIX98_THREAD_MUTEX_ATTRIBUTES)
typedef struct {
  const char *name;
  int type;
  int status;
} ToCheck_t;

ToCheck_t TypesToCheck[] = {
  { "bad type - EINVAL",             -1,                       EINVAL },
  { "PTHREAD_MUTEX_NORMAL - OK",     PTHREAD_MUTEX_NORMAL,     0 },
  { "PTHREAD_MUTEX_RECURSIVE - OK",  PTHREAD_MUTEX_RECURSIVE,  0 },
  { "PTHREAD_MUTEX_ERRORCHECK - OK", PTHREAD_MUTEX_ERRORCHECK, 0 },
  { "PTHREAD_MUTEX_DEFAULT - OK",    PTHREAD_MUTEX_DEFAULT,    0 },
};

#define TO_CHECK sizeof(TypesToCheck) / sizeof(ToCheck_t)
#endif

void *POSIX_Init(
  void *ignored
)
{
  int                 sc;
  pthread_mutexattr_t attr;
  int                 type;
  int                 i;

  TEST_BEGIN();

#if defined(_UNIX98_THREAD_MUTEX_ATTRIBUTES)
  puts( "Init - pthread_mutexattr_gettype - attr NULL - EINVAL" );
  sc = pthread_mutexattr_gettype( NULL, &type );
  rtems_test_assert( sc == EINVAL );

  puts( "Init - pthread_mutexattr_gettype - type NULL - EINVAL" );
  sc = pthread_mutexattr_gettype( &attr, NULL );
  rtems_test_assert( sc == EINVAL );

  memset( &attr, 0, sizeof(attr) );
  puts( "Init - pthread_mutexattr_gettype - attr not initialized - EINVAL" );
  sc = pthread_mutexattr_gettype( &attr, &type );
  rtems_test_assert( sc == EINVAL );

  puts( "Init - pthread_mutexattr_init - OK" );
  sc = pthread_mutexattr_init( &attr );
  rtems_test_assert( sc == 0 );

  puts( "Init - pthread_mutexattr_gettype - OK" );
  sc = pthread_mutexattr_gettype( &attr, &type );
  rtems_test_assert( sc == 0 );

  /* now do settype */
  puts( "Init - pthread_mutexattr_settype - type NULL - EINVAL" );
  sc = pthread_mutexattr_settype( NULL, PTHREAD_MUTEX_NORMAL );
  rtems_test_assert( sc == EINVAL );

  memset( &attr, 0, sizeof(attr) );
  puts( "Init - pthread_mutexattr_settype - attr not initialized - EINVAL" );
  sc = pthread_mutexattr_settype( NULL, PTHREAD_MUTEX_NORMAL );
  rtems_test_assert( sc == EINVAL );

  /* iterate over good/bad get sets */

  for (i=0 ; i<TO_CHECK ; i++ ) {
    puts( "Init - pthread_mutexattr_init - OK" );
    sc = pthread_mutexattr_init( &attr );
    rtems_test_assert( sc == 0 );

    printf( "Init - pthread_mutexattr_settype - %s\n", TypesToCheck[i].name );
    sc = pthread_mutexattr_settype( &attr, TypesToCheck[i].type );
    rtems_test_assert( sc == TypesToCheck[i].status );

    type = -2;

    if ( TypesToCheck[i].status == 0 ) {
      printf( "Init - pthread_mutexattr_gettype - %s\n", TypesToCheck[i].name );
      sc = pthread_mutexattr_gettype( &attr, &type );
      rtems_test_assert( sc == 0 );
      rtems_test_assert( type == TypesToCheck[i].type );
    }
  }
#else
  puts( "POSIX Mutex Attribute Type Not Supported in Tools" );
#endif

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS  1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
