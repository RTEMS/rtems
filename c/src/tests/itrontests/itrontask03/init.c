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
#include <assert.h>

void ITRON_Init( void )
{
  ER                status;
  T_CTSK            pk_ctsk;
  T_RTSK            pk_rtsk;

  pk_ctsk.exinf    = NULL;
  pk_ctsk.tskatr   = TA_HLNG;
  pk_ctsk.stksz    = RTEMS_MINIMUM_STACK_SIZE; 
  pk_ctsk.itskpri  = PREEMPT_PRIORITY; 
  pk_ctsk.task     = Preempt_task;

  puts( "\n\n*** ITRON TASK TEST 3 ***" );

  /*
   *  Create and start the Preempt task the first time.
   *  Verify that it is dormant when it comes back.
   */

  puts( "INIT - Create and Start PREEMPT" );
  status = chg_pri( TSK_SELF, (PREEMPT_PRIORITY+2) );
  directive_failed( status, "chg_pri of SELF" );
  
  status = cre_tsk(  PREEMPT_TASK_ID, &pk_ctsk );
  directive_failed( status, "cre_tsk of RTEMS_PREEMPT" );

  status  = sta_tsk( PREEMPT_TASK_ID, 0 );
  directive_failed( status, "sta_tsk of RTEMS_PREEMPT" );
  puts( "INIT - rot_rdq - no tasks at this priority" );
  status = rot_rdq( 1 );
  directive_failed( status, "rot_rdq" );

  puts( "INIT - ref_tsk PREEMPT - Validate DORMANT STATE" );
  status = ref_tsk( &pk_rtsk, PREEMPT_TASK_ID );
  directive_failed( status, "INIT - ref_tsk of RTEMS_PREEMPT");
  fatal_directive_status(pk_rtsk.tskstat,TTS_DMT,"tskstat of PREEMPT");
  
  /*
   * Restart the Preempt Task.
   */

  status  = sta_tsk( PREEMPT_TASK_ID, 0 );
  directive_failed( status, "sta_tsk of RTEMS_PREEMPT" );
  puts( "INIT - rot_rdq - yielding processor" );
  status = rot_rdq( 1 );
  directive_failed( status, "rot_rdq" );
  puts( "INIT - ref_tsk PREEMPT - Validate no longer exists" );
  status = ref_tsk( &pk_rtsk, PREEMPT_TASK_ID );
  fatal_directive_status( status, E_NOEXS, "tskstat of PREEMPT");
  status = chg_pri( TSK_SELF, PREEMPT_PRIORITY );
  directive_failed( status, "chg_pri of SELF" );
  
  /*
   * XXX 
   */
	
  pk_ctsk.itskpri  = 3; 
  pk_ctsk.task     = Task_1;
  status = cre_tsk( TA1_ID, &pk_ctsk );
  directive_failed( status, "cre_tsk of TA1" );

  pk_ctsk.task     = Task_2;
  status = cre_tsk( TA2_ID, &pk_ctsk );
  directive_failed( status, "cre_tsk of TA2" );

  pk_ctsk.task     = Task_3;
  status = cre_tsk( TA3_ID, &pk_ctsk );
  directive_failed( status, "cre_tsk of TA3" );

  status  = sta_tsk( TA1_ID, 0 );
  directive_failed( status, "sta_tsk of TA1" );
  status  = sta_tsk( TA2_ID, 0 );
  directive_failed( status, "sta_tsk of TA2" );
  status  = sta_tsk( TA3_ID, 0 );
  directive_failed( status, "sta_tsk of TA3" );

  status = ref_tsk( &pk_rtsk, TA1_ID);
  directive_failed( status, "INIT - ref_tsk of TA1");
  fatal_directive_status( pk_rtsk.tskstat, TTS_RDY , "tskstat of TA1");

  puts( "INIT - suspending TA2 while middle task on a ready chain" );
  status = sus_tsk( TA2_ID  );
  directive_failed( status, "sus_tsk of TA2" );
  status = ref_tsk( &pk_rtsk, TA2_ID);
  directive_failed( status, "INIT - ref_tsk of TA2");
  fatal_directive_status( pk_rtsk.tskstat, TTS_SUS, "tskstat of TA2");

  status = ter_tsk( TA1_ID );
  directive_failed( status, "ter_tsk of TA1" );
  status = del_tsk( TA1_ID );
  directive_failed( status, "del_tsk of TA1" );
  status = ter_tsk( TA2_ID );
  directive_failed( status, "ter_tsk of TA2" );
  status = ter_tsk( TA3_ID );
  directive_failed( status, "ter_tsk of TA3" );

  pk_ctsk.itskpri  = 1; 
  pk_ctsk.task     = Task_1;
  status = cre_tsk( TA1_ID, &pk_ctsk );
  directive_failed( status, "cre_tsk of TA1 at priority 1" );

  status  = sta_tsk( TA1_ID, 0 );
  directive_failed( status, "sta_tsk of TA1" );
  status  = sta_tsk( TA2_ID, 0 );
  directive_failed( status, "sta_tsk of TA2" );
  status  = sta_tsk( TA3_ID, 0 );
  directive_failed( status, "sta_tsk of TA3" );

  exd_tsk();
  assert(0);
}






