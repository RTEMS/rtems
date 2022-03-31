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
#include <limits.h>

#include <rtems.h>
#include <rtems/libio.h>

const char rtems_test_name[] = "SPFIFO 5";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_task read_task(rtems_task_argument not_used);

#define SEND_RCV_BUFSIZ       (PIPE_BUF+20)

rtems_id rBarrier;
rtems_id wBarrier;

char recvBuf[SEND_RCV_BUFSIZ] = {0};
char sendBuf[SEND_RCV_BUFSIZ] = {0};

rtems_task read_task(rtems_task_argument not_used)
{
  int fd = 0;
  int status = -1;
  uint32_t released = 0;

  puts( "read_task - opening /fifo in non-blocking mode" );

  fd = open( "/fifo", O_RDONLY | O_NONBLOCK );
  if( fd <= 0 ) {
    printf( "Error opening file: (%d) :: %s\n", errno, strerror( errno ) );
    rtems_test_assert( 0 );
  }

  puts( "read_task - attempt to read with number of writers = 0" );
  status = read( fd, recvBuf, SEND_RCV_BUFSIZ );
  rtems_test_assert( status == 0 );

  puts( "read_task - releasing the write task" );
  status = rtems_barrier_release( wBarrier, &released );
  rtems_test_assert( status == 0 );

  puts( "read_task - waiting at a barrier" );
  status = rtems_barrier_wait( rBarrier, RTEMS_NO_TIMEOUT );
  rtems_test_assert( status == RTEMS_SUCCESSFUL );

  puts( "read_task - attempting to read from the fifo -- Expect EAGAIN" );
  status = read( fd, recvBuf, SEND_RCV_BUFSIZ );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EAGAIN );

  puts( "read_task - closing /fifo" );
  status = close( fd );
  rtems_test_assert( status == 0 );

  puts("read_task - opening /fifo in read-only mode");
  fd = open( "/fifo", O_RDONLY );
  if( fd <= 0 ) {
    printf( "Error opening file: (%d) :: %s\n", errno, strerror( errno ) );
    rtems_test_assert( 0 );
  }

  puts( "read_task - releasing the write task" );
  status = rtems_barrier_release( wBarrier, &released );
  rtems_test_assert( status == 0 );

  puts( "read_task - reading from /fifo - OK" );
  status = read( fd, recvBuf, 10 );
  rtems_test_assert( status == 10 );

  puts( "read_task - reading from /fifo - OK" );
  status = read( fd, recvBuf+10, 10 );
  rtems_test_assert( status == 10 );

  puts( "read_task - waiting at a barrier" );
  status = rtems_barrier_wait( rBarrier, RTEMS_NO_TIMEOUT );
  rtems_test_assert( status == RTEMS_SUCCESSFUL );

  puts( "read_task - reading from /fifo - OK" );
  status = read( fd, recvBuf+20, SEND_RCV_BUFSIZ - 20 );
  rtems_test_assert( status == SEND_RCV_BUFSIZ - 20 );

  if( strcmp( sendBuf, recvBuf ) ) {
    puts( "Sent, received buffer do not match" );
    rtems_test_assert( 0 );
  }

  puts( "read_task - releasing the write task" );
  status = rtems_barrier_release( wBarrier, &released );
  rtems_test_assert( status == 0 );

  puts( "read_task - waiting at a barrier" );
  status = rtems_barrier_wait( rBarrier, RTEMS_NO_TIMEOUT );
  rtems_test_assert( status == RTEMS_SUCCESSFUL );

  status = close( fd );
  rtems_test_assert( status == 0 );

  puts( "read_task - releasing the write task" );
  status = rtems_barrier_release( wBarrier, &released );
  rtems_test_assert( status == 0 );

  rtems_task_exit();
}

