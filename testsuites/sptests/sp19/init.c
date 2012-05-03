/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is a user initialization task and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
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

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  puts( "\n\n*** TEST 19 ***" );

  Task_name[ 1 ] =  rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ] =  rtems_build_name( 'T', 'A', '2', ' ' );
  Task_name[ 3 ] =  rtems_build_name( 'T', 'A', '3', ' ' );
  Task_name[ 4 ] =  rtems_build_name( 'T', 'A', '4', ' ' );
  Task_name[ 5 ] =  rtems_build_name( 'T', 'A', '5', ' ' );
  Task_name[ 6 ] =  rtems_build_name( 'F', 'P', '1', ' ' );

  status = rtems_task_create(
    Task_name[ 1 ],
    2,
    RTEMS_MINIMUM_STACK_SIZE * 4,
    RTEMS_DEFAULT_MODES,
    RTEMS_FLOATING_POINT,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TA1" );

  status = rtems_task_create(
    Task_name[ 2 ],
    2,
    RTEMS_MINIMUM_STACK_SIZE * 4,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of TA2" );

  status = rtems_task_create(
    Task_name[ 3 ],
    2,
    RTEMS_MINIMUM_STACK_SIZE * 4,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 3 ]
  );
  directive_failed( status, "rtems_task_create of TA3" );

  status = rtems_task_create(
    Task_name[ 4 ],
    2,
    RTEMS_MINIMUM_STACK_SIZE * 4,
    RTEMS_DEFAULT_MODES,
    RTEMS_FLOATING_POINT,
    &Task_id[ 4 ]
  );
  directive_failed( status, "rtems_task_create of TA4" );

  status = rtems_task_create(
    Task_name[ 5 ],
    2,
    RTEMS_MINIMUM_STACK_SIZE * 4,
    RTEMS_DEFAULT_MODES,
    RTEMS_FLOATING_POINT,
    &Task_id[ 5 ]
  );
  directive_failed( status, "rtems_task_create of TA5" );

  status = rtems_task_create(
    Task_name[ 6 ],
    1,
    RTEMS_MINIMUM_STACK_SIZE * 4,
    RTEMS_DEFAULT_MODES,
    RTEMS_FLOATING_POINT,
    &Task_id[ 6 ]
  );
  directive_failed( status, "rtems_task_create of FP1" );

  status = rtems_task_start( Task_id[ 6 ], First_FP_task, 0 );
  directive_failed( status, "rtems_task_start of FP1" );

  status = rtems_task_start( Task_id[ 1 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  status = rtems_task_start( Task_id[ 2 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start of TA2" );

  status = rtems_task_start( Task_id[ 3 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start of TA3" );

  status = rtems_task_start( Task_id[ 4 ], FP_task, 0 );
  directive_failed( status, "rtems_task_start of TA4" );

  status = rtems_task_start( Task_id[ 5 ], FP_task, 0 );
  directive_failed( status, "rtems_task_create of TA5" );

  /*
   *  Load "task dependent factors" in the context areas
   */


  FP_factors[0] =    0.0;
  FP_factors[1] = 1000.1;
  FP_factors[2] = 2000.2;
  FP_factors[3] = 3000.3;
  FP_factors[4] = 4000.4;
  FP_factors[5] = 5000.5;
  FP_factors[6] = 6000.6;
  FP_factors[7] = 7000.7;
  FP_factors[8] = 8000.8;
  FP_factors[9] = 9000.9;

  INTEGER_factors[0] = 0x0000;
  INTEGER_factors[1] = 0x1000;
  INTEGER_factors[2] = 0x2000;
  INTEGER_factors[3] = 0x3000;
  INTEGER_factors[4] = 0x4000;
  INTEGER_factors[5] = 0x5000;
  INTEGER_factors[6] = 0x6000;
  INTEGER_factors[7] = 0x7000;
  INTEGER_factors[8] = 0x8000;
  INTEGER_factors[9] = 0x9000;

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of TA1" );
}
