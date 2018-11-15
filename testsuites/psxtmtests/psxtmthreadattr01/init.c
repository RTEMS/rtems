/*
 *  COPYRIGHT (c) 2018.
 *  Himanshu Sekhar Nayak (GCI 2018)
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

#include <timesys.h>
#include <rtems/btimer.h>
#include "test_support.h"
#include <tmacros.h>
#include <pthread.h>
#include <timesys.h>
#include <sched.h>

#define GUARDSIZE_TEST_VALUE 512
#define STACKSIZE_TEST_VALUE 1024

const char rtems_test_name[] = "PSXTMTHREADATTR01";

/* forward declarations to avoid warnings */
static void *POSIX_Init(void *argument);

static pthread_attr_t attr;
static size_t guardsize = GUARDSIZE_TEST_VALUE;
static size_t stacksize = STACKSIZE_TEST_VALUE;
static struct sched_param param;
static void *stackaddr;

static void benchmark_create_pthread_attr(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
  status = pthread_attr_init(&attr);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_attr_init: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_attr_setdetachstate(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
  status = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_attr_setdetachstate: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_attr_getdetachstate(void)
{
  benchmark_timer_t end_time;
  int  status;
  int detachstate;

  benchmark_timer_initialize();
  status = pthread_attr_getdetachstate(&attr, &detachstate);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );
  rtems_test_assert( detachstate == PTHREAD_CREATE_DETACHED );

  put_time(
    "pthread_attr_getdetachstate: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_attr_setguardsize(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
  status = pthread_attr_setguardsize(&attr, guardsize);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_attr_setguardsize: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_attr_getguardsize(void)
{
  benchmark_timer_t end_time;
  int  status;
  size_t tmp_guardsize;

  benchmark_timer_initialize();
  status = pthread_attr_getguardsize(&attr, &tmp_guardsize);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );
  rtems_test_assert( tmp_guardsize == guardsize );

  put_time(
    "pthread_attr_getguardsize: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_attr_setinheritsched(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
  status = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_attr_setinheritsched: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_attr_getinheritsched(void)
{
  benchmark_timer_t end_time;
  int  status;
  int inheritsched;

  benchmark_timer_initialize();
  status = pthread_attr_getinheritsched(&attr, &inheritsched);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );
  rtems_test_assert( inheritsched == PTHREAD_EXPLICIT_SCHED );

  put_time(
    "pthread_attr_getinheritsched: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_attr_setschedparam(void)
{
  benchmark_timer_t end_time;
  int  status;

  status = pthread_attr_setschedparam(&attr, &param);
  rtems_test_assert( status == 0 );
  param.sched_priority = sched_get_priority_min( SCHED_RR ) + 2;
  benchmark_timer_initialize();
  status = pthread_attr_setschedparam(&attr, &param);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_attr_setschedparam: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_attr_getschedparam(void)
{
  benchmark_timer_t end_time;
  int  status;
  struct sched_param tmp_param;

  status = pthread_attr_getschedparam(&attr, &tmp_param);
  rtems_test_assert( status == 0 );
  tmp_param.sched_priority = sched_get_priority_min( SCHED_RR ) + 2;
  benchmark_timer_initialize();
  status = pthread_attr_getschedparam(&attr, &tmp_param);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );
  rtems_test_assert( tmp_param.sched_priority == param.sched_priority );

  put_time(
    "pthread_attr_getschedparam: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_attr_setschedpolicy(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
  status = pthread_attr_setschedpolicy(&attr, SCHED_RR);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_attr_setschedpolicy: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_attr_getschedpolicy(void)
{
  benchmark_timer_t end_time;
  int  status;
  int policy;

  benchmark_timer_initialize();
  status = pthread_attr_getschedpolicy(&attr, &policy);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );
  rtems_test_assert( policy == SCHED_RR );

  put_time(
    "pthread_attr_getschedpolicy: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_attr_setscope(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
  status = pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_attr_setscope: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_attr_getscope(void)
{
  benchmark_timer_t end_time;
  int  status;
  int scope;

  benchmark_timer_initialize();
  status = pthread_attr_getscope(&attr, &scope);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );
  rtems_test_assert( scope == PTHREAD_SCOPE_PROCESS );

  put_time(
    "pthread_attr_getscope: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_attr_setstack(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
  status = pthread_attr_setstack(&attr, stackaddr, stacksize);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_attr_setstack: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_attr_getstack(void)
{
  benchmark_timer_t end_time;
  int  status;
  size_t tmp_stacksize;

  benchmark_timer_initialize();
  status = pthread_attr_getstack(&attr, &stackaddr, &tmp_stacksize);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );
  rtems_test_assert(tmp_stacksize == stacksize);

  put_time(
    "pthread_attr_getstack: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_destroy_pthread_attr(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
  status = pthread_attr_destroy(&attr);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_attr_destroy: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void *POSIX_Init(
  void *argument
)
{
  TEST_BEGIN();

  benchmark_create_pthread_attr();
  benchmark_pthread_attr_setdetachstate();
  benchmark_pthread_attr_getdetachstate();
  benchmark_pthread_attr_setguardsize();
  benchmark_pthread_attr_getguardsize();
  benchmark_pthread_attr_setinheritsched();
  benchmark_pthread_attr_getinheritsched();
  benchmark_pthread_attr_setschedparam();
  benchmark_pthread_attr_getschedparam();
  benchmark_pthread_attr_setschedpolicy();
  benchmark_pthread_attr_getschedpolicy();
  benchmark_pthread_attr_setscope();
  benchmark_pthread_attr_getscope();
  benchmark_pthread_attr_setstack();
  benchmark_pthread_attr_getstack();
  benchmark_destroy_pthread_attr();

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

/* configure an instance of the object created/destroyed */

#define CONFIGURE_MAXIMUM_POSIX_THREADS     1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
