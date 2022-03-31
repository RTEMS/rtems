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

#include <bsp.h>
#include <tmacros.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/libio.h>

const char rtems_test_name[] = "SPFIFO 4";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

#define SEND_RCV_BUFSIZ 12

rtems_task Init(
  rtems_task_argument not_used
)
{
  int fd = -1;
  int status = -1;
  off_t offset = 4;
  int pipe_length = -1;
  int flag = 1;

  TEST_BEGIN();

  puts( "Init - Creating /fifo" );
  status = mkfifo( "/fifo", 0777 );
  rtems_test_assert( status == 0 );
  
  puts( "Init - Opening /fifo in readonly, non-blocking mode" );
  fd = open( "/fifo", O_RDONLY | O_NONBLOCK );
  rtems_test_assert( fd != -1 );
  
  puts( "Init - Attempt to lseek on fifo -- Expected ESPIPE" );
  offset = lseek( fd, offset, SEEK_CUR );
  rtems_test_assert( offset == -1 );
  rtems_test_assert( errno == ESPIPE );

  puts( "Init - ioctl: FIONBIO -- Expected EFAULT" );
  status = ioctl( fd, FIONBIO, NULL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EFAULT );

  puts( "Init - ioctl: FIONBIO -- OK" );
  status = ioctl( fd, FIONBIO, &flag );
  rtems_test_assert( status == 0 );
 
  flag = 0;
  puts( "Init - ioctl: FIONBIO -- OK" );
  status = ioctl( fd, FIONBIO, &flag );
  rtems_test_assert( status == 0 );

  puts( "Init - ioctl: Dummy Command -- Expected EINVAL" );
  status = ioctl( fd, -1, NULL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Init - ioctl: FIONREAD -- Expected EFAULT" );
  status = ioctl( fd, FIONREAD, NULL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EFAULT );

  puts( "Init - ioctl: FIONREAD -- OK" );
  status = ioctl( fd, FIONREAD, &pipe_length );
  rtems_test_assert( status == 0 );
  rtems_test_assert( pipe_length == 0 );
  
  puts( "Init - closing /fifo" );
  status = close( fd );
  rtems_test_assert( status == 0 );
  
  puts( "Init - removing /fifo" );
  status = unlink( "/fifo" );
  rtems_test_assert( status == 0 );

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 6

#define CONFIGURE_MAXIMUM_TASKS 3

#define CONFIGURE_IMFS_ENABLE_MKFIFO

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
