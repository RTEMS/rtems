/*  Task_1
 *
 *  This task initializes the signal catcher, sends the first signal
 *  if running on the first node, and loops while waiting for signals.
 *
 *  NOTE: The signal catcher is not reentrant and hence RTEMS_NO_ASR must
 *        be a part of its execution mode.
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

#include "system.h"

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  puts( "TA1 - rtems_signal_catch: initializing signal catcher" );
  status = rtems_signal_catch( Process_asr, RTEMS_NO_ASR | RTEMS_NO_PREEMPT );
  directive_failed( status, "rtems_signal_catch" );

  puts( "TA1 - Sending signal to self" );
  status = rtems_signal_send( Task_id[ 1 ], RTEMS_SIGNAL_16 );
  directive_failed( status, "rtems_signal_send" );

  if ( Task_2_preempted == TRUE )
    puts( "TA1 - TA2 correctly preempted me" );

  puts("TA1 - Got Back!!!");

  puts( "*** END OF TEST 17 ***" );
  rtems_test_exit( 0 );
}
