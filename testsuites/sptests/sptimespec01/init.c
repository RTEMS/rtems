/*
 *  Copyright (c) 2012.
 *  Krzysztof Miesowicz <krzysztof.miesowicz@gmail.com>
 *  
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <tmacros.h>
#include "test_support.h"
#include <rtems/timespec.h>
#include <rtems/score/todimpl.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void test_add(void);
void test_divide(void);
void test_divide_by_integer(void);
void test_compare(void);
void test_validity(void);
void test_subtract(void);
void test_convert(void);

struct timespec *timespec1;
struct timespec *timespec2;
struct timespec *tpointer;
struct timespec ts1;
struct timespec ts2;
struct timespec notvalid={-20,-50}; /* set very invalid timespec */
bool result;

rtems_task Init(
  rtems_task_argument argument
)
{
  timespec1=&ts1;
  timespec2=&ts2;

  puts( "\n\n*** TEST sptimespec01 ***" );

  test_add();
  test_divide();
  test_divide_by_integer();
  test_compare();
  test_validity();
  test_subtract();
  test_convert();

  puts( "\n*** END OF TEST sptimespec01 ***" );

  rtems_test_exit(0);
}

void test_add(){
/* Basic add_to test. tv_nsec in result is in range 
 * (less than TOD_NANOSECONDS_PER_SECOND) */
  rtems_timespec_set(timespec1, 13, 300);
  rtems_timespec_set(timespec2, 26, 5000);
  rtems_timespec_add_to(timespec1, timespec2);
  rtems_test_assert( (timespec1->tv_sec==39)&&(timespec1->tv_nsec==5300) );
/* Another case. tv_nsec in result is greater than TOD_NANOSECONDS_PER_SECOND */
  rtems_timespec_set(timespec1, 13, (TOD_NANOSECONDS_PER_SECOND-300));
  rtems_timespec_set(timespec2, 26, 5000);
  rtems_timespec_add_to(timespec1, timespec2);
  rtems_test_assert( (timespec1->tv_sec==40)&&(timespec1->tv_nsec==4700) );

  puts( "\n rtems_timespec_add_to test PASSED " );
}

void test_divide(){
/* RTEMS_TIMESPEC_DIVIDE TEST -PART1- divide by non-zero timespec */
  uint32_t ival_percentage, fval_percentage;
  rtems_timespec_set(timespec1, 20, 5000);
  rtems_timespec_set(timespec2, 61, 5000);
  rtems_timespec_divide(
    timespec1,
    timespec2,
    &ival_percentage,
    &fval_percentage
  );
  rtems_test_assert( (ival_percentage==32) || (fval_percentage=786) );
  puts( " rtems_timespace_divide test -part1- divide by non-zero PASSED" );


/* RTEMS_TIMESPEC_DIVIDE TEST -PART2- divide by zero */
  rtems_timespec_zero(timespec2);
  rtems_timespec_divide(
    timespec1,
    timespec2, 
    &ival_percentage, 
    &fval_percentage
  );
  rtems_test_assert( (ival_percentage==0) && (fval_percentage==0) );
  puts( " rtems_timespace_divide test -part2- divide by zero PASSED" );
}

void test_divide_by_integer(){
/* RTEMS_TIMESPEC_DIVIDE_BY_INTEGER TEST - simple divide */
  rtems_timespec_set(timespec1, 40, 4700);
  rtems_timespec_divide_by_integer(timespec1, 30, timespec2);
  rtems_test_assert( ((timespec2->tv_sec)==1) &&
			((timespec2->tv_nsec)==333333490) );
  puts( " rtems_timespec_divide_by_integer test PASSED" );

}

void test_compare(){
/* Each case hit another single branch in timespeclessthan.c file */
/* 
 * Basic check if timespec1 is less than timespec2. Timespec1 has lower number
 * of either seconds and nanoseconds.
 */
  rtems_timespec_set(timespec1,2,300);
  rtems_timespec_set(timespec2,3,400);
  result = rtems_timespec_less_than(timespec1,timespec2);
  rtems_test_assert(result);
/*
 * Another check if timespec1 is less. Now number of seconds are equal in both
 * timespec1 and timespec2. It hits another branch in rtems_timespec_less_than
 * method.
 */
  rtems_timespec_set(timespec2,2,400);
  result = rtems_timespec_less_than(timespec1,timespec2);
  rtems_test_assert(result);
/*
 * Another check if timespec1 is less. In this case timespecs are equal so it
 * should return false.
 */
  rtems_timespec_set(timespec2,2,300);
  result = rtems_timespec_less_than(timespec1,timespec2);
  rtems_test_assert(!result);
/*
 * Check if timespec1 is less. Both timespecs are equal on seconds, but 
 * timespec2 has smaller number of nanoseconds.
 */
  rtems_timespec_set(timespec2,2,0);
  result = rtems_timespec_less_than(timespec1,timespec2);
  rtems_test_assert(!result);
/* In this case timespec2 is smaller in both seconds and nanoseconds. */
  rtems_timespec_set(timespec2,1,400);
  result = rtems_timespec_less_than(timespec1,timespec2);
  rtems_test_assert(!result);

  puts( " rtems_timespec_less_than PASSED ");
}

