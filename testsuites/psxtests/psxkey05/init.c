/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Copyright (c) 2012 Zhongwei Yao.
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

#include <pthread.h>
#include <errno.h>
#include "tmacros.h"
#include "pmacros.h"

const char rtems_test_name[] = "PSXKEY 5";

/* forward declarations to avoid warnings */
rtems_task Init( rtems_task_argument ignored );

rtems_task Init( rtems_task_argument ignored )
{
  pthread_key_t    key1, key2;
  int              sc, *value;
  int Data_array[2] = {1, 2};

  TEST_BEGIN();

  puts( "Init - pthread key1 create - OK" );
  sc = pthread_key_create( &key1, NULL );
  rtems_test_assert( !sc );

  puts( "Init - pthread key2 create - OK" );
  sc = pthread_key_create( &key2, NULL );
  rtems_test_assert( !sc );

  puts( "Init - key1 pthread_setspecific - OK" );
  sc = pthread_setspecific( key1, &Data_array[0] );
  rtems_test_assert( !sc );

  puts( "Init - key2 pthread_setspecific - OK" );
  sc = pthread_setspecific( key2, &Data_array[1] );
  rtems_test_assert( !sc );

  puts( "Init - key1 pthread_getspecific - OK" );
  value = pthread_getspecific( key1 );
  rtems_test_assert( *value == Data_array[0] );

  puts( "Init - key2 pthread_getspecific - OK" );
  value = pthread_getspecific( key2 );
  rtems_test_assert( *value == Data_array[1] );

  puts( "Init - key1 pthread_setspecific - OK" );
  sc = pthread_setspecific( key1, &Data_array[1] );
  rtems_test_assert( !sc );

  puts( "Init - key1 pthread_getspecific - OK" );
  value = pthread_getspecific( key1 );
  rtems_test_assert( *value == Data_array[1] );

  puts( "Init - pthread key1 delete - OK" );
  sc = pthread_key_delete( key1 );
  rtems_test_assert( sc == 0 );

  puts( "Init - pthread key2 delete - OK" );
  sc = pthread_key_delete( key2 );
  rtems_test_assert( sc == 0 );

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_TASKS          1
#define CONFIGURE_MAXIMUM_POSIX_KEYS     2

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE


#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
