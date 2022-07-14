/* SPDX-License-Identifier: BSD-2-Clause */

/*
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

#define CONFIGURE_INIT
#include "system.h"

const char rtems_test_name[] = "SP 1";

static RTEMS_NOINIT RTEMS_ALIGNED( RTEMS_TASK_STORAGE_ALIGNMENT ) char
Task_1_storage[
  RTEMS_TASK_STORAGE_SIZE( 2 * RTEMS_MINIMUM_STACK_SIZE, RTEMS_FLOATING_POINT )
];

static const rtems_task_config Task_1_config = {
  .name = rtems_build_name( 'T', 'A', '1', ' ' ),
  .initial_priority = 1,
  .storage_area = Task_1_storage,
  .storage_size = sizeof( Task_1_storage ),
  .maximum_thread_local_storage_size = 0,
  .storage_free = NULL,
  .initial_modes = RTEMS_DEFAULT_MODES,
  .attributes = RTEMS_FLOATING_POINT
};

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_time_of_day time;
  rtems_status_code status;
  rtems_id          id;

  TEST_BEGIN();

  build_time( &time, 12, 31, 1988, 9, 0, 0, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  status = rtems_task_construct( &Task_1_config, &id );
  directive_failed( status, "rtems_task_construct of TA1" );

  status = rtems_task_start( id, Task_1_through_3, 1 );
  directive_failed( status, "rtems_task_start of TA1" );

  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '2', ' ' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE * 2,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create of TA2" );

  status = rtems_task_start( id, Task_1_through_3, 2 );
  directive_failed( status, "rtems_task_start of TA2" );

  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '3', ' ' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE * 3,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create of TA3" );

  status = rtems_task_start( id, Task_1_through_3, 3 );
  directive_failed( status, "rtems_task_start of TA3" );

  rtems_task_exit();
}