void test_validity(){
/* Each case hits another branch in timespecisvalid.c src file */
  /* #1 INVALID - pointer to timespec is null */
  result=rtems_timespec_is_valid(tpointer);	
  rtems_test_assert(!result);

  tpointer=&notvalid; /* set pointer to invalid timespec */

  /* #2 INVALID- tv_sec of timespec is less than 0 */
  result=rtems_timespec_is_valid(tpointer);
  rtems_test_assert(!result);
  /* #3 INVALID- tv_nsec of timespec is less than 0 */
  notvalid.tv_sec=20; /* correct seconds */
  result=rtems_timespec_is_valid(tpointer);
  rtems_test_assert(!result);
  /* #4 INVALID- tv_nsec is not in the range - positive but too large */
  notvalid.tv_nsec=TOD_NANOSECONDS_PER_SECOND+10; 
  result=rtems_timespec_is_valid(tpointer);
  rtems_test_assert(!result);
  /* #5 VALID- valid timespec */
  rtems_timespec_set(tpointer,13, 500);
  result=rtems_timespec_is_valid(tpointer);
  rtems_test_assert(result);

  puts(" rtems_timespec_is_valid test PASSED");
}

void test_subtract(){
/* 
 * Simple subtraction, number of nanoseconds in timespec2 is less than in 
 * timespec1.
 */
  rtems_timespec_set(timespec1,10, 800);
  rtems_timespec_set(timespec2,13, 500);
  rtems_timespec_subtract(timespec1,timespec2,tpointer);
  rtems_test_assert( (tpointer->tv_sec==2) && 
      (tpointer->tv_nsec==(TOD_NANOSECONDS_PER_SECOND-300)) );
/* 
 * Simple subtraction, number of nanoseconds in timespec2 is greater than in 
 * timespec1. It hits another branch.
 */ 
  rtems_timespec_set(timespec1,10,200);
  rtems_timespec_subtract(timespec1,timespec2,tpointer);
  rtems_test_assert( (tpointer->tv_sec==3) && 
      (tpointer->tv_nsec==300) );
/* 
 * Case when timespec2 (end) is less than timespec1 (start). It produce
 * negative result.
 */
  rtems_timespec_set(timespec1,13,600);
  rtems_timespec_subtract(timespec1,timespec2,tpointer);
  rtems_test_assert( (tpointer->tv_sec==(-1)) && \
      (tpointer->tv_nsec==999999900) );
  puts(" rtems_timespec_subtract test PASSED");
}

void test_convert(){
/* RTEMS_TIMESPEC_FROM_TICKS TEST - basic conversions */
  uint32_t ticks=0;
  rtems_timespec_from_ticks(ticks, timespec1);
  rtems_test_assert( ((timespec1->tv_sec)==0) && ((timespec1->tv_nsec)==0) );
  ticks=1008;
  rtems_timespec_from_ticks(ticks, timespec2);
  puts( " rtems_timespec_from_ticks PASSED ");

/* RTEMS_TIMESPEC_TO_TICKS TEST - basic conversion - inverse to above ones */
  ticks = rtems_timespec_to_ticks(timespec1);
  rtems_test_assert(ticks==0);
  ticks = rtems_timespec_to_ticks(timespec2);
  rtems_test_assert(ticks==1008);
/* 
 * RTEMS_TIMESPEC_TO_TICKS TEST - test case when tv_nsec of timespec isn't 
 * multiplicity of nanoseconds_per_tick. Due to that, calculated number of ticks
 * should be increased by one.
 */
  uint32_t nanoseconds_per_tick;
  uint32_t nanoseconds;
  nanoseconds_per_tick  = rtems_configuration_get_nanoseconds_per_tick();
  nanoseconds = timespec2->tv_nsec;
  if( (nanoseconds % nanoseconds_per_tick)==0 )
    nanoseconds += 1;
  rtems_timespec_set(timespec1,timespec2->tv_sec,nanoseconds);
  ticks = rtems_timespec_to_ticks(timespec1);
  rtems_test_assert(ticks==1009);
  puts( " rtems_timespec_from_ticks and rtems_timespec_to_ticks test PASSED");
}


/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
