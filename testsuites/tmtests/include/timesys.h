/*  timesys.h
 *
 *  This header file contains the global variables for the Time
 *  suite.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <tmacros.h>
#include <inttypes.h>

/*
 *  This constant determines the maximum number of a resource
 *  that will be created.  For example, some test create multiple
 *  blocking tasks to determine the execution time of blocking
 *  services.  By default, the blocking time of 100 tasks will
 *  be measured.  Small targets often do not have enough memory
 *  to create 100 tasks.  By overriding the default OPERATION_COUNT
 *  with a lower number (typically 10 or less), all of the time tests
 *  can usually be run.  This is stil not very fine-grained but
 *  is enough to significantly reduce memory consumption.
 */

#ifndef OPERATION_COUNT
#define OPERATION_COUNT 100
#endif

/* functions */

#define put_time( _message, _total_time, \
                  _iterations, _loop_overhead, _overhead ) \
    printf( \
      "%s %" PRId32 "\n", \
      (_message), \
      (((_total_time) - (_loop_overhead)) / (_iterations)) - (_overhead) \
    )

#if  defined(CONFIGURE_STACK_CHECKER_ENABLED) || defined(RTEMS_DEBUG)
#define Print_Warning() \
  do { \
    puts( \
      "\n" \
      "THE TIMES REPORTED BY THIS TEST INCLUDE DEBUG CODE!\n" \
      "\n" \
    ); \
  } while (0)

#else
#define Print_Warning()
#endif

/* variables */

TEST_EXTERN volatile uint32_t   end_time;   /* ending time variable */
TEST_EXTERN volatile uint32_t   overhead;   /* loop overhead variable */

TEST_EXTERN rtems_id   Task_id[ OPERATION_COUNT+1 ];   /* array of task ids */
TEST_EXTERN rtems_name Task_name[ OPERATION_COUNT+1 ]; /* array of task names */

/* end of include file */
