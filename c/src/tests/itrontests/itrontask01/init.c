/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is called from init_exec and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define TEST_INIT
#include "system.h"
#include <stdio.h>

void ITRON_Init( void )
{
  rtems_time_of_day time;
  ER                status;
  T_CTSK            pk_ctsk;

  puts( "\n\n*** ITRON TASK TEST 1 ***" );

  /*
   * XXX - Change this to an itron clock !!
   */

  build_time( &time, 12, 31, 1988, 9, 0, 0, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  pk_ctsk.exinf    = NULL;
  pk_ctsk.tskatr   = TA_HLNG;
  pk_ctsk.itskpri  = 1; 
  pk_ctsk.task     = Task_2_through_4;

  pk_ctsk.stksz    = RTEMS_MINIMUM_STACK_SIZE * 2; 
  status = cre_tsk( 2, &pk_ctsk );
  directive_failed( status, "cre_tsk of TA1" );

  pk_ctsk.stksz = RTEMS_MINIMUM_STACK_SIZE * 2;
  status = cre_tsk( 3, &pk_ctsk );
  directive_failed( status, "cre_tsk of TA2" );

  pk_ctsk.stksz = RTEMS_MINIMUM_STACK_SIZE * 3; 
  status = cre_tsk( 4, &pk_ctsk );
  directive_failed( status, "cre_tsk of TA3" );

  status  = sta_tsk( 2, 0 );
  directive_failed( status, "sta_tsk of TA1" );

  status  = sta_tsk( 3, 0 );
  directive_failed( status, "sta_tsk of TA1" );

  status  = sta_tsk( 4, 0 );
  directive_failed( status, "sta_tsk of TA1" );

  exd_tsk();
  directive_failed( 0, "exd_tsk" );
}
