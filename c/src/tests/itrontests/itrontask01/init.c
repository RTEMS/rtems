/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is a user initialization task and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.  This test is based off of sp01.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */


#define TEST_INIT
#include "system.h"

#warning "Task_id is really an rtems_id"
void Task_1_through_3( void ){}

void ITRON_Init( void )
{
  rtems_time_of_day time;
  ER                status;
  T_CTSK            values;

  puts( "\n\n*** ITRONTASK01 - TICKER TEST ***" );

  build_time( &time, 12, 31, 1988, 9, 0, 0, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  values.exinf   = NULL;
  values.tskatr  = TA_HLNG; 
  values.task    = Task_1_through_3;
  values.itskpri = 1;
  values.stksz   = RTEMS_MINIMUM_STACK_SIZE;

  status = cre_tsk( Task_id[1], &values );
  directive_failed( status, "cre_tsk of TA1" );

  values.stksz   = RTEMS_MINIMUM_STACK_SIZE;
  status = cre_tsk( Task_id[2], &values );
  directive_failed( status, "cre_tsk of TA1" );

  values.stksz   = RTEMS_MINIMUM_STACK_SIZE;
  status = cre_tsk( Task_id[3], &values );
  directive_failed( status, "cre_tsk of TA1" );

#if 0
  status = sta_tsk( Task_id[1] );
  directive_failed( status, "rtems_task_start of TA1" );

  directive_failed( status, "rtems_task_start of TA2" );

  status = rtems_task_start( Task_id[ 3 ], Task_1_through_3, 0 );
  directive_failed( status, "rtems_task_start of TA3" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
#endif
}
