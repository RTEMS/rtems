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

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <tmacros.h>

#include <rtems/score/objectimpl.h>

const char rtems_test_name[] = "SP 41";

static bool visitor( Thread_Control *thread, void *arg )
{
  int *counter = arg;
  ++( *counter );
  return false;
}

static void iterator( Thread_Control *thread )
{
}

static rtems_task Init(
  rtems_task_argument ignored
)
{
  void *tmp;
  int   counter;

  TEST_BEGIN();

  puts( "Init - overwrite internal value to trip case" );
  tmp = _Objects_Information_table[ OBJECTS_CLASSIC_API ][ 1 ];
  _Objects_Information_table[ OBJECTS_CLASSIC_API ][ 1 ] = NULL;

  puts( "Init - rtems_task_iterate - NULL table" );
  counter = 0;
  rtems_task_iterate( visitor, &counter );
  _Objects_Information_table[ OBJECTS_CLASSIC_API ][ 1 ] = tmp;
  rtems_test_assert( counter == 1 );

  puts( "Init - rtems_task_iterate - normal" );
  counter = 0;
  rtems_task_iterate( visitor, &counter );
  rtems_test_assert( counter == 2 );

  puts( "Init - rtems_iterate_over_all_threads - NULL" );
  rtems_iterate_over_all_threads( NULL );

  puts( "Init - rtems_iterate_over_all_threads - iterator" );
  rtems_iterate_over_all_threads( iterator );

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS         1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
