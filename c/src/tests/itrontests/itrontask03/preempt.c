/*  Preempt_task
 *
 *  This routine serves as a test task.  It verifies the task manager.
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
 *
 *  $Id$
 */

#include <assert.h>
#include "system.h"

int Preempt_task_Count;

void Preempt_task()
{
  ER       status;
  T_RTSK   pk_rtsk;

  puts( "PREEMPT - ref_tsk validation"  );
  status = ref_tsk( &pk_rtsk, PREEMPT_TASK_ID );
  fatal_directive_status( status, E_OK , "ref_tsk of PREEMPT");
  assert( pk_rtsk.tskpri  == PREEMPT_PRIORITY );
  assert( pk_rtsk.itskpri == PREEMPT_PRIORITY );
  assert( pk_rtsk.task    == Preempt_task );
  assert( pk_rtsk.stksz   >= RTEMS_MINIMUM_STACK_SIZE );
  assert( pk_rtsk.tskstat == (TTS_RUN | TTS_RDY) );

  if ( Preempt_task_Count == 0 ) {
    Preempt_task_Count ++;
    puts( "PREEMPT - chg_pri increment priority "); 
    status = chg_pri( PREEMPT_TASK_ID, (PREEMPT_PRIORITY+1) );
    directive_failed( status, "chg_pri" );
    puts( "PREEMPT - ext_tsk - going to DORMANT state" );
    ext_tsk( );
    assert( 0 );
  } else {
    Preempt_task_Count ++;
    puts( "PREEMPT - exd_tsk - Exit and Delete task" );
    exd_tsk(  );
    assert( 0 );
  }
}

