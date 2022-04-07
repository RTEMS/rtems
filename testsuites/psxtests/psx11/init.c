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

#define CONFIGURE_INIT
#include "system.h"
#include <errno.h>

const char rtems_test_name[] = "PSX 11";

void *POSIX_Init(
  void *argument
)
{
  int                  status;
  struct sched_param   param;
  pthread_attr_t       attr;
  int                  priority_1;
  int                  priority_2;
  int                  priority_3;
  int                  priority_4;

  TEST_BEGIN();

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08" PRIxpthread_t "\n", Init_id );

  /* exercise pthread_setschedparam */

  priority_1 = sched_get_priority_max( SCHED_FIFO );      /* was 127 */
  priority_2 = sched_get_priority_max( SCHED_FIFO ) - 2;  /* was 125 */
  priority_3 = sched_get_priority_max( SCHED_FIFO ) - 6;  /* was 121 */
  priority_4 = sched_get_priority_max( SCHED_FIFO ) - 7;  /* was 120 */

  param.sched_priority = priority_1;

  printf(
    "Init: Setting scheduling parameters to FIFO with priority %d\n",
    priority_1
  );
  status = pthread_setschedparam( Init_id, SCHED_FIFO, &param );
  rtems_test_assert( !status );

  param.sched_priority = priority_2;

  printf(
    "Init: Setting scheduling parameters to RR with priority %d\n",
    priority_2
  );
  status = pthread_setschedparam( Init_id, SCHED_RR, &param );
  rtems_test_assert( !status );

  param.sched_priority = priority_3;

  printf(
    "Init: Setting scheduling parameters to OTHER with priority %d\n",
    priority_3
  );
  status = pthread_setschedparam( Init_id, SCHED_OTHER, &param );
  rtems_test_assert( !status );

  /* create a thread as SCHED_FIFO */

  printf(
    "Init: create a thread of SCHED_FIFO with priority %d\n", priority_4 );
  status = pthread_attr_init( &attr );
  rtems_test_assert( !status );

  attr.schedpolicy = SCHED_FIFO;
  attr.schedparam.sched_priority = priority_4;

  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  rtems_test_assert( !status );

  puts( "Init: join with the other thread" );
  status = pthread_join( Task_id, NULL );
  rtems_test_assert( !status );

  /* create a thread as SCHED_RR */

  printf( "Init: create a thread of SCHED_RR with priority %d\n", priority_4 );
  status = pthread_attr_init( &attr );
  rtems_test_assert( !status );

  status = pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED );
  rtems_test_assert( !status );
  attr.schedpolicy = SCHED_RR;
  attr.schedparam.sched_priority = priority_4;

  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  rtems_test_assert( !status );

  puts( "Init: join with the other thread" );
  status = pthread_join( Task_id, NULL );
  rtems_test_assert( !status );

  /* create a thread as SCHED_OTHER */

  printf(
    "Init: create a thread of SCHED_OTHER with priority %d\n", priority_4 );
  status = pthread_attr_init( &attr );
  rtems_test_assert( !status );

  attr.schedpolicy = SCHED_OTHER;
  attr.schedparam.sched_priority = priority_4;

  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  rtems_test_assert( !status );

  puts( "Init: join with the other thread" );
  status = pthread_join( Task_id, NULL );
  rtems_test_assert( !status );

  TEST_END();
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
