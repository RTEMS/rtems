/*  Process_asr
 *
 *  This is the asynchronous signal routine (asr) for task 1.
 *  It demonstrates that ASRs can block execute and block.
 *
 *  Input parameters:
 *    the_signal_set - signal set
 *
 *  Output parameters:  NONE
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

rtems_asr Process_asr(
  rtems_signal_set the_signal_set
)
{
  rtems_status_code status;

  printf( "ASR - ENTRY - signal => %08x\n", the_signal_set );
  switch( the_signal_set ) {
    case RTEMS_SIGNAL_16:
    case RTEMS_SIGNAL_17:
    case RTEMS_SIGNAL_18 | RTEMS_SIGNAL_19:
      break;
    case RTEMS_SIGNAL_0:
    case RTEMS_SIGNAL_1:
      puts( "ASR - rtems_task_wake_after - yield processor" );
      status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
      directive_failed( status, "rtems_task_wake_after yield" );
      break;
    case RTEMS_SIGNAL_3:
      Asr_fired = TRUE;
      break;
  }
  printf( "ASR - EXIT  - signal => %08x\n", the_signal_set );
}
