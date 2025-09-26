/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2009.
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

#include <sched.h>
#include <sys/utsname.h>

#define CONFIGURE_INIT
#include "system.h"
#include "pritime.h"

#include <rtems/score/todimpl.h>

const char rtems_test_name[] = "PSX 1";

void *POSIX_Init(
  void *argument
)
{
  (void) argument;

  struct timespec tr;
  int             status;
  int             priority;
  pthread_t       thread_id;
  struct utsname  uts;

  TEST_BEGIN();

  /* print some system information */

  puts( "Init: uname - EFAULT (invalid uts pointer argument)" );
  status = uname( NULL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EFAULT );

  status = uname( &uts );
  rtems_test_assert( !status );
  printf( "Init: uts.sysname: %s\n", uts.sysname );
  printf( "Init: uts.nodename: %s\n", uts.nodename );
  printf( "Init: uts.release: %s\n", uts.release );
  printf( "Init: uts.version: %s\n", uts.version );
  printf( "Init: uts.machine: %s\n", uts.machine );
  puts("");

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init: ID is 0x%08" PRIxpthread_t "\n", Init_id );

  /* exercise get minimum priority */

  priority = sched_get_priority_min( SCHED_FIFO );
  printf( "Init: sched_get_priority_min (SCHED_FIFO) -- %d\n", priority );
  rtems_test_assert( priority != -1 );

  puts( "Init: sched_get_priority_min -- EINVAL (invalid policy)" );
  priority = sched_get_priority_min( -1 );
  rtems_test_assert( priority == -1 );
  rtems_test_assert( errno == EINVAL );

  /* exercise get maximum priority */

  priority = sched_get_priority_max( SCHED_FIFO );
  printf( "Init: sched_get_priority_max (SCHED_FIFO) -- %d\n", priority );
  rtems_test_assert( priority != -1 );

  puts( "Init: sched_get_priority_max -- EINVAL (invalid policy)" );
  priority = sched_get_priority_max( -1 );
  rtems_test_assert( priority == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Init: sched_rr_get_interval -- ESRCH (invalid PID)" );
  status = sched_rr_get_interval( 4, &tr );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ESRCH );

  puts( "Init: sched_rr_get_interval -- EINVAL (invalid interval pointer)" );
  status = sched_rr_get_interval( getpid(), NULL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EINVAL );

  /* print the round robin time quantum */

  status = sched_rr_get_interval( getpid(), &tr );
  printf(
    "Init: Round Robin quantum is %" PRIdtime_t " seconds, %ld nanoseconds\n",
    tr.tv_sec,
    tr.tv_nsec
  );
  rtems_test_assert( !status );

  /* create a thread */

  puts( "Init: pthread_create - SUCCESSFUL" );
  status = pthread_create( &thread_id, NULL, Task_1_through_3, NULL );
  rtems_test_assert( !status );

  /* too may threads error */

  puts( "Init: pthread_create - EAGAIN (too many threads)" );
  status = pthread_create( &thread_id, NULL, Task_1_through_3, NULL );
  rtems_test_assert( status == EAGAIN );

  puts( "Init: sched_yield to Task_1" );
  status = sched_yield();
  rtems_test_assert( !status );

    /* switch to Task_1 */

  /* exit this thread */

  puts( "Init: pthread_exit" );
  pthread_exit( NULL );

    /* switch to Task_1 */

  return NULL; /* just so the compiler thinks we returned something */
}
