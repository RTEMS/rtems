/*
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

#include "system.h"
#undef EXTERN
#define EXTERN
#include "conftbl.h"
#include "gvar.h"

rtems_task Test_task(
  rtems_task_argument argument
);

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  puts( "\n\n*** TIME TEST 1 ***" );

  Task_name[ 1 ] = rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ] = rtems_build_name( 'T', 'A', '2', ' ' );
  Task_name[ 3 ] = rtems_build_name( 'T', 'A', '3', ' ' );

  status = rtems_task_create(
    Task_name[ 1 ],
    128,
    4096,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TA1" );

  status = rtems_task_start( Task_id[ 1 ], Test_task, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );

}

rtems_task Test_task(
  rtems_task_argument argument
)
{
  rtems_unsigned32  semaphore_obtain_time;
  rtems_unsigned32  semaphore_release_time;
  rtems_unsigned32  semaphore_obtain_no_wait_time;
  rtems_unsigned32  semaphore_obtain_loop_time;
  rtems_unsigned32  semaphore_release_loop_time;
  rtems_unsigned32  index;
  rtems_unsigned32  iterations;
  rtems_name        name;
  rtems_id          smid;
  rtems_status_code status;

  name = rtems_build_name( 'S', 'M', '1', ' ' );

  semaphore_obtain_time          = 0;
  semaphore_release_time         = 0;
  semaphore_obtain_no_wait_time  = 0;
  semaphore_obtain_loop_time     = 0;
  semaphore_release_loop_time    = 0;


  /* Time one invocation of rtems_semaphore_create */

  Timer_initialize();
    (void) rtems_semaphore_create(
      name,
      OPERATION_COUNT,
      RTEMS_DEFAULT_MODES,
      &smid
    );
  end_time = Read_timer();
  put_time(
    "rtems_semaphore_create",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_SEMAPHORE_CREATE
  );

  /* Time one invocation of rtems_semaphore_delete */

  Timer_initialize();
    (void) rtems_semaphore_delete( smid );
  end_time = Read_timer();
  put_time(
    "rtems_semaphore_delete",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_SEMAPHORE_CREATE
  );

  status = rtems_semaphore_create(
    name,
    OPERATION_COUNT,
    RTEMS_DEFAULT_ATTRIBUTES,
    &smid
  );

  for ( iterations=OPERATION_COUNT ; iterations ; iterations-- ) {

    Timer_initialize();
      for ( index = 1 ; index<=OPERATION_COUNT ; index++ )
        (void) Empty_function();
    end_time = Read_timer();

    semaphore_obtain_loop_time  += end_time;
    semaphore_release_loop_time += end_time;

    /* rtems_semaphore_obtain (available) */

    Timer_initialize();
      for ( index = 1 ; index<=OPERATION_COUNT ; index++ )
        (void) rtems_semaphore_obtain(
          smid,
          RTEMS_DEFAULT_OPTIONS,
          RTEMS_NO_TIMEOUT
        );
    end_time = Read_timer();

    semaphore_obtain_time += end_time;

    /* rtems_semaphore_release */

    Timer_initialize();
      for ( index = 1 ; index<=OPERATION_COUNT ; index++ )
        (void) rtems_semaphore_release( smid );
    end_time = Read_timer();

    semaphore_release_time += end_time;

    /* semaphore obtain (RTEMS_NO_WAIT) */
    Timer_initialize();
      for ( index = 1 ; index<=OPERATION_COUNT ; index++ )
        rtems_semaphore_obtain( smid, RTEMS_NO_WAIT, RTEMS_NO_TIMEOUT );
    semaphore_obtain_no_wait_time += Read_timer();

    Timer_initialize();
      for ( index = 1 ; index<=OPERATION_COUNT ; index++ )
        rtems_semaphore_release( smid );
    end_time = Read_timer();

    semaphore_release_time += end_time;
  }

  put_time(
    "rtems_semaphore_obtain",
    semaphore_obtain_time,
    OPERATION_COUNT * OPERATION_COUNT,
    semaphore_obtain_loop_time,
    CALLING_OVERHEAD_SEMAPHORE_OBTAIN
  );

  put_time(
    "rtems_semaphore_obtain (RTEMS_NO_WAIT)",
    semaphore_obtain_no_wait_time,
    OPERATION_COUNT * OPERATION_COUNT,
    semaphore_obtain_loop_time,
    CALLING_OVERHEAD_SEMAPHORE_OBTAIN
  );

  put_time(
    "rtems_semaphore_release",
    semaphore_release_time,
    OPERATION_COUNT * OPERATION_COUNT * 2,
    semaphore_release_loop_time * 2,
    CALLING_OVERHEAD_SEMAPHORE_RELEASE
  );

  exit( 0 );
}
