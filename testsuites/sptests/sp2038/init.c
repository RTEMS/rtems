/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>

#include <time.h>
#include <string.h>

#include <rtems.h>

#define TEST_APPLICABLE \
  (CPU_TIMESTAMP_USE_INT64 == TRUE || CPU_TIMESTAMP_USE_INT64_INLINE == TRUE)

#define ASSERT_SC(sc) rtems_test_assert((sc) == RTEMS_SUCCESSFUL)

static const rtems_time_of_day nearly_problem_2038 = {
  .year = 2038,
  .month = 1,
  .day = 19,
  .hour = 3,
  .minute = 14,
  .second = 7
};

static const rtems_time_of_day problem_2038 = {
  .year = 2038,
  .month = 1,
  .day = 19,
  .hour = 3,
  .minute = 14,
  .second = 8
};

static const rtems_time_of_day nearly_problem_2106 = {
  .year = 2106,
  .month = 2,
  .day = 7,
  .hour = 6,
  .minute = 28,
  .second = 15
};

static const rtems_time_of_day problem_2106 = {
  .year = 2106,
  .month = 2,
  .day = 7,
  .hour = 6,
  .minute = 28,
  .second = 16
};

static void test_case(void)
{
#if TEST_APPLICABLE
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  int64_t one = 1;
  int64_t large = one << 32;
  time_t time_t_large = (time_t) large;
  bool time_t_is_32_bit = time_t_large != large;
  bool time_t_is_signed = (((time_t) 0) - ((time_t) 1)) < 0;

  if (time_t_is_32_bit) {
    const rtems_time_of_day *nearly_problem = NULL;
    const rtems_time_of_day *problem = NULL;
    rtems_time_of_day now;

    if (time_t_is_signed) {
      nearly_problem = &nearly_problem_2038;
      problem = &problem_2038;
    } else {
      nearly_problem = &nearly_problem_2106;
      problem = &problem_2106;
    }

    sc = rtems_clock_set(nearly_problem);
    ASSERT_SC(sc);
    sc = rtems_clock_get_tod(&now);
    ASSERT_SC(sc);
    rtems_test_assert(memcmp(&now, nearly_problem, sizeof(now)) == 0);

    sc = rtems_clock_set(problem);
    ASSERT_SC(sc);
    sc = rtems_clock_get_tod(&now);
    ASSERT_SC(sc);
    rtems_test_assert(memcmp(&now, problem, sizeof(now)) == 0);
  }
#endif /* TEST_APPLICABLE */
}

rtems_task Init(rtems_task_argument argument)
{
  puts("\n\n*** TEST 2038 ***");

  test_case();

  puts("*** END OF TEST 2038 ***");

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
