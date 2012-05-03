/*
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

#define MAXIMUM_DISTRIBUTION 1000

#undef OPERATION_COUNT
#define OPERATION_COUNT    100000

uint32_t Distribution[ MAXIMUM_DISTRIBUTION + 1 ];

rtems_task Task_1(
  rtems_task_argument argument
);

void check_read_timer( void );

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_id          id;
  rtems_status_code status;

  /*
   *  Tell the Timer Driver what we are doing
   */

  benchmark_timer_disable_subtracting_average_overhead( 1 );

  Print_Warning();

  puts( "\n\n*** TIME CHECKER ***" );

  Task_name[ 1 ] = rtems_build_name( 'T', 'A', '1', ' ' ),

  status = rtems_task_create(
    1,
    5,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create of TA1" );

  status = rtems_task_start( id, Task_1, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

rtems_task Task_1(
  rtems_task_argument argument
)
{
  uint32_t   index;

  check_read_timer();
rtems_test_pause();

  benchmark_timer_initialize();
  end_time = benchmark_timer_read();

  put_time(
    "NULL timer stopped at",
    end_time,
    1,
    0,
    0
  );

  benchmark_timer_initialize();
  for ( index = 1 ; index <= 1000 ; index++ )
    (void) benchmark_timer_empty_function();
  end_time = benchmark_timer_read();

  put_time(
    "LOOP (1000) timer stopped at",
    end_time,
    1,
    0,
    0
  );

  benchmark_timer_initialize();
  for ( index = 1 ; index <= 10000 ; index++ )
    (void) benchmark_timer_empty_function();
  end_time = benchmark_timer_read();

  put_time(
    "LOOP (10000) timer stopped at",
    end_time,
    1,
    0,
    0
  );

  benchmark_timer_initialize();
  for ( index = 1 ; index <= 50000 ; index++ )
    (void) benchmark_timer_empty_function();
  end_time = benchmark_timer_read();

  put_time(
    "LOOP (50000) timer stopped at",
    end_time,
    1,
    0,
    0
  );

  benchmark_timer_initialize();
  for ( index = 1 ; index <= 100000 ; index++ )
    (void) benchmark_timer_empty_function();
  end_time = benchmark_timer_read();

  put_time(
    "LOOP (100000) timer stopped at",
    end_time,
    1,
    0,
    0
  );

  puts( "*** END OF TIME CHECKER ***" );
  rtems_test_exit( 0 );
}

void check_read_timer()
{
  uint32_t   index;
  uint32_t   time;

  for ( index = 1 ; index <= MAXIMUM_DISTRIBUTION ; index++ )
    Distribution[ index ] = 0;

  for ( index = 1 ; index <= OPERATION_COUNT ; ) {
    benchmark_timer_initialize();
    end_time = benchmark_timer_read();
    if ( end_time > MAXIMUM_DISTRIBUTION ) {
      /*
       *  Under UNIX a simple process swap takes longer than we
       *  consider valid for our testing purposes.
       */
      printf( "TOO LONG (%" PRIu32 ") at index %" PRIu32 "!!!\n", end_time, index );
      continue;
    }
    Distribution[ end_time ]++;
    index++;
  }

  printf( "Units may not be in microseconds for this test!!!\n" );
  time = 0;
  for ( index = 0 ; index <= MAXIMUM_DISTRIBUTION ; index++ ) {
    time += (Distribution[ index ] * index);
    if ( Distribution[ index ] != 0 )
      printf( "%" PRId32 " %" PRId32 "\n", index, Distribution[ index ] );
  }
  printf( "Total time = %" PRId32 "\n", time );
  printf( "Average time = %" PRId32 "\n", time / OPERATION_COUNT );
}
