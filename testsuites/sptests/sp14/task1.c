/*
 *  COPYRIGHT (c) 1989-2011.
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

rtems_timer_service_routine Signal_3_to_task_1(
  rtems_id  id,
  void     *pointer
)
{
  rtems_status_code status;

  status = rtems_signal_send( Task_id[ 1 ], RTEMS_SIGNAL_3 );
  directive_failed_with_level( status, "rtems_signal_send of 3", 1 );

  Timer_got_this_id  = id;
  Timer_got_this_pointer = pointer;

  Signals_sent = TRUE;
}

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_mode        previous_mode;
  rtems_status_code status;

  puts( "TA1 - rtems_signal_catch - RTEMS_INTERRUPT_LEVEL( 3 )" );
  status = rtems_signal_catch( Process_asr, RTEMS_INTERRUPT_LEVEL(3) );
  directive_failed( status, "rtems_signal_catch" );

  puts( "TA1 - rtems_signal_send - RTEMS_SIGNAL_16 to self" );
  status = rtems_signal_send( RTEMS_SELF, RTEMS_SIGNAL_16 );
  directive_failed( status, "rtems_signal_send" );

  puts( "TA1 - rtems_signal_send - RTEMS_SIGNAL_0 to self" );
  status = rtems_signal_send( RTEMS_SELF, RTEMS_SIGNAL_0 );
  directive_failed( status, "rtems_signal_send" );

  puts( "TA1 - rtems_signal_catch - RTEMS_NO_ASR" );
  status = rtems_signal_catch( Process_asr, RTEMS_NO_ASR );
  directive_failed( status, "rtems_signal_catch" );

  FLUSH_OUTPUT();

rtems_test_pause();

  puts( "TA1 - rtems_signal_send - RTEMS_SIGNAL_1 to self" );
  status = rtems_signal_send( RTEMS_SELF, RTEMS_SIGNAL_1 );
  directive_failed( status, "rtems_signal_send" );

  puts( "TA1 - rtems_task_mode - disable ASRs" );
  status = rtems_task_mode( RTEMS_NO_ASR, RTEMS_ASR_MASK, &previous_mode );
  directive_failed( status, "rtems_task_mode" );

  Timer_got_this_id = 0;
  Timer_got_this_pointer = NULL;

  puts( "TA1 - sending signal to RTEMS_SELF from timer" );
  status = rtems_timer_fire_after(
    Timer_id[ 1 ],
    rtems_clock_get_ticks_per_second() / 2,
    Signal_3_to_task_1,
    (void *) Task_1
  );
  directive_failed( status, "rtems_timer_fire_after" );

  puts( "TA1 - waiting for signal to arrive" );

  Signals_sent = FALSE;
  Asr_fired    = FALSE;

  while ( Signals_sent == FALSE )
    ;

  if ( Timer_got_this_id == Timer_id[ 1 ] &&
       Timer_got_this_pointer == Task_1 )
    puts( "TA1 - timer routine got the correct arguments" );
  else
    printf(
      "TA1 - timer got (0x%" PRIxrtems_id ", %p) instead of (0x%"
        PRIxrtems_id ", %p)!!!!\n",
      Timer_got_this_id,
      Timer_got_this_pointer,
      Timer_id[ 1 ],
      Task_1
    );

  puts( "TA1 - rtems_task_mode - enable ASRs" );
  FLUSH_OUTPUT();
  status = rtems_task_mode( RTEMS_ASR, RTEMS_ASR_MASK, &previous_mode );
  directive_failed( status, "rtems_task_mode" );

  status = rtems_task_wake_after(2 * rtems_clock_get_ticks_per_second());
  directive_failed( status, "rtems_task_wake_after" );

  puts( "TA1 - rtems_signal_catch - asraddr of NULL" );
  status = rtems_signal_catch( NULL, RTEMS_DEFAULT_MODES );
  directive_failed( status, "rtems_signal_catch" );

  puts( "TA1 - rtems_task_delete - delete self" );
  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}
