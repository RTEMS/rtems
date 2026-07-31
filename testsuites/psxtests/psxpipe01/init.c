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

#define _GNU_SOURCE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp.h>
#include <pmacros.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <rtems/libcsupport.h>
#include <rtems/libio.h>
#include <rtems/malloc.h>

const char rtems_test_name[] = "PSXPIPE 1";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument ignored);

static int dup_fd_test(int *fd)
{
  int r;

  if ((r = fcntl(*fd, F_DUPFD, 0)) < 0 ||
      fcntl(r, F_SETFD, FD_CLOEXEC) < 0 ||
      fcntl(r, F_SETFL, O_NONBLOCK))
    return -1;
  (void)close(*fd);
  (*fd = r);
  return 0;
}

static int pipe_handler( int fildes[ 2 ] )
{
  fildes[ 0 ] = 0x1234;
  fildes[ 1 ] = 0xabcd;
  return 54321;
}

static int pipe2_handler( int fildes[ 2 ], int flags )
{
  rtems_test_assert( flags == 0x77654321 );
  fildes[ 0 ] = 0xfedc;
  fildes[ 1 ] = 0x5432;
  return -2;
}

static void pipe_handler_test()
{
  int fd[ 2 ] = { 0, 0 };
  int status = 0;

  puts( "" );
  puts( "Pipe handler tests:" );

  puts( " Register handlers" );
  rtems_filesystem_register_pipe( pipe_handler );
  rtems_filesystem_register_pipe2( pipe2_handler );

  puts( " Check pipe handler call" );
  status = pipe( &fd[ 0 ] );
  rtems_test_assert( status == 54321 );
  rtems_test_assert( fd[ 0 ] == 0x1234 );
  rtems_test_assert( fd[ 1 ] == 0xabcd );

  puts( " Check pipe2 handler call" );
  status = pipe2( &fd[ 0 ], 0x77654321 );
  rtems_test_assert( status == -2 );
  rtems_test_assert( fd[ 0 ] == 0xfedc );
  rtems_test_assert( fd[ 1 ] == 0x5432 );
}

static void pipe_default_test()
{
  int   fd[ 2 ] = { 0, 0 };
  int   dummy_fd[ 4 ] = { 0, 0 };
  int   status = 0;
  void *opaque = NULL;
  char c;

  puts( "" );
  puts( "Default pipe tests:" );

  puts( "Init - attempt to create pipe -- expect EFAULT" );
  status = pipe( NULL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EFAULT );

  puts( "Init - create pipe -- OK" );
  status = pipe( fd );
  rtems_test_assert( status == 0 );

  puts( "Init - dup pipe -- OK" );
  status = dup_fd_test(&fd[0]);
  status |= dup_fd_test(&fd[1]);
  rtems_test_assert( status == 0 );

  /* close pipe writer, so reader can return immediately */
  puts( "Init - read pipe -- OK" );
  status = close( fd[1] );
  status |= read(fd[0], &c, 1);
  status |= close( fd[0] );
  rtems_test_assert( status == 0 );

  puts( "Init - create pipe -- OK" );
  status = pipe( fd );
  rtems_test_assert( status == 0 );

  status = close( fd[0] );
  status |= close( fd[1] );
  rtems_test_assert( status == 0 );

  opaque = rtems_heap_greedy_allocate( NULL, 0 );

  /* case where mkfifo fails */
  puts( "Init - attempt to create pipe -- expect ENOMEM" );
  status = pipe( fd );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOMEM );

  rtems_heap_greedy_free( opaque );
  
  dummy_fd[0] = open( "/file01", O_RDONLY | O_CREAT, S_IRWXU );
  rtems_test_assert( dummy_fd[0] != -1 );
  dummy_fd[1] = open( "/file02", O_RDONLY | O_CREAT, S_IRWXU );
  rtems_test_assert( dummy_fd[1] != -1 );
  dummy_fd[2] = open( "/file03", O_RDONLY | O_CREAT, S_IRWXU );
  rtems_test_assert( dummy_fd[2] != -1 );
  dummy_fd[3] = open( "/file04", O_RDONLY | O_CREAT, S_IRWXU );
  rtems_test_assert( dummy_fd[3] != -1 );

  /* case where fifo_open for read => open fails */
  puts( "Init - create pipe -- expect ENFILE" );
  status = pipe( fd );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENFILE );

  status = close( dummy_fd[3] );
  status |= unlink( "/file04" );
  rtems_test_assert( status == 0 );

  /* case where fifo_open for write => open fails */
  puts( "Init - create pipe -- expect ENFILE" );
  status = pipe( fd );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENFILE );

  status = close( dummy_fd[0] );
  status |= unlink( "/file01" );
  status |= close( dummy_fd[1] );
  status |= unlink( "/file02" );
  status |= close( dummy_fd[2] );
  status |= unlink( "/file03" );
  rtems_test_assert( status == 0 );
}

rtems_task Init( rtems_task_argument ignored )
{
  (void) ignored;

  TEST_BEGIN();

  pipe_default_test();
  pipe_handler_test();

  TEST_END();
  rtems_test_exit( 0 );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 7

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_IMFS_ENABLE_MKFIFO

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