rtems_task Init(
  rtems_task_argument not_used
)
{
  rtems_id readTaskID;

  rtems_name readTaskName;

  int status = -1;
  int fd = 0;
  uint32_t released = 0;
  int flag = 1;

  TEST_BEGIN();

  memset( sendBuf, '1', SEND_RCV_BUFSIZ - 30 );
  memset( sendBuf, '2', 29 );
  sendBuf[SEND_RCV_BUFSIZ-1] = 0;

  memset( &rBarrier, 0, sizeof(rBarrier) );
  status = rtems_barrier_create (
    rtems_build_name ( 'B', 'A', 'R', 'r' ),
    RTEMS_BARRIER_MANUAL_RELEASE,
    2,
    &rBarrier
    );

  rtems_test_assert( status == RTEMS_SUCCESSFUL );

  memset( &wBarrier, 0, sizeof( wBarrier ) );
  status = rtems_barrier_create (
    rtems_build_name ( 'B', 'A', 'R', 'w' ),
    RTEMS_BARRIER_MANUAL_RELEASE,
    2,
    &wBarrier
    );
  rtems_test_assert( status == RTEMS_SUCCESSFUL );

  readTaskName = rtems_build_name( 'T','A','r',' ' );
  status = rtems_task_create(
			     readTaskName,
			     1,
			     RTEMS_MINIMUM_STACK_SIZE * 2,
			     RTEMS_DEFAULT_MODES,
			     RTEMS_DEFAULT_ATTRIBUTES,
			     &readTaskID
			     );
  
  rtems_test_assert( status == RTEMS_SUCCESSFUL );

  puts( "Init - Creating fifo file /fifo" );
  status = mkfifo( "/fifo", 0777 );
  rtems_test_assert( status == 0 );

  puts( "Init - starting the read task" );
  status = rtems_task_start( readTaskID, read_task, 0 );
  rtems_test_assert( status == 0 );

  puts( "Init - waiting at a barrier" );
  status = rtems_barrier_wait( wBarrier, RTEMS_NO_TIMEOUT );
  rtems_test_assert( status == RTEMS_SUCCESSFUL );

  puts("Init - opening the fifo in write only mode -- OK");
  fd = open("/fifo", O_WRONLY);
  if(fd <= 0) {
    printf("Error opening file: (%d) :: %s\n", errno, strerror(errno));
    rtems_test_assert(0);
  }

  puts( "Init - writing to /fifo" );
  status = write(fd, sendBuf, 0 );
  rtems_test_assert( status == 0 );

  puts( "Init - releasing the read_task" );
  status = rtems_barrier_release( rBarrier, &released );
  rtems_test_assert( status == 0 );

  puts( "Init - waiting at a barrier" );
  status = rtems_barrier_wait( wBarrier, RTEMS_NO_TIMEOUT );
  rtems_test_assert( status == RTEMS_SUCCESSFUL );
  
  puts( "Init - writing to /fifo - OK" );
  status = write(fd, sendBuf, SEND_RCV_BUFSIZ - 30 );
  rtems_test_assert( status == SEND_RCV_BUFSIZ - 30 );

  puts( "Init - writing to /fifo - OK" );
  status = write(fd, sendBuf+SEND_RCV_BUFSIZ - 30, 30 );
  rtems_test_assert( status == 30 );

  puts( "Init - releasing the read_task" );
  status = rtems_barrier_release( rBarrier, &released );
  rtems_test_assert( status == 0 );

  puts( "Init - waiting at a barrier" );
  status = rtems_barrier_wait( wBarrier, RTEMS_NO_TIMEOUT );
  rtems_test_assert( status == RTEMS_SUCCESSFUL );  

  puts( "Init - converting the write-mode to non-block" );
  status = ioctl( fd, FIONBIO, &flag );
  rtems_test_assert( status == 0 );

  puts( "Init - writing to /fifo - OK" );
  status = write(fd, sendBuf, SEND_RCV_BUFSIZ - 30 );
  rtems_test_assert( status == SEND_RCV_BUFSIZ - 30 );
  
  puts( "Init - writing to /fifo - Expect EAGAIN" );
  status = write(fd, sendBuf, SEND_RCV_BUFSIZ - 30 );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EAGAIN );

  puts( "Init - releasing the read_task" );
  status = rtems_barrier_release( rBarrier, &released );
  rtems_test_assert( status == 0 );

  puts( "Init - waiting at a barrier" );
  status = rtems_barrier_wait( wBarrier, RTEMS_NO_TIMEOUT );
  rtems_test_assert( status == RTEMS_SUCCESSFUL );  

  puts( "Init - writing to /fifo - Expect EPIPE" );
  status = write(fd, sendBuf, SEND_RCV_BUFSIZ - 30 );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EPIPE );

  status = close( fd );
  rtems_test_assert( status == 0 );

  puts( "Removing the fifo" );
  status = unlink("/fifo");
  rtems_test_assert(status == 0);
  
  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 6

#define CONFIGURE_MAXIMUM_TASKS 3

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_BARRIERS 2

#define CONFIGURE_IMFS_ENABLE_MKFIFO

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* end of file */
