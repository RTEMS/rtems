/*  timesys.h
 *
 *  This header file contains the global variables for the Time
 *  suite.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <tmacros.h>

/*
 *   How many times a particular operation is performed while timed.
 */
 
#define OPERATION_COUNT 100
#define IT_COUNT        100

/* functions */

#define put_time( _message, _total_time, \
                  _iterations, _loop_overhead, _overhead ) \
    printf( \
      "%s %d\n", \
      (_message), \
      (((_total_time) - (_loop_overhead)) / (_iterations)) - (_overhead) \
    )

#if  defined(STACK_CHECKER_ON) || defined(RTEMS_DEBUG)
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

TEST_EXTERN volatile rtems_unsigned32 end_time;   /* ending time variable */
TEST_EXTERN volatile rtems_unsigned32 overhead;   /* loop overhead variable */

TEST_EXTERN rtems_id   Task_id[ OPERATION_COUNT+1 ];   /* array of task ids */
TEST_EXTERN rtems_id   Task_name[ OPERATION_COUNT+1 ]; /* array of task names */

/* end of include file */
