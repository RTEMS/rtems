/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>

#include <sys/types.h>
#include <rtems/score/timespec.h>
#include <rtems/score/threadq.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void timespec_divide_by_zero(void);
void timespec_greater_than_lhs_sec_less(void);

void timespec_divide_by_zero(void)
{
  struct timespec      t1;
  struct timespec      zero;
  uint32_t             ival_percentage;
  uint32_t             fval_percentage;

  t1.tv_sec  = 1;
  t1.tv_nsec = 0;

  zero.tv_sec  = 0;
  zero.tv_nsec = 0;

  ival_percentage = 1234;
  fval_percentage = 5678;

  puts( "Init - _Timespec_Divide - by zero" );
  _Timespec_Divide( &t1, &zero, &ival_percentage, &fval_percentage );
  rtems_test_assert( ival_percentage == 0 );
  rtems_test_assert( fval_percentage == 0 );
}

void timespec_greater_than_lhs_sec_less(void)
{
  struct timespec   lhs;
  struct timespec   rhs;
  bool              rc;

  lhs.tv_sec  = 100;
  lhs.tv_nsec = 0;

  rhs.tv_sec  = 101;
  rhs.tv_nsec = 0;

  puts( "Init - _Timespec_Greater_than - lhs seconds less" );
  rc = _Timespec_Greater_than( &lhs, &rhs );
  rtems_test_assert( rc == false );
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  puts( "\n\n*** TEST 58 ***" );

  timespec_divide_by_zero();
  timespec_greater_than_lhs_sec_less();

  puts( "*** END OF TEST 58 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS  1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
