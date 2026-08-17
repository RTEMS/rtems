/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2026 Yang Zhang
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
#include <string.h>
#include <rtems/malloc.h>
#include <rtems/rtl/rap.h>

const char rtems_test_name[] = "libdl (RAP) 14";

static rtems_task Init( rtems_task_argument argument )
{
  char  error_message[ 64 ];
  void *greedy;
  bool  loaded;
  int   error;

  (void) argument;

  TEST_BEGIN();

  puts( "Init - allocating most of heap -- OK" );
  greedy = rtems_heap_greedy_allocate( NULL, 0 );

  puts( "Init - attempt to load RAP app - expect failure" );
  loaded = rtems_rap_load( "dummy.rap", 0, 0, NULL );
  rtems_test_assert( loaded == false );

  error = rtems_rap_get_error( error_message, sizeof( error_message ) );
  rtems_test_assert( error == ENOMEM );

  puts( "Init - freeing allocated memory -- OK" );
  rtems_heap_greedy_free( greedy );

  TEST_END();
  rtems_test_exit( 0 );
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_MAXIMUM_TASKS        1
#define CONFIGURE_INITIAL_EXTENSIONS   RTEMS_TEST_INITIAL_EXTENSION
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_INIT
#include <rtems/confdefs.h>
