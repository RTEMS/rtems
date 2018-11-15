/*
 *  COPYRIGHT (c) 2018.
 *  Shashvat Jain GCI 2018
 *
 *  Permission to use, copy, modify, and/or distribute this software
 *  for any purpose with or without fee is hereby granted.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 *  WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
 *  BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
 *  OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 *  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 *  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
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

const char rtems_test_name[] = "PSXTMMQRCVBLOCK 01";

void *POSIX_Init(void *argument);
void *Middle(void *argument);
void *Low(void *argument);

#define MQ_MAXMSG     1
#define MQ_MSGSIZE    sizeof(int)

mqd_t     queue;
int       message[MQ_MAXMSG];
const char *q_name;

void *POSIX_Init(
  void *argument
)
{
  int        i;
  int        status;
  int        oflag= O_CREAT |O_RDWR;
  pthread_t  threadId;
  struct mq_attr attr;

  attr.mq_maxmsg  = MQ_MAXMSG;
  attr.mq_msgsize = MQ_MSGSIZE;

  TEST_BEGIN();

  for ( i=0 ; i < OPERATION_COUNT - 1 ; i++ ) {
    status = pthread_create( &threadId, NULL, Middle, NULL );
    rtems_test_assert( !status );
  }

  status = pthread_create( &threadId, NULL, Low, NULL );
  rtems_test_assert( !status );

  queue = mq_open( "queue" , oflag , 0x777, &attr );

  /* let other threads run */
  sched_yield();

  /* now run the benchmark */
  benchmark_timer_initialize();
  status = mq_receive( queue, (char *)message , MQ_MSGSIZE, 0 );
  return NULL;
}


void *Middle(
  void *argument
)
{
  sched_yield();

    /* let other threads run */

    (void) mq_receive( queue, (char *)message , MQ_MSGSIZE, 0 );

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
    "mq_receive: not available: block",
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

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     OPERATION_COUNT + 2
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
  /* end of file */
