/*
 * Copyright (c) 2011, 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>

#include <time.h>
#include <string.h>

#include <rtems.h>

const char rtems_test_name[] = "SP 2038";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

#define ASSERT_SC(sc) rtems_test_assert((sc) == RTEMS_SUCCESSFUL)

static const uint32_t sample_seconds [] = {
  571213695UL,
  602836095UL,
  634372095UL,
  665908095UL,
  697444095UL,
  729066495UL,
  760602495UL,
  792138495UL,
  823674495UL,
  855296895UL,
  886832895UL,
  918368895UL,
  949904895UL,
  981527295UL,
  1013063295UL,
  1044599295UL,
  1076135295UL,
  1107757695UL,
  1139293695UL,
  1170829695UL,
  1202365695UL,
  1233988095UL,
  1265524095UL,
  1297060095UL,
  1328596095UL,
  1360218495UL,
  1391754495UL,
  1423290495UL,
  1454826495UL,
  1486448895UL,
  1517984895UL,
  1549520895UL,
  1581056895UL,
  1612679295UL,
  1644215295UL,
  1675751295UL,
  1707287295UL,
  1738909695UL,
  1770445695UL,
  1801981695UL,
  1833517695UL,
  1865140095UL,
  1896676095UL,
  1928212095UL,
  1959748095UL,
  1991370495UL,
  2022906495UL,
  2054442495UL,
  2085978495UL,
  2117600895UL,
  2149136895UL,
  2180672895UL,
  2212208895UL,
  2243831295UL,
  2275367295UL,
  2306903295UL,
  2338439295UL,
  2370061695UL,
  2401597695UL,
  2433133695UL,
  2464669695UL,
  2496292095UL,
  2527828095UL,
  2559364095UL,
  2590900095UL,
  2622522495UL,
  2654058495UL,
  2685594495UL,
  2717130495UL,
  2748752895UL,
  2780288895UL,
  2811824895UL,
  2843360895UL,
  2874983295UL,
  2906519295UL,
  2938055295UL,
  2969591295UL,
  3001213695UL,
  3032749695UL,
  3064285695UL,
  3095821695UL,
  3127444095UL,
  3158980095UL,
  3190516095UL,
  3222052095UL,
  3253674495UL,
  3285210495UL,
  3316746495UL,
  3348282495UL,
  3379904895UL,
  3411440895UL,
  3442976895UL,
  3474512895UL,
  3506135295UL,
  3537671295UL,
  3569207295UL,
  3600743295UL,
  3632365695UL,
  3663901695UL,
  3695437695UL,
  3726973695UL,
  3758596095UL,
  3790132095UL,
  3821668095UL,
  3853204095UL,
  3884826495UL,
  3916362495UL,
  3947898495UL,
  3979434495UL,
  4011056895UL,
  4042592895UL,
  4074128895UL,
  4105664895UL,
  4137200895UL,
  4168736895UL,
  4200272895UL,
  4231808895UL,
  4263431295UL,
  4294967295UL
};

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

static const rtems_time_of_day problem_2100 = {
  .year = 2100,
  .month = 2,
  .day = 28,
  .hour = 0,
  .minute = 0,
  .second = 0
};

static const rtems_time_of_day problem_2100_2 = {
  .year = 2100,
  .month = 2,
  .day = 29,
  .hour = 0,
  .minute = 0,
  .second = 0
};

static void test_tod_to_seconds(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  size_t i = 0;
  size_t n = sizeof(sample_seconds) / sizeof(sample_seconds [0]);

  for (i = 0; i < n; ++i) {
    rtems_time_of_day tod = nearly_problem_2106;
    uint32_t seconds = 0;
    rtems_interval seconds_as_interval = 0;

    tod.year = 1988 + i;
    seconds = _TOD_To_seconds(&tod);
    rtems_test_assert(seconds == sample_seconds [i]);

    sc = rtems_clock_set(&tod);
    ASSERT_SC(sc);

    sc = rtems_clock_get_seconds_since_epoch(&seconds_as_interval);
    ASSERT_SC(sc);
    rtems_test_assert(seconds_as_interval == sample_seconds [i]);
  }
}

static void test_problem_year(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  time_t zero = 0;
  time_t one = 1;
  time_t maybe_negative = zero - one;
  bool time_t_is_32_bit = sizeof(time_t) == 4;
  bool time_t_is_signed = maybe_negative < zero;

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
}

static void test_leap_year(void)
{
    bool test_status;
    const rtems_time_of_day *problem = &problem_2100;
    const rtems_time_of_day *problem2 = &problem_2100_2;
    // 2100 is not a leap year, so it should have 28 days
    test_status = _TOD_Validate(problem);
    rtems_test_assert(test_status == true);
    test_status = _TOD_Validate(problem2);
    rtems_test_assert(test_status == false);
}

static bool test_year_is_leap_year(uint32_t year)
{
    return (((year % 4) == 0) && ((year % 100) != 0)) || ((year % 400) == 0);
}

static void test_every_day(void)
{
    rtems_time_of_day every_day = {
        .year = 1970,
        .month = 1,
        .day = 1,
        .hour = 0,
        .minute = 1,
        .second = 2
    };
    const int days_per_month[2][12] = {
        { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
        { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
    };
    rtems_status_code sc = RTEMS_SUCCESSFUL;
    rtems_time_of_day now;

    for (every_day.year = 1988; every_day.year <= 2100; ++every_day.year) {
        int leap_year = test_year_is_leap_year(every_day.year) ? 1 : 0;
        for (every_day.month = 1; every_day.month <= 12; ++every_day.month) {
            int days = days_per_month[leap_year][every_day.month - 1];
            for (every_day.day = 1; every_day.day <= days; ++every_day.day) {
                sc = rtems_clock_set(&every_day);
                ASSERT_SC(sc);
                sc = rtems_clock_get_tod(&now);
                ASSERT_SC(sc);
                rtems_test_assert(memcmp(&now, &every_day, sizeof(now)) == 0);
            }
        }
    }
}

rtems_task Init(rtems_task_argument argument)
{
  TEST_BEGIN();

  test_tod_to_seconds();
  test_every_day();
  test_problem_year();
  test_leap_year();

  TEST_END();

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
