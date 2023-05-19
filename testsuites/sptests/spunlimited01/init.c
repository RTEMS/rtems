/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2022 embedded brains GmbH & Co. KG
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

#include <rtems.h>
#include <rtems/score/heapimpl.h>

#include <string.h>

#include <rtems/test.h>
#include <rtems/test-info.h>

const char rtems_test_name[] = "SPUNLIMITED 1";

void __real__Workspace_Free( void * );
void __wrap__Workspace_Free( void * );

void __wrap__Workspace_Free( void *ptr )
{
  uintptr_t alloc_begin;
  uintptr_t block_size;
  uintptr_t block_end;
  Heap_Block *block;

  if ( ptr == NULL ) {
    return;
  }

  alloc_begin = (uintptr_t) ptr;
  block = _Heap_Block_of_alloc_area( alloc_begin, CPU_HEAP_ALIGNMENT );
  block_size = _Heap_Block_size( block );
  block_end = (uintptr_t) block + block_size;
  memset( ptr, 0, block_end - alloc_begin );

  __real__Workspace_Free( ptr );
}

static void delete_mutex( rtems_id id )
{
  rtems_status_code sc;

  sc = rtems_semaphore_delete( id );
  T_rsc_success( sc );
}

T_TEST_CASE( UnlimitedShrink )
{
  rtems_id ids[ 6 ];
  size_t i;
  rtems_status_code sc;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ids ); ++i ) {
    sc = rtems_semaphore_create(
      rtems_build_name( 'M', 'U', 'T', 'X' ),
      1,
      RTEMS_BINARY_SEMAPHORE,
      0,
      &ids[ i ]
    );
    T_assert_rsc_success( sc );
  }

  sc = rtems_semaphore_obtain( ids[ 2 ], RTEMS_WAIT, RTEMS_NO_TIMEOUT );
  T_rsc_success( sc );

  delete_mutex( ids[ 5 ] );
  delete_mutex( ids[ 4 ] );
  delete_mutex( ids[ 3 ] );

  sc = rtems_semaphore_release( ids[ 2 ] );
  T_rsc_success( sc );

  delete_mutex( ids[ 2 ] );
  delete_mutex( ids[ 1 ] );
  delete_mutex( ids[ 0 ] );
}

static rtems_task Init( rtems_task_argument argument )
{
  rtems_test_run( argument, TEST_STATE );
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_MAXIMUM_SEMAPHORES rtems_resource_unlimited( 2 )

#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
