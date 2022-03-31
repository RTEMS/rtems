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

#include <tmacros.h>

#include <sys/types.h>
#include <rtems/score/timespec.h>

const char rtems_test_name[] = "SP 58";

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
  TEST_BEGIN();

  timespec_divide_by_zero();
  timespec_greater_than_lhs_sec_less();

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS  1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
