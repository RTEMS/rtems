/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

/*
 * This test program runs also on GNU/Linux and FreeBSD.  Use
 *
 * cc init.c tls.c && ./a.out
 *
 * to run it.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sptls04.h"

#ifdef __rtems__

#include <tmacros.h>

#else /* __rtems__ */

#include <assert.h>

#define rtems_test_assert(x) assert(x)

#endif /* __rtems__ */

static _Thread_local int i;

static _Thread_local int j;

static __attribute__((__constructor__)) void con(void)
{
  i = 1;
}

static void test(void)
{
  rtems_test_assert(i == 1);
  rtems_test_assert(j == 0);
  rtems_test_assert(k == 2);
}

#ifdef __rtems__

const char rtems_test_name[] = "SPTLS 4";

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();
  test();
  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

#else /* __rtems__ */

int main(void)
{
  test();
  return 0;
}

#endif /* __rtems__ */
