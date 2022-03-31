/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2010.
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
#include "test_support.h"
#include <errno.h>
#include <rtems/libio_.h>
#include <rtems/libcsupport.h>
#include <rtems/malloc.h>

const char rtems_test_name[] = "SPMOUNTMGR 1";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
int fs_mount(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
);

int fs_mount(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
)
{
  return 0;
}

rtems_task Init(
  rtems_task_argument argument
)
{
  int status = 0;
  void *greedy;

  TEST_BEGIN();

  puts( "Init - allocating most of heap -- OK" );
  greedy = rtems_heap_greedy_allocate( NULL, 0 );

  puts( "Init - attempt to register filesystem fs - expect ENOMEM" );
  status = rtems_filesystem_register( "fs", fs_mount );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOMEM );

  puts( "Init - freeing allocated memory -- OK" );
  rtems_heap_greedy_free( greedy );

  puts( "Init - register filesystem fs -- OK" );
  status = rtems_filesystem_register( "fs", fs_mount );
  rtems_test_assert( status == 0 );

  puts( "Init - attempt to make target(NULL) and mount - expect EINVAL" );
  status = mount_and_make_target_path(
             NULL,
             NULL,
             "fs",
             0,
             NULL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Init - attempt to make target and mount - expect EINVAL" );
  status = mount_and_make_target_path(
             NULL,
             "/tmp",
             "fs",
             2,
             NULL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Init - register filesystem fs - expect EINVAL" );
  status = rtems_filesystem_register( "fs", fs_mount );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Init - register filesystem bfs -- OK" );
  status = rtems_filesystem_register( "bfs", fs_mount );
  rtems_test_assert( status == 0 );

  puts( "Init - register filesystem bfs - expect EINVAL" );
  status = rtems_filesystem_register( "bfs", fs_mount );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Init - attempt to unregister with bad args - expect EINVAL" );
  status = rtems_filesystem_unregister( NULL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Init - attempt to unregister fs -- OK" );
  status = rtems_filesystem_unregister( "fs" );
  rtems_test_assert( status == 0 );

  puts( "Init - attempt to unregister fs again - expect ENOENT" );
  status = rtems_filesystem_unregister( "fs" );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOENT );

  puts( "Init - attempt to unregister bfs -- OK" );
  status = rtems_filesystem_unregister( "bfs" );
  rtems_test_assert( status == 0 );

  puts( "Init - attempt to unregister bfs again - expect ENOENT" );
  status = rtems_filesystem_unregister( "bfs" );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOENT );

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
