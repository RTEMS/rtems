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
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/libio.h>

const char rtems_test_name[] = "SPFIFO 3";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_task read_task(rtems_task_argument not_used);
void test_main(void);

#define SEND_RCV_BUFSIZ 12
rtems_id Barrier;

rtems_task read_task(rtems_task_argument not_used)
{
  (void) not_used;

  int fd = 0;
  int status = -1;

  char recvBuf_r1[SEND_RCV_BUFSIZ] = {0};
  char recvBuf_r2[SEND_RCV_BUFSIZ] = {0};

  puts("\nRead task activated, waiting till writer opens");

  status = rtems_barrier_wait( Barrier, RTEMS_NO_TIMEOUT );
  rtems_test_assert( status == RTEMS_SUCCESSFUL );

  sleep(1);

  puts("\nNow, reader opening file(1)");  
  fd = open("/tmp/fifo01", O_RDONLY);
  if(fd <= 0) {
    printf("Error opening file: (%d) :: %s", errno, strerror(errno));
    rtems_test_assert(0);
  }

  status = read(fd, recvBuf_r1, sizeof(recvBuf_r1)-1);
  rtems_test_assert(status == sizeof(recvBuf_r1)-1);

  printf("\n@ receiver (being a unblocked reader): Got %s", recvBuf_r1);

  status = close(fd);
  rtems_test_assert(status == 0);

  status = rtems_barrier_wait( Barrier, RTEMS_NO_TIMEOUT );
  rtems_test_assert( status == RTEMS_SUCCESSFUL );
  
  puts("\nReader opening file(2)");
  fd = open("/tmp/fifo01", O_RDONLY);
  if(fd <= 0) {
    printf("Error opening file: (%d) :: %s", errno, strerror(errno));
    rtems_test_assert(0);
  }

  status = read(fd, recvBuf_r2, sizeof(recvBuf_r2)-1);
  rtems_test_assert(status == sizeof(recvBuf_r2)-1);

  printf("\n@ receiver (being a blocked reader): Got %s", recvBuf_r2);

  status = close(fd);
  rtems_test_assert(status == 0);
  
  
  puts("\nReader done!");
  status = rtems_barrier_wait( Barrier, RTEMS_NO_TIMEOUT );
  rtems_test_assert( status == RTEMS_SUCCESSFUL );
  rtems_task_exit();
}

void test_main(void) /* Also acts as the write task */
{

  rtems_id readTaskID;

  rtems_name readTaskName;

  char sendBuf_r1[SEND_RCV_BUFSIZ] = {0};
  char sendBuf_r2[SEND_RCV_BUFSIZ] = {0};
  int status = -1;
  int fd = 0;


  strcpy( sendBuf_r1, "SendBuffer1" );
  strcpy( sendBuf_r2, "SendBuffer2" );

  memset( &Barrier, 0, sizeof(Barrier) );
  status = rtems_barrier_create (
    rtems_build_name ( 'B', 'A', 'R', 't' ),
    RTEMS_BARRIER_AUTOMATIC_RELEASE,
    2,
    &Barrier
    );

  rtems_test_assert( status == RTEMS_SUCCESSFUL );

  TEST_BEGIN();

  puts("\nCreating a task name and a task");
  readTaskName = rtems_build_name('T','A','r',' ');

  status = rtems_task_create(
			     readTaskName,
			     1,
			     RTEMS_MINIMUM_STACK_SIZE * 2,
			     RTEMS_DEFAULT_MODES,
			     RTEMS_DEFAULT_ATTRIBUTES,
			     &readTaskID
			     );
  
  rtems_test_assert( status == RTEMS_SUCCESSFUL );

  puts("\ncreating directory /tmp");
  status = mkdir("/tmp", 0777);
  rtems_test_assert(status == 0);

  puts("\ncreating fifo file /tmp/fifo01");
  status = mkfifo("/tmp/fifo01", 0777);
  rtems_test_assert(status == 0);

  puts("\nStarting the read task");
  status = rtems_task_start(readTaskID, read_task, 0);
  rtems_test_assert(status == 0);

  status = rtems_barrier_wait( Barrier, RTEMS_NO_TIMEOUT );
  rtems_test_assert( status == RTEMS_SUCCESSFUL );

  puts("\nWriter opening file(1)");
  fd = open("/tmp/fifo01", O_WRONLY);
  if(fd <= 0) {
    printf("Error opening file: (%d) :: %s", errno, strerror(errno));
    rtems_test_assert(0);
  }

  
  printf("\n@ sender: %s", sendBuf_r1);
  status = write(fd, sendBuf_r1, sizeof(sendBuf_r1)-1);
  rtems_test_assert(status == sizeof(sendBuf_r1)-1);

  status = close(fd);
  rtems_test_assert(status == 0);
  
  status = rtems_barrier_wait( Barrier, RTEMS_NO_TIMEOUT );
  rtems_test_assert( status == RTEMS_SUCCESSFUL );

  sleep(1);

  // Reader would have blocked by now
  puts("\nWriter opening file(2)");
  fd = open("/tmp/fifo01", O_WRONLY);
  if(fd <= 0) {
    printf("Error opening file: (%d) :: %s", errno, strerror(errno));
    rtems_test_assert(0);
  }

  printf("\n@ sender: %s", sendBuf_r2);
  status = write(fd, sendBuf_r2, sizeof(sendBuf_r2)-1);
  rtems_test_assert(status == sizeof(sendBuf_r2)-1);

  status = close(fd);
  rtems_test_assert(status == 0);

  status = rtems_barrier_wait( Barrier, RTEMS_NO_TIMEOUT );
  rtems_test_assert( status == RTEMS_SUCCESSFUL );

  puts( "Removing the fifo" );
  status = unlink("/tmp/fifo01");
  rtems_test_assert(status == 0);
  
  puts( "Removing /tmp" );
  status = rmdir("/tmp");
  rtems_test_assert(status == 0);
  
  TEST_END();
}

rtems_task Init(
  rtems_task_argument not_used
)
{
  (void) not_used;

  test_main();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 6

#define CONFIGURE_MAXIMUM_TASKS 3

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_BARRIERS 1

#define CONFIGURE_IMFS_ENABLE_MKFIFO

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* end of file */
