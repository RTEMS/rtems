/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/*
 *  WARNING!!!!!!!!!
 *
 *  THIS TEST USES INTERNAL RTEMS VARIABLES!!!
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

#include <bsp.h>

#define _RTEMS_TMTEST27
#include <tm27.h>

rtems_task Task_1(
  rtems_task_argument argument
);

rtems_task Task_2(
  rtems_task_argument argument
);

volatile uint32_t   Interrupt_occurred;
volatile uint32_t   Interrupt_enter_time, Interrupt_enter_nested_time;
volatile uint32_t   Interrupt_return_time, Interrupt_return_nested_time;
uint32_t   Interrupt_nest;
uint32_t   timer_overhead;

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
  if (_Scheduler.Operations.initialize != _Scheduler_priority_Initialize) {
    puts("  Error ==> " );
    puts("Test only supported for deterministic priority scheduler\n" );
    puts( "*** END OF TEST 26 ***" );
    rtems_test_exit( 0 );
  }

#define LOW_PRIORITY (RTEMS_MAXIMUM_PRIORITY - 1u)
  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '1', ' ' ),
    LOW_PRIORITY,
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
    LOW_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of Task_2" );

  status = rtems_task_start( Task_id[ 2 ], Task_2, 0 );
  directive_failed( status, "rtems_task_start of Task_2" );

  benchmark_timer_initialize();
  benchmark_timer_read();
  benchmark_timer_initialize();
  timer_overhead = benchmark_timer_read();

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

rtems_task Task_1(
  rtems_task_argument argument
)
{
  Chain_Control   *ready_queues;

  Install_tm27_vector( Isr_handler );

  /*
   *  No preempt .. no nesting
   */

  Interrupt_nest = 0;

  _Thread_Dispatch_set_disable_level( 0 );

  Interrupt_occurred = 0;

  benchmark_timer_initialize();
  Cause_tm27_intr();
  /* goes to Isr_handler */

#if (MUST_WAIT_FOR_INTERRUPT == 1)
  while ( Interrupt_occurred == 0 );
#endif
  Interrupt_return_time = benchmark_timer_read();

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

  _Thread_Dispatch_set_disable_level( 1 );

  Interrupt_nest = 1;

  Interrupt_occurred = 0;
  benchmark_timer_initialize();
  Cause_tm27_intr();
  /* goes to Isr_handler */

#if (MUST_WAIT_FOR_INTERRUPT == 1)
  while ( Interrupt_occurred == 0 );
#endif
  Interrupt_return_time = benchmark_timer_read();

  _Thread_Dispatch_set_disable_level( 0 );

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

  _Thread_Dispatch_set_disable_level( 0 );

  ready_queues      = (Chain_Control *) _Scheduler.information;
  _Thread_Executing =
        (Thread_Control *) _Chain_First(&ready_queues[LOW_PRIORITY]);

  _Thread_Dispatch_necessary = 1;

  Interrupt_occurred = 0;
  benchmark_timer_initialize();
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
  Chain_Control   *ready_queues;

#if (MUST_WAIT_FOR_INTERRUPT == 1)
  while ( Interrupt_occurred == 0 );
#endif
  end_time = benchmark_timer_read();

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

  _Thread_Dispatch_set_disable_level( 0 );

  ready_queues      = (Chain_Control *) _Scheduler.information;
  _Thread_Executing =
        (Thread_Control *) _Chain_First(&ready_queues[LOW_PRIORITY]);

  _Thread_Dispatch_necessary = 1;

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
  end_time = benchmark_timer_read();

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
      benchmark_timer_initialize();
      Cause_tm27_intr();
      /* goes to a nested copy of Isr_handler */
#if (MUST_WAIT_FOR_INTERRUPT == 1)
       while ( Interrupt_occurred == 0 );
#endif
      Interrupt_return_nested_time = benchmark_timer_read();
      break;
    case 2:
      Interrupt_enter_nested_time = end_time;
      break;
  }

  benchmark_timer_initialize();
}
