/*
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

/*
 *  WARNING!!!!!!!!!   
 *
 *  THIS TEST USES INTERNAL RTEMS VARIABLES!!!
 */

#define TEST_INIT
#define RTEMS_TM27
#include "system.h"

#include <bsp.h>

rtems_task Task_1(
  rtems_task_argument argument
);

rtems_task Task_2(
  rtems_task_argument argument
);

volatile rtems_unsigned32 Interrupt_occurred;
volatile rtems_unsigned32 Interrupt_enter_time, Interrupt_enter_nested_time;
volatile rtems_unsigned32 Interrupt_return_time, Interrupt_return_nested_time;
rtems_unsigned32 Interrupt_nest;
rtems_unsigned32 timer_overhead;

rtems_isr Isr_handler(
  rtems_vector_number vector
);

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 27 ***" );

  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '1', ' ' ),
    254,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create Task_1" );

  status = rtems_task_start( Task_id[ 1 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start Task_1" );

  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '2', ' ' ),
    254,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of Task_2" );

  status = rtems_task_start( Task_id[ 2 ], Task_2, 0 );
  directive_failed( status, "rtems_task_start of Task_2" );

  Timer_initialize();
  Read_timer();
  Timer_initialize();
  timer_overhead = Read_timer();

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

rtems_task Task_1(
  rtems_task_argument argument
)
{
  Install_tm27_vector( Isr_handler );

  /*
   *  No preempt .. no nesting
   */

  Interrupt_nest = 0;

  _Thread_Dispatch_disable_level = 0;

  Interrupt_occurred = 0;

  Timer_initialize();
  Cause_tm27_intr();
  /* goes to Isr_handler */

#if (MUST_WAIT_FOR_INTERRUPT == 1)
  while ( Interrupt_occurred == 0 );
#endif
  Interrupt_return_time = Read_timer();

  put_time(
    "interrupt entry overhead: returns to interrupted task",
    Interrupt_enter_time,
    1,
    0,
    timer_overhead
  );

  put_time(
    "interrupt exit overhead: returns to interrupted task",
    Interrupt_return_time,
    1,
    0,
    timer_overhead
  );

  /*
   *  No preempt .. nested
   */

  _Thread_Dispatch_disable_level = 1;

  Interrupt_nest = 1;

  Interrupt_occurred = 0;
  Timer_initialize();
  Cause_tm27_intr();
  /* goes to Isr_handler */

#if (MUST_WAIT_FOR_INTERRUPT == 1)
  while ( Interrupt_occurred == 0 );
#endif
  Interrupt_return_time = Read_timer();

  put_time(
    "interrupt entry overhead: returns to nested interrupt",
    Interrupt_enter_nested_time,
    1,
    0,
    0
  );

  put_time(
    "interrupt exit overhead: returns to nested interrupt",
    Interrupt_return_nested_time,
    1,
    0,
    0
  );

  /*
   *  Does a preempt .. not nested
   */

  _Thread_Dispatch_disable_level = 0;

  _Thread_Heir = (rtems_tcb *) _Thread_Ready_chain[254].last;

  _Context_Switch_necessary = 1;

  Interrupt_occurred = 0;
  Timer_initialize();
  Cause_tm27_intr();

  /*
   *  goes to Isr_handler and then returns
   */

  puts( "*** END OF TEST 27 ***" );
  rtems_test_exit( 0 );
}

/*
 *  NOTE:  When this task is executing, some of the assumptions made 
 *         regarding the placement of the currently executing task's TCB
 *         on the ready chains have been violated.  At least the assumption
 *         that this task is at the head of the chain for its priority
 *         has been violated.
 */

rtems_task Task_2(
  rtems_task_argument argument
)
{
#if (MUST_WAIT_FOR_INTERRUPT == 1)
  while ( Interrupt_occurred == 0 );
#endif
  end_time = Read_timer();

  put_time(
    "interrupt entry overhead: returns to preempting task",
    Interrupt_enter_time,
    1,
    0,
    timer_overhead
  );

  put_time(
    "interrupt exit overhead: returns to preempting task",
    end_time,
    1,
    0,
    0
  );

  fflush( stdout );

  /*
   *  Switch back to the other task to exit the test.
   */

  _Thread_Dispatch_disable_level = 0;
 
  _Thread_Heir = (rtems_tcb *) _Thread_Ready_chain[254].first;
 
  _Context_Switch_necessary = 1;

  _Thread_Dispatch();

}

/*  The Isr_handler() and Isr_handler_inner() routines are structured
 *  so that there will be as little entry overhead as possible included
 *  in the interrupt entry time.
 */

void Isr_handler_inner( void );

rtems_isr Isr_handler(
  rtems_vector_number vector
)
{
  end_time = Read_timer();

  Interrupt_occurred = 1;
  Isr_handler_inner();
}

void Isr_handler_inner( void )
{

  /*enable_tracing();*/
  Clear_tm27_intr();
  switch ( Interrupt_nest ) {
    case 0:
      Interrupt_enter_time = end_time;
      break;
    case 1:
      Interrupt_enter_time = end_time;
      Interrupt_nest = 2;
      Interrupt_occurred = 0;
      Lower_tm27_intr();
      Timer_initialize();
      Cause_tm27_intr();
      /* goes to a nested copy of Isr_handler */
#if (MUST_WAIT_FOR_INTERRUPT == 1)
       while ( Interrupt_occurred == 0 );
#endif
      Interrupt_return_nested_time = Read_timer();
      break;
    case 2:
      Interrupt_enter_nested_time = end_time;
      break;
  }

  Timer_initialize();
}
