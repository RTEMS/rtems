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

#include <pmacros.h>
#include <errno.h>

const char rtems_test_name[] = "PSXCANCEL 1";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

static volatile int TSR_occurred;

static volatile int TSR_status;

static rtems_id  timer_id;

static pthread_t thread;

static void *suspend_self( void *arg )
{
  int               eno;
  rtems_status_code status;

  (void) arg;

  eno = pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );
  rtems_test_assert( eno == 0 );

  status = rtems_task_suspend( RTEMS_SELF);
  rtems_test_assert( status == RTEMS_SUCCESSFUL );

  return NULL;
}

static rtems_timer_service_routine Cancel_duringISR_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  TSR_status = pthread_cancel( thread );
  TSR_occurred = 1;
}

static rtems_timer_service_routine SetState_duringISR_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  int oldstate;

  TSR_status = pthread_setcancelstate( 0, &oldstate );
  TSR_occurred = 1;
}

static rtems_timer_service_routine SetType_duringISR_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  int oldtype;

  TSR_status = pthread_setcanceltype( 0, &oldtype );
  TSR_occurred = 1;
}

static void doit(
  rtems_timer_service_routine (*TSR)(rtems_id, void *),
  const char                   *method,
  int                           expected_status
)
{
  rtems_interval    start;
  rtems_interval    end;
  rtems_status_code status;

  printf( "Init: schedule %s from a TSR\n", method );

  TSR_occurred = 0;
  TSR_status   = 0;

  status = rtems_timer_fire_after( timer_id, 10, TSR, NULL );
  rtems_test_assert( !status );

  start = rtems_clock_get_ticks_since_boot();
  do {
    end = rtems_clock_get_ticks_since_boot();
  } while ( !TSR_occurred && ((end - start) <= 800));

  if ( !TSR_occurred ) {
    printf( "%s did not occur\n", method );
    rtems_test_exit(0);
  }
  if ( TSR_status != expected_status ) {
    printf( "%s returned %s\n", method, strerror(TSR_status) );
    rtems_test_exit(0);
  }
  printf( "%s - from ISR returns expected status - OK\n", method );

}

void *POSIX_Init(
  void *argument
)
{
  rtems_status_code status;
  int               eno;
  void             *value;

  TEST_BEGIN();

  status = rtems_timer_create(
    rtems_build_name( 'T', 'M', '1', ' ' ),
    &timer_id
  );
  rtems_test_assert( !status );

  eno = pthread_create( &thread, NULL, suspend_self, NULL );
  rtems_test_assert( eno == 0 );

  doit( Cancel_duringISR_TSR, "pthread_cancel", 0 );
  doit( SetState_duringISR_TSR, "pthread_setcancelstate", EPROTO );
  doit( SetType_duringISR_TSR, "pthread_setcanceltype", EPROTO );

  value = NULL;
  eno = pthread_join( thread, &value );
  rtems_test_assert( eno == 0 );
  rtems_test_assert( value == PTHREAD_CANCELED );

  TEST_END();
  rtems_test_exit(0);
  return NULL; /* just so the compiler thinks we returned something */
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TIMERS        1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS        2
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
