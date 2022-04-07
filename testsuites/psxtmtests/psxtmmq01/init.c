/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2013.
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
#include <fcntl.h>
#include <timesys.h>
#include <errno.h>
#include <rtems/btimer.h>
#include "test_support.h"
#include <tmacros.h>
#include <mqueue.h>
#include <signal.h>   /* signal facilities */

const char rtems_test_name[] = "PSXTMMQ 01";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

#define MQ_MAXMSG     1
#define MQ_MSGSIZE    sizeof(int)

mqd_t       queue;
mqd_t       queue2;
const char *q_name;

static void benchmark_mq_open(int printable)
{
  benchmark_timer_t end_time;
  struct mq_attr  attr;

  attr.mq_maxmsg  = MQ_MAXMSG;
  attr.mq_msgsize = MQ_MSGSIZE;
  q_name= "queue";

  benchmark_timer_initialize();
    queue = mq_open( q_name, O_CREAT | O_RDWR , 0x777, &attr );
  end_time = benchmark_timer_read();
  rtems_test_assert( queue != (-1) );

  if (printable == 1)
    put_time(
      "mq_open: first open",
      end_time,
      1,        /* Only executed once */
      0,
      0
    );
}

static void benchmark_mq_open_second(int printable)
{
  benchmark_timer_t end_time;
  struct mq_attr  attr;

  attr.mq_maxmsg  = MQ_MAXMSG;
  attr.mq_msgsize = MQ_MSGSIZE;

  benchmark_timer_initialize();
    queue2 =mq_open( q_name, O_RDONLY | O_CREAT , 0x777, &attr);
  end_time = benchmark_timer_read();
  rtems_test_assert( queue2 != (-1) );

  if (printable == 1)
    put_time(
      "mq_open: second open",
      end_time,
      1,        /* Only executed once */
      0,
      0
    );

}

static void benchmark_mq_close(int printable)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = mq_close(queue);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  if (printable == 1)
    put_time(
      "mq_close: close of first",
      end_time,
      1,        /* Only executed once */
      0,
      0
    );
}

static void benchmark_mq_close_second(int printable)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = mq_close(queue2);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  if (printable == 1)
    put_time(
      "mq_close: close of second",
      end_time,
      1,        /* Only executed once */
      0,
      0
    );
}

static void benchmark_mq_unlink(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = mq_unlink(q_name);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "mq_unlink: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );
}

static void benchmark_mq_notify(void)
{
#if defined(RTEMS_POSIX_API)
  benchmark_timer_t end_time;
  int             status;
  struct sigevent event;

  event.sigev_notify = SIGEV_SIGNAL;
  event.sigev_signo  = SIGUSR1;

  benchmark_timer_initialize();
    status = mq_notify( queue2, &event );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "mq_notify: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );
#endif
}

static void benchmark_mq_send(void)
{
  benchmark_timer_t end_time;
  int  status;

  status = 9;
  benchmark_timer_initialize();
    status = mq_send( queue, (const char *)&status, MQ_MSGSIZE, 1 );
  end_time = benchmark_timer_read();
  rtems_test_assert( status != (-1) );

  put_time(
    "mq_send: no threads waiting",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );
}

static void benchmark_mq_receive(void)
{
  benchmark_timer_t end_time;
  int           status;
  unsigned int  priority;
  int           message[MQ_MAXMSG];
  priority = 1;       /*priority low*/

  benchmark_timer_initialize();
    status = mq_receive( queue2, ( char *)message, MQ_MSGSIZE, &priority );
  end_time = benchmark_timer_read();
  rtems_test_assert( status != (-1) );

  put_time(
    "mq_receive: available",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );
}

static void benchmark_mq_timedsend(void)
{
  benchmark_timer_t end_time;
  int              status;
  struct timespec  timeout;

  status = 5;
  timeout.tv_sec  = 0;
  timeout.tv_nsec = 1;
  benchmark_timer_initialize();
    status = mq_timedsend(
	     queue, (const char *)&status, MQ_MSGSIZE, 1, &timeout);
  end_time = benchmark_timer_read();
  rtems_test_assert( status != (-1) );

  put_time(
    "mq_timedsend: no threads waiting",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );
}

static void benchmark_mq_timedreceive(void)
{
  benchmark_timer_t end_time;
  int              status;
  unsigned int     priority;
  struct timespec  timeout;
  int              message[MQ_MAXMSG];

  priority = 1;       /*priority low*/
  timeout.tv_sec  = 0;
  timeout.tv_nsec = 0;
  benchmark_timer_initialize();
    status = mq_timedreceive(
		  queue2, ( char *)message, MQ_MSGSIZE, &priority, &timeout);
  end_time = benchmark_timer_read();
  rtems_test_assert( status != (-1) );

  put_time(
    "mq_timedreceive: available",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );
}


void *POSIX_Init(
  void *argument
)
{
  TEST_BEGIN();
  /* create the first message queue READWRITE */
  benchmark_mq_open(1);
  /* send message using first queue */
  benchmark_mq_send();
  /* open a second message queue READ ONLY */
  benchmark_mq_open_second(1);
  /* receiving message using the seconde queue */
  benchmark_mq_receive();
  /* closing the second message queue */
  benchmark_mq_close_second(0);
  /* unlinking the first queue */
  benchmark_mq_unlink();
  /* closing the first queue */
  benchmark_mq_close(0);
  /* now repeat basically the same, but for the timed-send/recive */
  /* open the first queue */
  benchmark_mq_open(0);
  /* send message using the first queue */
  benchmark_mq_timedsend();
  /* open a second message queue READ ONLY */
  benchmark_mq_open_second(0);
  /* receiving message using the seconde queue */
  benchmark_mq_timedreceive();
  /* calling notify */
  benchmark_mq_notify();
  /* closing the second message queue */
  benchmark_mq_close_second(1);
  /* closing the first queue */
  benchmark_mq_close(1);

 TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE
#define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES  2

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
