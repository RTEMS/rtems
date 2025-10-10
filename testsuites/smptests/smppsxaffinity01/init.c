/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2014.
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

#define  _GNU_SOURCE

#include <tmacros.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>

const char rtems_test_name[] = "SMPPSXAFFINITY 1";

#define CPU_COUNT 4

pthread_t           Init_id;

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void Validate_attrgetaffinity_errors(void);
void Validate_attrsetaffinity_errors(void);
void Validate_attr(void);

void Validate_attrgetaffinity_errors(void)
{
  int                 sc;
  cpu_set_t           cpuset;
  pthread_attr_t      attr;

  sc = pthread_attr_init( &attr );
  rtems_test_assert( sc == 0 );

  /* Verify pthread_attr_getaffinity_np validates attr  */
  puts( "Init - pthread_attr_getaffinity_np - Invalid attr - EINVAL" );
  sc = pthread_attr_getaffinity_np( NULL, sizeof( cpuset ), &cpuset );
  rtems_test_assert( sc == EINVAL );

  /* Verify pthread_attr_getaffinity_np validates cpuset */
  puts( "Init - pthread_attr_getaffinity_np - Invalid attr - EINVAL" );
  sc = pthread_attr_getaffinity_np( &attr, sizeof( cpuset ), NULL );
  rtems_test_assert( sc == EINVAL );

  /* Verify pthread_attr_getaffinity_np validates cpusetsize */
  puts( "Init - pthread_attr_getaffinity_np - Invalid cpusetsize - EINVAL" );
  sc = pthread_attr_getaffinity_np( &attr, sizeof( cpuset ) * 2 , &cpuset );
  rtems_test_assert( sc == EINVAL );

  sc = pthread_attr_destroy( &attr );
  rtems_test_assert( sc == 0 );

  puts( "Init - pthread_attr_getaffinity_np - Not initialized attr - EINVAL" );
  sc = pthread_attr_getaffinity_np( &attr, sizeof( cpuset ), &cpuset );
  rtems_test_assert( sc == EINVAL );
}

void Validate_attrsetaffinity_errors(void)
{
  int                 sc;
  cpu_set_t           cpuset;
  pthread_attr_t      attr;

  sc = pthread_attr_init( &attr );
  rtems_test_assert( sc == 0 );

  /* Verify pthread_attr_setaffinity_np validates attr.  */
  puts( "Init - pthread_attr_setaffinity_np - Invalid attr - EINVAL" );
  sc = pthread_attr_setaffinity_np( NULL, sizeof( cpuset ), &cpuset );
  rtems_test_assert( sc == EINVAL );

  /* Verify pthread_attr_setaffinity_np validates cpuset    */
  puts( "Init - pthread_attr_setaffinity_np - Invalid attr - EINVAL" );
  sc = pthread_attr_setaffinity_np( &attr, sizeof( cpuset ), NULL );
  rtems_test_assert( sc == EINVAL );

  /* Verify pthread_attr_setaffinity_np validates cpusetsize */
  puts( "Init - pthread_attr_setaffinity_np - Invalid cpusetsize - EINVAL" );
  sc = pthread_attr_setaffinity_np( &attr, sizeof( cpuset ) * 2 , &cpuset );
  rtems_test_assert( sc == EINVAL );

  sc = pthread_attr_destroy( &attr );
  rtems_test_assert( sc == 0 );

  puts( "Init - pthread_attr_setaffinity_np - Not initialized attr - EINVAL" );
  sc = pthread_attr_setaffinity_np( &attr, sizeof( cpuset ), &cpuset  );
  rtems_test_assert( sc == EINVAL );
}

void Validate_attr(void )
{
  int                 sc;
  pthread_attr_t      attr;
  uint32_t            cpus;
  cpu_set_t           cpuset1;
  cpu_set_t           cpuset2;
  int                 i;
  int                 priority;

  sc = pthread_getattr_np( Init_id, &attr );
  rtems_test_assert( sc == 0 );

  priority = sched_get_priority_min( SCHED_FIFO );
  rtems_test_assert( priority != -1 );


  cpus = rtems_scheduler_get_processor_maximum();
  puts(
    "Init - Validate pthread_attr_setaffinity_np and "
    "pthread_attr_getaffinity_np"
  );

  /* Set each cpu seperately in the affinity set */
  for ( i=0; i<cpus; i++ ){
    CPU_ZERO(&cpuset1);
    CPU_SET(i, &cpuset1);

    sc = pthread_attr_setaffinity_np( &attr, sizeof(cpu_set_t), &cpuset1 );
    rtems_test_assert( sc == 0 );

    sc = pthread_attr_getaffinity_np( &attr, sizeof(cpu_set_t), &cpuset2 );
    rtems_test_assert( sc == 0 );

    rtems_test_assert( CPU_EQUAL(&cpuset1, &cpuset2) );
  }
}

void *POSIX_Init(
  void *ignored
)
{
  (void) ignored;

  TEST_BEGIN();

  /* Initialize thread id */
  Init_id = pthread_self();

  Validate_attrsetaffinity_errors();
  Validate_attrgetaffinity_errors();
  Validate_attr();

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP

#define CONFIGURE_MAXIMUM_POSIX_THREADS  1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
