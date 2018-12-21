/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2018, Himanshu Sekhar Nayak
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

#include <fcntl.h>
#include <timesys.h>
#include <rtems/btimer.h>
#include <tmacros.h>
#include "test_support.h"
#include <sched.h>
#include <pthread.h>
#include <mqueue.h>
#include <signal.h>

const char rtems_test_name[] = "PSXTMMQRCVBLOCK 02";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *Middle(void *argument);
void *Low(void *argument);

#define MQ_MAXMSG     1
#define MQ_MSGSIZE    sizeof(int)

static mqd_t queue;
int message[MQ_MAXMSG];
const char *q_name;
unsigned int priority;
struct timespec timeout;

void *POSIX_Init(
  void *argument
)
{
  int              i;
  int              status;
  int              oflag = O_CREAT |O_RDWR;
  pthread_t        threadId;
  struct mq_attr   attr;

  priority = 0;
  timeout.tv_sec  = 0;
  timeout.tv_nsec = 0;
  attr.mq_maxmsg  = MQ_MAXMSG;
  attr.mq_msgsize = MQ_MSGSIZE;

  TEST_BEGIN();

  for ( i=0 ; i < OPERATION_COUNT - 1 ; i++ ) {
    status = pthread_create( &threadId, NULL, Middle, NULL );
    rtems_test_assert( !status );
  }

  status = pthread_create( &threadId, NULL, Low, NULL );
  rtems_test_assert( !status );

  queue = mq_open( "queue", oflag, 0x777, &attr );

  /* let other threads run */
  sched_yield();

  /* now run the benchmark */
  benchmark_timer_initialize();
  status = mq_timedreceive(
		  queue, (char *)message, MQ_MSGSIZE, &priority, &timeout);
  return NULL;
}

void *Middle(
  void *argument
)
{
  sched_yield();

    /* let other threads run */

    (void) mq_timedreceive(
      queue, (char *)message, MQ_MSGSIZE, &priority, &timeout);

  return NULL;
}

void *Low(
  void *argument
)
{
  benchmark_timer_t end_time;

  sched_yield();

  end_time = benchmark_timer_read();

  put_time(
    "mq_timedreceive: not available: block",
    end_time,
    OPERATION_COUNT,
    0,
    0
  );

  TEST_END();

  rtems_test_exit( 0 );
  return NULL;
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     OPERATION_COUNT + 2
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
  /* end of file */
