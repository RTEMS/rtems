/*
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#define TEST_INIT
#include "system.h"

rtems_asr Process_asr_for_pass_1(
  rtems_signal_set signals
);

rtems_asr Process_asr_for_pass_2(
  rtems_signal_set signals
);

rtems_task Task_1(
  rtems_task_argument argument
);

rtems_task Task_2(
  rtems_task_argument argument
);

rtems_task Task_3(
  rtems_task_argument argument
);

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 19 ***" );

  status = rtems_task_create(
    rtems_build_name( 'T', 'I', 'M', 'E' ),
    128,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TASK1" );

  status = rtems_task_start( Task_id[ 1 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start of TASK1" );

  status = rtems_task_create(
    rtems_build_name( 'T', 'I', 'M', 'E' ),
    127,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of TASK2" );

  status = rtems_task_start( Task_id[ 2 ], Task_2, 0 );
  directive_failed( status, "rtems_task_start of TASK2" );

  status = rtems_task_create(
    rtems_build_name( 'T', 'I', 'M', 'E' ),
    126,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 3 ]
  );
  directive_failed( status, "rtems_task_create of TASK3" );

  status = rtems_task_start( Task_id[ 3 ], Task_3, 0 );
  directive_failed( status, "rtems_task_start of TASK3" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

rtems_asr Process_asr_for_pass_1(
  rtems_signal_set signals
)
{
  end_time = Read_timer();

  put_time(
    "rtems_signal_send: signal to self",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_SIGNAL_SEND
  );

  Timer_initialize();
}

rtems_asr Process_asr_for_pass_2(
  rtems_signal_set signals
)
{
  rtems_status_code status;

  status = rtems_task_resume( Task_id[ 3 ] );
  directive_failed( status, "rtems_task_resume" );

  Timer_initialize();
}

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  Timer_initialize();
    (void) rtems_signal_catch( Process_asr_for_pass_1, RTEMS_DEFAULT_MODES );
  end_time = Read_timer();

  put_time(
    "rtems_signal_catch",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_SIGNAL_CATCH
  );

  Timer_initialize();
    rtems_signal_send( Task_id[ 2 ], 1 );
  end_time = Read_timer();

  put_time(
    "rtems_signal_send: returns to caller",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_SIGNAL_SEND
  );

  Timer_initialize();
    (void) rtems_signal_send( RTEMS_SELF, RTEMS_SIGNAL_1 );

  /* end time is done is RTEMS_ASR */

  end_time = Read_timer();

  put_time(
    "exit ASR overhead: returns to calling task",
    end_time,
    1,
    0,
    0
  );

  status = rtems_signal_catch( Process_asr_for_pass_2, RTEMS_NO_PREEMPT );
  directive_failed( status, "rtems_signal_catch" );

  Timer_initialize();
    (void) rtems_signal_send( RTEMS_SELF, RTEMS_SIGNAL_1 );
}

rtems_asr Process_asr_for_task_2(
  rtems_signal_set signals
)
{
  ;
}

rtems_task Task_2(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  status = rtems_signal_catch( Process_asr_for_task_2, RTEMS_DEFAULT_MODES );
  directive_failed( status, "rtems_signal_catch" );

  (void) rtems_task_suspend( RTEMS_SELF );
}

rtems_task Task_3(
  rtems_task_argument argument
)
{
  (void) rtems_task_suspend( RTEMS_SELF );

  end_time = Read_timer();

  put_time(
    "exit ASR overhead: returns to preempting task",
    end_time,
    1,
    0,
    0
  );

  puts( "*** END OF TEST 19 ***" );
  rtems_test_exit( 0 );
}
