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

#include <pmacros.h>
#include <unistd.h>
#include <errno.h>
#include <tmacros.h>

#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>

#include "test_support.h"

const char rtems_test_name[] = "PSXMSGQ 4";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

void *POSIX_Init(
  void *argument
)
{
  (void) argument;

  struct mq_attr          attr;
  mqd_t                   Queue, second_Queue;
  int                     sc;
  Heap_Information_block  start;
  size_t                  to_alloc;
  void                   *alloced;
  bool                    sb;
  const char             *name;

  TEST_BEGIN();

  attr.mq_maxmsg = 1;
  attr.mq_msgsize = sizeof(int);

  puts( "Init - Open message queue instance 1" );
  Queue = mq_open( "Queue", O_CREAT | O_RDWR, 0x777, &attr );
  if ( Queue == (-1) )
    perror( "mq_open failed" );
  rtems_test_assert( Queue != (-1) );

  puts( "Init - Open message queue instance 2 - FAIL - ENFILE " );
  second_Queue = mq_open( "Queue2", O_CREAT | O_RDWR, 0x777, &attr );
  if ( second_Queue != (-1) )
    puts( "mq_open did not failed" );
  rtems_test_assert( second_Queue == (-1) );
  rtems_test_assert( errno == ENFILE );

  puts( "Init - Unlink message queue instance 1" );
  sc = mq_unlink( "Queue" );
  if ( sc != 0 )
    perror( "mq_unlink failed" );
  rtems_test_assert( sc == 0 );

  puts( "Init - Close message queue instance 1" );
  sc = mq_close( Queue );
  if ( sc != 0 )
    perror( "mq_close failed" );
  rtems_test_assert( sc == 0 );

  sb = rtems_workspace_get_information( &start );
  rtems_test_assert( start.Free.number == 1 );
  to_alloc = start.Free.largest;

  /* find the largest we can actually allocate */
  while ( 1 ) {
    sb = rtems_workspace_allocate( to_alloc, &alloced );
    if ( sb )
      break;
    to_alloc -= 4;
  }

  rtems_workspace_free( alloced );

  /*
   * Now do the test
   */
  puts( "Init - Memory allocation error test" );

  name = Get_Longest_Name();
  do {
    sb = rtems_workspace_allocate( to_alloc, &alloced );
    rtems_test_assert( sb );

    second_Queue = mq_open(name,O_CREAT | O_RDWR, 0x777, &attr );

    /* free the memory we snagged, then check the status */
    rtems_workspace_free( alloced );

    to_alloc -= 4;
  } while ( second_Queue == -1 );

  puts( "Init - Message Queue created" );

  puts( "Init - Unlink message queue" );
    sc = mq_unlink( name );
    if ( sc != 0 )
      perror( "mq_unlink failed" );
    rtems_test_assert( sc == 0 );

  puts( "Init - Close message queue" );
    sc = mq_close( second_Queue );
    if ( sc != 0 )
      perror( "mq_close failed" );
    rtems_test_assert( sc == 0 );

  TEST_END();
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

/* account for message buffers and string names */
#define CONFIGURE_MESSAGE_BUFFER_MEMORY \
    (2 * CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(1, sizeof(int)))

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS                   1
#define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES            1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
