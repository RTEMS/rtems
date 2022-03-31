/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  Extension create fails
 */

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

const char rtems_test_name[] = "SP 56";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
bool task_create(Thread_Control *executing, Thread_Control *created);

bool task_create(
  Thread_Control *executing,
  Thread_Control *created
)
{
  return false;
}

rtems_extensions_table Extensions = {
  task_create,               /* task create user extension */
  NULL,                      /* task start user extension */
  NULL,                      /* task restart user extension */
  NULL,                      /* task delete user extension */
  NULL,                      /* task switch user extension */
  NULL,                      /* task begin user extension */
  NULL,                      /* task exitted user extension */
  NULL                       /* fatal error user extension */
};

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code    status;
  rtems_id             extension;
  rtems_id             task_id;

  TEST_BEGIN();

  puts( "Init - rtems_extension_create - OK" );
  status = rtems_extension_create(
    rtems_build_name( 'E', 'X', 'T', ' ' ),
    &Extensions,
    &extension
  );
  directive_failed( status, "rtems_extension_create" );

  puts( "Init - rtems_task_create - create extension fails - UNSATISFIED" );
  status = rtems_task_create(
     rtems_build_name( 'T', 'A', '1', ' ' ),
     1,
     RTEMS_MINIMUM_STACK_SIZE,
     RTEMS_TIMESLICE,
     RTEMS_FLOATING_POINT,
     &task_id
  );
  fatal_directive_status( status, RTEMS_UNSATISFIED, "rtems_task_create" );

  puts( "Init - rtems_extension_delete - OK" );
  status = rtems_extension_delete( extension );
  directive_failed( status, "rtems_extension_delete" );
  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             2
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS   1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
