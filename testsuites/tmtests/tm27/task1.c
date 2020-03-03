/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
#include <rtems/btimer.h>
#include <rtems/score/schedulerpriorityimpl.h>

#define _RTEMS_TMTEST27
#include <tm27.h>

const char rtems_test_name[] = "TIME TEST 27";

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

static void set_thread_executing( Thread_Control *thread )
{
  _Per_CPU_Get_snapshot()->executing = thread;
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  Print_Warning();

  TEST_BEGIN();

  if (
    _Scheduler_Table[ 0 ].Operations.initialize
      != _Scheduler_priority_Initialize
  ) {
    puts("  Error ==> " );
    puts("Test only supported for deterministic priority scheduler\n" );
    TEST_END();
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

  rtems_task_exit();
}

rtems_task Task_1(
  rtems_task_argument argument
)
{
  Scheduler_priority_Context *scheduler_context =
    _Scheduler_priority_Get_context( _Thread_Scheduler_get_home( _Thread_Get_executing() ) );
#if defined(RTEMS_SMP)
  rtems_interrupt_level level;
#endif

  Install_tm27_vector( Isr_handler );

  /*
   *  No preempt .. no nesting
   */

  Interrupt_nest = 0;

  Interrupt_occurred = 0;

  benchmark_timer_initialize();
  Cause_tm27_intr();
  /* goes to Isr_handler */

#if (MUST_WAIT_FOR_INTERRUPT == 1)
  while ( Interrupt_occurred == 0 );
#endif
  Interrupt_return_time = benchmark_timer_read();

  put_time(
    "rtems interrupt: entry overhead returns to interrupted task",
    Interrupt_enter_time,
    1,
    0,
    timer_overhead
  );

  put_time(
    "rtems interrupt: exit overhead returns to interrupted task",
    Interrupt_return_time,
    1,
    0,
    timer_overhead
  );

  /*
   *  No preempt .. nested
   */

  _Thread_Dispatch_disable();

  Interrupt_nest = 1;

  Interrupt_occurred = 0;
  benchmark_timer_initialize();
  Cause_tm27_intr();
  /* goes to Isr_handler */

#if (MUST_WAIT_FOR_INTERRUPT == 1)
  while ( Interrupt_occurred == 0 );
#endif
  Interrupt_return_time = benchmark_timer_read();

  _Thread_Dispatch_enable( _Per_CPU_Get() );

  put_time(
    "rtems interrupt: entry overhead returns to nested interrupt",
    Interrupt_enter_nested_time,
    1,
    0,
    0
  );

  put_time(
    "rtems interrupt: exit overhead returns to nested interrupt",
    Interrupt_return_nested_time,
    1,
    0,
    0
  );

  /*
   *  Does a preempt .. not nested
   */

#if defined(RTEMS_SMP)
  _ISR_Local_disable(level);
#endif

  set_thread_executing(
    (Thread_Control *) _Chain_First(&scheduler_context->Ready[LOW_PRIORITY])
  );

  _Thread_Dispatch_necessary = 1;

#if defined(RTEMS_SMP)
  _ISR_Local_enable(level);
#endif

  Interrupt_occurred = 0;
  benchmark_timer_initialize();
  Cause_tm27_intr();

  /*
   *  goes to Isr_handler and then returns
   */

  TEST_END();
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
  Thread_Control *executing = _Thread_Get_executing();
  const Scheduler_Control    *scheduler;
  Scheduler_priority_Context *scheduler_context;
  ISR_lock_Context state_lock_context;
  ISR_lock_Context scheduler_lock_context;

  _Thread_State_acquire( executing, &state_lock_context );
  scheduler = _Thread_Scheduler_get_home( executing );
  scheduler_context = _Scheduler_priority_Get_context( scheduler );
  _Thread_State_release( executing, &state_lock_context );

#if (MUST_WAIT_FOR_INTERRUPT == 1)
  while ( Interrupt_occurred == 0 );
#endif
  end_time = benchmark_timer_read();

  put_time(
    "rtems interrupt: entry overhead returns to preempting task",
    Interrupt_enter_time,
    1,
    0,
    timer_overhead
  );

  put_time(
    "rtems interrupt: exit overhead returns to preempting task",
    end_time,
    1,
    0,
    0
  );

  fflush( stdout );

  /*
   *  Switch back to the other task to exit the test.
   */

  _Thread_State_acquire( executing, &state_lock_context );
  _Scheduler_Acquire_critical( scheduler, &scheduler_lock_context );

  set_thread_executing(
    (Thread_Control *) _Chain_First(&scheduler_context->Ready[LOW_PRIORITY])
  );

  _Thread_Dispatch_necessary = 1;

  _Scheduler_Release_critical( scheduler, &scheduler_lock_context );
  _Thread_State_release( executing, &state_lock_context );

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
