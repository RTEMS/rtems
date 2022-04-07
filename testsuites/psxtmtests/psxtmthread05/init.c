/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2013.
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

#include <timesys.h>
#include <pthread.h>
#include <sched.h>
#include <rtems/btimer.h>
#include "test_support.h"

const char rtems_test_name[] = "PSXTMTHREAD 05";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void benchmark_pthread_create(void);
void benchmark_pthread_setschedparam(void);
void *test_thread(void *argument);

void benchmark_pthread_setschedparam(void)
{
  int status;
  int policy;
  struct sched_param param;
  pthread_t thread_ID;

  status = pthread_create(&thread_ID, NULL, test_thread, NULL);
  rtems_test_assert( status == 0 );

  /* make test_thread equal to POSIX_Init() */
  pthread_getschedparam(pthread_self(), &policy, &param);

  pthread_setschedparam(thread_ID, policy, &param);
  /* At this point, we've switched to test_thread */

  /* Back from test_thread, switch to test_thread again */
  param.sched_priority = sched_get_priority_min(policy);

  benchmark_timer_initialize();
  //lower own priority to minimun, scheduler forces an involuntary context switch
  pthread_setschedparam(pthread_self(), policy, &param);
}

void *test_thread(
  void *argument
)
{
  uint32_t end_time;

  sched_yield();

  end_time = benchmark_timer_read();
  put_time(
    "pthread_setschedparam: lower own priority preempt",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

  TEST_END();
  rtems_test_exit(0);
  //Empty thread used in pthread_create().
  return NULL;
}

void *POSIX_Init(
  void *argument
)
{

  TEST_BEGIN();
  benchmark_pthread_setschedparam();

  rtems_test_assert( 1 );
  return NULL;
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     2
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
