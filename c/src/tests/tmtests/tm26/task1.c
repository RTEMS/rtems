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

#include <rtems.h>
#include "system.h"
#include "fptest.h"
#include <coverhd.h>
#include <tmacros.h>
#include <timesys.h>

#undef EXTERN
#define EXTERN
#include "gvar.h"
#include "conftbl.h"
#undef EXTERN
#define EXTERN extern

/* TEST DATA */
rtems_id Semaphore_id;

Objects_Locations location;   /* uses internal RTEMS type */

Thread_Control *Middle_tcb;   /* uses internal RTEMS type */

Thread_Control *Low_tcb;      /* uses internal RTEMS type */

rtems_task High_task(
  rtems_task_argument argument
);

rtems_task Middle_task(
  rtems_task_argument argument
);

rtems_task Low_task(
  rtems_task_argument argument
);

rtems_task Floating_point_task_1(
  rtems_task_argument argument
);

rtems_task Floating_point_task_2(
  rtems_task_argument argument
);

void complete_test( void );

rtems_task null_task(
  rtems_task_argument argument
)
{
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_unsigned32  index;
  rtems_id          task_id;
  rtems_status_code status;

  puts( "\n\n*** TIME TEST 26 ***" );

  status = rtems_task_create(
    rtems_build_name( 'F', 'P', '1', ' ' ),
    201,
    2048,
    RTEMS_DEFAULT_MODES,
    RTEMS_FLOATING_POINT,
    &task_id
  );
  directive_failed( status, "rtems_task_create of FP1" );

  status = rtems_task_start( task_id, Floating_point_task_1, 0 );
  directive_failed( status, "rtems_task_start of FP1" );

  status = rtems_task_create(
    rtems_build_name( 'F', 'P', '2', ' ' ),
    202,
    2048,
    RTEMS_DEFAULT_MODES,
    RTEMS_FLOATING_POINT,
    &task_id
  );
  directive_failed( status, "rtems_task_create of FP2" );

  status = rtems_task_start( task_id, Floating_point_task_2, 0 );
  directive_failed( status, "rtems_task_start of FP2" );

  status = rtems_task_create(
    rtems_build_name( 'L', 'O', 'W', ' ' ),
    200,
    2048,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  directive_failed( status, "rtems_task_create of LOW" );

  status = rtems_task_start( task_id, Low_task, 0 );
  directive_failed( status, "rtems_task_start of LOW" );

  status = rtems_task_create(
    rtems_build_name( 'M', 'I', 'D', ' ' ),
    128,
    2048,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  directive_failed( status, "rtems_task_create of MIDDLE" );

  status = rtems_task_start( task_id, Middle_task, 0 );
  directive_failed( status, "rtems_task_start of MIDDLE" );

  status = rtems_task_create(
    rtems_build_name( 'H', 'I', 'G', 'H' ),
    5,
    2048,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  directive_failed( status, "rtems_task_create of HIGH" );

  status = rtems_task_start( task_id, High_task, 0 );
  directive_failed( status, "rtems_task_start of HIGH" );

  status = rtems_semaphore_create(
    rtems_build_name( 'S', 'E', 'M', '1' ),
    OPERATION_COUNT,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Semaphore_id
  );
  directive_failed( status, "rtems_semaphore_create" );

  for ( index = 1 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'N', 'U', 'L', 'L' ),
      254,
      1024,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &task_id
    );
    directive_failed( status, "rtems_task_create LOOP" );

    status = rtems_task_start( task_id, null_task, 0 );
    directive_failed( status, "rtems_task_start LOOP" );
  }

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

rtems_task High_task(
  rtems_task_argument argument
)
{
  rtems_interrupt_level level;

  Timer_initialize();
    rtems_interrupt_disable( level );
  end_time = Read_timer();
 
  put_time(
    "INTERRUPT DISABLE",
    end_time,
    1,
    0,
    0
  );

  Timer_initialize();
    rtems_interrupt_flash( level );
  end_time = Read_timer();
 
  put_time(
    "INTERRUPT FLASH",
    end_time,
    1,
    0,
    0
  );
 
  Timer_initialize();
    rtems_interrupt_enable( level );
  end_time = Read_timer();
 
  put_time(
    "INTERRUPT ENABLE",
    end_time,
    1,
    0,
    0
  );
 
  Timer_initialize();
    _Thread_Disable_dispatch();
  end_time = Read_timer();

  put_time(
    "THREAD_DISABLE_DISPATCH",
    end_time,
    1,
    0,
    0
  );

  Timer_initialize();
    _Thread_Enable_dispatch();
  end_time = Read_timer();

  put_time(
    "THREAD_ENABLE_DISPATCH",
    end_time,
    1,
    0,
    0
  );

  Timer_initialize();
    _Thread_Set_state( _Thread_Executing, STATES_SUSPENDED );
  end_time = Read_timer();

  put_time(
    "THREAD_SET_STATE",
    end_time,
    1,
    0,
    0
  );

  _Context_Switch_necessary = TRUE;

  Timer_initialize();
    _Thread_Dispatch();           /* dispatches Middle_task */
}

rtems_task Middle_task(
  rtems_task_argument argument
)
{
  end_time = Read_timer();

  put_time(
    "THREAD_DISPATCH (NO FP)",
    end_time,
    1,
    0,
    0
  );

  _Thread_Set_state( _Thread_Executing, STATES_SUSPENDED );

  Middle_tcb   = _Thread_Executing;

  _Thread_Executing =
        (Thread_Control *) _Thread_Ready_chain[200].first;

  /* do not force context switch */

  _Context_Switch_necessary = FALSE;

  _Thread_Disable_dispatch();

  Timer_initialize();
    _Context_Switch( &Middle_tcb->Registers, &_Thread_Executing->Registers );

  Timer_initialize();
    _Context_Switch(&Middle_tcb->Registers, &Low_tcb->Registers);
}

rtems_task Low_task(
  rtems_task_argument argument
)
{
  Thread_Control *executing;

  end_time = Read_timer();

  put_time(
    "CONTEXT_SWITCH (NO FP)",
    end_time,
    1,
    0,
    0
  );

  executing    = _Thread_Executing;

  Low_tcb = executing;

  Timer_initialize();
    _Context_Switch( &executing->Registers, &executing->Registers );

  end_time = Read_timer();

  put_time(
    "CONTEXT_SWITCH (SELF)",
    end_time,
    1,
    0,
    0
  );

  _Context_Switch(&executing->Registers, &Middle_tcb->Registers);

  end_time = Read_timer();

  put_time(
    "CONTEXT_SWITCH (Initialised)",
    end_time,
    1,
    0,
    0
  );

  _Thread_Executing =
        (Thread_Control *) _Thread_Ready_chain[201].first;

  /* do not force context switch */

  _Context_Switch_necessary = FALSE;

  _Thread_Disable_dispatch();

  Timer_initialize();
    _Context_Restore_fp( &_Thread_Executing->fp_context );
    _Context_Switch( &executing->Registers, &_Thread_Executing->Registers );
}

rtems_task Floating_point_task_1(
  rtems_task_argument argument
)
{
  Thread_Control *executing;
  FP_DECLARE;

  end_time = Read_timer();

  put_time(
    "CONTEXT_SWITCH (restore 1st FP)",
    end_time,
    1,
    0,
    0
  );

  executing = _Thread_Executing;

  _Thread_Executing =
        (Thread_Control *) _Thread_Ready_chain[202].first;

  /* do not force context switch */

  _Context_Switch_necessary = FALSE;

  _Thread_Disable_dispatch();

  Timer_initialize();
    _Context_Save_fp( &executing->fp_context );
    _Context_Restore_fp( &_Thread_Executing->fp_context );
    _Context_Switch( &executing->Registers, &_Thread_Executing->Registers );
  /* switch to Floating_point_task_2 */

  end_time = Read_timer();

  put_time(
    "CONTEXT_SWITCH (used->init FP)",
    end_time,
    1,
    0,
    0
  );

  FP_LOAD( 1.0 );

  executing = _Thread_Executing;

  _Thread_Executing =
       (Thread_Control *) _Thread_Ready_chain[202].first;

  /* do not force context switch */

  _Context_Switch_necessary = FALSE;

  _Thread_Disable_dispatch();

  Timer_initialize();
    _Context_Save_fp( &executing->fp_context );
    _Context_Restore_fp( &_Thread_Executing->fp_context );
    _Context_Switch( &executing->Registers, &_Thread_Executing->Registers );
  /* switch to Floating_point_task_2 */
}

rtems_task Floating_point_task_2(
  rtems_task_argument argument
)
{
  Thread_Control *executing;
  FP_DECLARE;

  end_time = Read_timer();

  put_time(
    "CONTEXT_SWITCH (init->init FP)",
    end_time,
    1,
    0,
    0
  );

  executing = _Thread_Executing;

  _Thread_Executing =
       (Thread_Control *) _Thread_Ready_chain[201].first;

  FP_LOAD( 1.0 );

  /* do not force context switch */

  _Context_Switch_necessary = FALSE;

  _Thread_Disable_dispatch();

  Timer_initialize();
    _Context_Save_fp( &executing->fp_context );
    _Context_Restore_fp( &_Thread_Executing->fp_context );
    _Context_Switch( &executing->Registers, &_Thread_Executing->Registers );
  /* switch to Floating_point_task_1 */

  end_time = Read_timer();

  put_time(
    "CONTEXT_SWITCH (used->used FP)",
    end_time,
    1,
    0,
    0
  );

  complete_test();
}

void complete_test( void )
{
  rtems_unsigned32  index;
  rtems_id          task_id;

  Timer_initialize();
    _Thread_Resume( Middle_tcb );
  end_time = Read_timer();

  put_time(
    "THREAD_RESUME",
    end_time,
    1,
    0,
    0
  );

  _Thread_Set_state( Middle_tcb, STATES_WAITING_FOR_MESSAGE );

  Timer_initialize();
    _Thread_Unblock( Middle_tcb );
  end_time = Read_timer();

  put_time(
    "THREAD_UNBLOCK",
    end_time,
    1,
    0,
    0
  );

  _Thread_Set_state( Middle_tcb, STATES_WAITING_FOR_MESSAGE );

  Timer_initialize();
    _Thread_Ready( Middle_tcb );
  end_time = Read_timer();

  put_time(
    "THREAD_READY",
    end_time,
    1,
    0,
    0
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) Empty_function();
  overhead = Read_timer();

  task_id = Middle_tcb->Object.id;

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) _Thread_Get( task_id, &location );
  end_time = Read_timer();

  put_time(
    "THREAD_GET",
    end_time,
    OPERATION_COUNT,
    0,
    0
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) _Semaphore_Get( Semaphore_id, &location );
  end_time = Read_timer();

  put_time(
    "SEMAPHORE_GET",
    end_time,
    OPERATION_COUNT,
    0,
    0
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) _Thread_Get( 0x3, &location );
  end_time = Read_timer();

  put_time(
    "THREAD_GET invalid id",
    end_time,
    OPERATION_COUNT,
    0,
    0
  );
  exit( 0 );
}

