/*
 *
 *  COPYRIGHT (c) 1989-1997.
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
#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#include <rtems.h>
#include "system.h"
#include "fptest.h"
#include <coverhd.h>
#include <tmacros.h>
#include <timesys.h>


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

  Print_Warning();

  puts( "\n\n*** TIME TEST 26 ***" );

  status = rtems_task_create(
    rtems_build_name( 'F', 'P', '1', ' ' ),
    201,
    RTEMS_MINIMUM_STACK_SIZE,
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
    RTEMS_MINIMUM_STACK_SIZE,
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
    RTEMS_MINIMUM_STACK_SIZE,
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
    RTEMS_MINIMUM_STACK_SIZE,
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
    RTEMS_MINIMUM_STACK_SIZE,
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
    RTEMS_NO_PRIORITY,
    &Semaphore_id
  );
  directive_failed( status, "rtems_semaphore_create" );

  for ( index = 1 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'N', 'U', 'L', 'L' ),
      254,
      RTEMS_MINIMUM_STACK_SIZE,
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
    "_ISR_Disable",
    end_time,
    1,
    0,
    0
  );

  Timer_initialize();
    rtems_interrupt_flash( level );
  end_time = Read_timer();
 
  put_time(
    "_ISR_Flash",
    end_time,
    1,
    0,
    0
  );
 
  Timer_initialize();
    rtems_interrupt_enable( level );
  end_time = Read_timer();
 
  put_time(
    "_ISR_Enable",
    end_time,
    1,
    0,
    0
  );
 
  Timer_initialize();
    _Thread_Disable_dispatch();
  end_time = Read_timer();

  put_time(
    "_Thread_Disable_dispatch",
    end_time,
    1,
    0,
    0
  );

  Timer_initialize();
    _Thread_Enable_dispatch();
  end_time = Read_timer();

  put_time(
    "_Thread_Enable_dispatch",
    end_time,
    1,
    0,
    0
  );

  Timer_initialize();
    _Thread_Set_state( _Thread_Executing, STATES_SUSPENDED );
  end_time = Read_timer();

  put_time(
    "_Thread_Set_state",
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
    "_Thread_Disptach (NO FP)",
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
    "context switch: no floating point contexts",
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
    "context switch: self",
    end_time,
    1,
    0,
    0
  );

  _Context_Switch(&executing->Registers, &Middle_tcb->Registers);

  end_time = Read_timer();

  put_time(
    "context switch: to another task",
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
    "fp context switch: restore 1st FP task",
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
    "fp context switch: save idle, restore initialized",
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
    "fp context switch: save idle, restore idle",
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
    "fp context switch: save initialized, restore initialized",
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
    "_Thread_Resume",
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
    "_Thread_Unblock",
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
    "_Thread_Ready",
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
    "_Thread_Get",
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
    "_Semaphore_Get",
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
    "_Thread_Get: invalid id",
    end_time,
    OPERATION_COUNT,
    0,
    0
  );

  puts( "*** END OF TEST 26 ***" );
  exit( 0 );
}

