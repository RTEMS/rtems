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

#define CONFIGURE_INIT
#include <rtems.h>
#include "system.h"
#include "fptest.h"
#include <coverhd.h>
#include <tmacros.h>
#include <timesys.h>

#include <rtems/rtems/semimpl.h>

#if defined( RTEMS_SMP ) && defined( RTEMS_DEBUG )
  #define PREVENT_SMP_ASSERT_FAILURES
#endif

/* TEST DATA */
rtems_id Semaphore_id;

Objects_Locations location;   /* uses internal RTEMS type */

Thread_Control *Middle_tcb;   /* uses internal RTEMS type */

Thread_Control *Low_tcb;      /* uses internal RTEMS type */

/*
 *  Variables to hold execution times until they are printed
 *  at the end of the test.
 */

uint32_t   isr_disable_time;
uint32_t   isr_flash_time;
uint32_t   isr_enable_time;
uint32_t   thread_disable_dispatch_time;
uint32_t   thread_enable_dispatch_time;
uint32_t   thread_set_state_time;
uint32_t   thread_dispatch_no_fp_time;
uint32_t   context_switch_no_fp_time;
uint32_t   context_switch_self_time;
uint32_t   context_switch_another_task_time;
uint32_t   context_switch_restore_1st_fp_time;
uint32_t   context_switch_save_idle_restore_initted_time;
uint32_t   context_switch_save_restore_idle_time;
uint32_t   context_switch_save_restore_initted_time;
uint32_t   thread_resume_time;
uint32_t   thread_unblock_time;
uint32_t   thread_ready_time;
uint32_t   thread_get_time;
uint32_t   semaphore_get_time;
uint32_t   thread_get_invalid_time;

rtems_task null_task(
  rtems_task_argument argument
);

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

static void set_thread_dispatch_necessary( bool dispatch_necessary )
{
#if defined( PREVENT_SMP_ASSERT_FAILURES )
  ISR_Level level;

  _ISR_Disable_without_giant( level );
#endif

  _Thread_Dispatch_necessary = dispatch_necessary;

#if defined( PREVENT_SMP_ASSERT_FAILURES )
  _ISR_Enable_without_giant( level );
#endif
}

static void set_thread_heir( Thread_Control *thread )
{
#if defined( PREVENT_SMP_ASSERT_FAILURES )
  ISR_Level level;

  _ISR_Disable_without_giant( level );
#endif

  _Thread_Heir = thread;

#if defined( PREVENT_SMP_ASSERT_FAILURES )
  _ISR_Enable_without_giant( level );
#endif
}

static void set_thread_executing( Thread_Control *thread )
{
#if defined( PREVENT_SMP_ASSERT_FAILURES )
  ISR_Level level;

  _ISR_Disable_without_giant( level );
#endif

  _Thread_Executing = thread;

#if defined( PREVENT_SMP_ASSERT_FAILURES )
  _ISR_Enable_without_giant( level );
#endif
}

static void thread_disable_dispatch( void )
{
#if defined( PREVENT_SMP_ASSERT_FAILURES )
  Per_CPU_Control *self_cpu;
  ISR_Level level;

  _ISR_Disable_without_giant( level );
  ( void ) level;

  self_cpu = _Per_CPU_Get();
  self_cpu->thread_dispatch_disable_level = 1;

  _Per_CPU_Acquire( self_cpu );
#else
  _Thread_Disable_dispatch();
#endif
}

static void thread_set_state( Thread_Control *thread, States_Control state )
{
#if defined( PREVENT_SMP_ASSERT_FAILURES )
  _Thread_Disable_dispatch();
#endif

  _Thread_Set_state( thread, state );

#if defined( PREVENT_SMP_ASSERT_FAILURES )
  _Thread_Unnest_dispatch();
#endif
}

static void thread_resume( Thread_Control *thread )
{
#if defined( PREVENT_SMP_ASSERT_FAILURES )
  _Thread_Disable_dispatch();
#endif

  _Thread_Resume( thread );

#if defined( PREVENT_SMP_ASSERT_FAILURES )
  _Thread_Unnest_dispatch();
#endif
}

static void thread_unblock( Thread_Control *thread )
{
#if defined( PREVENT_SMP_ASSERT_FAILURES )
  _Thread_Disable_dispatch();
#endif

  _Thread_Unblock( thread );

#if defined( PREVENT_SMP_ASSERT_FAILURES )
  _Thread_Unnest_dispatch();
#endif
}

static void thread_ready( Thread_Control *thread )
{
#if defined( PREVENT_SMP_ASSERT_FAILURES )
  _Thread_Disable_dispatch();
#endif

  _Thread_Ready( thread );

#if defined( PREVENT_SMP_ASSERT_FAILURES )
  _Thread_Unnest_dispatch();
#endif
}

rtems_task null_task(
  rtems_task_argument argument
)
{
}

rtems_task Init(
  rtems_task_argument argument
)
{
  uint32_t    index;
  rtems_id          task_id;
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 26 ***" );

  if (_Scheduler.Operations.initialize != _Scheduler_priority_Initialize) {
    puts("  Error ==> " );
    puts("Test only supported for deterministic priority scheduler\n" );
    puts( "*** END OF TEST 26 ***" );
    rtems_test_exit( 0 );
  }

#define FP1_PRIORITY (RTEMS_MAXIMUM_PRIORITY - 3u)      /* 201, */
  status = rtems_task_create(
    rtems_build_name( 'F', 'P', '1', ' ' ),
    FP1_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_FLOATING_POINT,
    &task_id
  );
  directive_failed( status, "rtems_task_create of FP1" );

  status = rtems_task_start( task_id, Floating_point_task_1, 0 );
  directive_failed( status, "rtems_task_start of FP1" );

#define FP2_PRIORITY (RTEMS_MAXIMUM_PRIORITY - 2u)      /* 202, */
  status = rtems_task_create(
    rtems_build_name( 'F', 'P', '2', ' ' ),
    FP2_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_FLOATING_POINT,
    &task_id
  );
  directive_failed( status, "rtems_task_create of FP2" );

  status = rtems_task_start( task_id, Floating_point_task_2, 0 );
  directive_failed( status, "rtems_task_start of FP2" );

#define LOW_PRIORITY (RTEMS_MAXIMUM_PRIORITY - 4u)   /*  200, */
  status = rtems_task_create(
    rtems_build_name( 'L', 'O', 'W', ' ' ),
    LOW_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  directive_failed( status, "rtems_task_create of LOW" );

  status = rtems_task_start( task_id, Low_task, 0 );
  directive_failed( status, "rtems_task_start of LOW" );

#define MIDDLE_PRIORITY (RTEMS_MAXIMUM_PRIORITY - 5u)   /*  128, */
  status = rtems_task_create(
    rtems_build_name( 'M', 'I', 'D', ' ' ),
    MIDDLE_PRIORITY,
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
      RTEMS_MAXIMUM_PRIORITY - 1u,      /* 254, */
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

  _Thread_Disable_dispatch();

  benchmark_timer_initialize();
    rtems_interrupt_disable( level );
  isr_disable_time = benchmark_timer_read();

  benchmark_timer_initialize();
    rtems_interrupt_flash( level );
  isr_flash_time = benchmark_timer_read();

  benchmark_timer_initialize();
    rtems_interrupt_enable( level );
  isr_enable_time = benchmark_timer_read();

  _Thread_Enable_dispatch();

  benchmark_timer_initialize();
    _Thread_Disable_dispatch();
  thread_disable_dispatch_time = benchmark_timer_read();

  benchmark_timer_initialize();
    _Thread_Enable_dispatch();
  thread_enable_dispatch_time = benchmark_timer_read();

  benchmark_timer_initialize();
    thread_set_state( _Thread_Get_executing(), STATES_SUSPENDED );
  thread_set_state_time = benchmark_timer_read();

  set_thread_dispatch_necessary( true );

  benchmark_timer_initialize();
    _Thread_Dispatch();           /* dispatches Middle_task */
}

rtems_task Middle_task(
  rtems_task_argument argument
)
{
  Chain_Control   *ready_queues;

  thread_dispatch_no_fp_time = benchmark_timer_read();

  thread_set_state( _Thread_Get_executing(), STATES_SUSPENDED );

  Middle_tcb   = _Thread_Get_executing();

  ready_queues      = (Chain_Control *) _Scheduler.information;
  set_thread_executing(
    (Thread_Control *) _Chain_First(&ready_queues[LOW_PRIORITY])
  );

  /* do not force context switch */

  set_thread_dispatch_necessary( false );

  thread_disable_dispatch();

  benchmark_timer_initialize();
    _Context_Switch(
      &Middle_tcb->Registers,
      &_Thread_Get_executing()->Registers
    );

  benchmark_timer_initialize();
    _Context_Switch(&Middle_tcb->Registers, &Low_tcb->Registers);
}

rtems_task Low_task(
  rtems_task_argument argument
)
{
  Thread_Control *executing;
  Chain_Control  *ready_queues;

  ready_queues      = (Chain_Control *) _Scheduler.information;

  context_switch_no_fp_time = benchmark_timer_read();

  executing    = _Thread_Get_executing();

  Low_tcb = executing;

  benchmark_timer_initialize();
    _Context_Switch( &executing->Registers, &executing->Registers );

  context_switch_self_time = benchmark_timer_read();

  _Context_Switch(&executing->Registers, &Middle_tcb->Registers);

  context_switch_another_task_time = benchmark_timer_read();

  set_thread_executing(
    (Thread_Control *) _Chain_First(&ready_queues[FP1_PRIORITY])
  );

  /* do not force context switch */

  set_thread_dispatch_necessary( false );

  thread_disable_dispatch();

  benchmark_timer_initialize();
#if (CPU_HARDWARE_FP == 1) || (CPU_SOFTWARE_FP == 1)
    _Context_Restore_fp( &_Thread_Get_executing()->fp_context );
#endif
    _Context_Switch(
      &executing->Registers,
      &_Thread_Get_executing()->Registers
    );
}

rtems_task Floating_point_task_1(
  rtems_task_argument argument
)
{
  Chain_Control   *ready_queues;
  Thread_Control  *executing;
  FP_DECLARE;

  context_switch_restore_1st_fp_time = benchmark_timer_read();

  executing = _Thread_Get_executing();

  ready_queues      = (Chain_Control *) _Scheduler.information;
  set_thread_executing(
    (Thread_Control *) _Chain_First(&ready_queues[FP2_PRIORITY])
  );

  /* do not force context switch */

  set_thread_dispatch_necessary( false );

  thread_disable_dispatch();

  benchmark_timer_initialize();
#if (CPU_HARDWARE_FP == 1) || (CPU_SOFTWARE_FP == 1)
    _Context_Save_fp( &executing->fp_context );
    _Context_Restore_fp( &_Thread_Get_executing()->fp_context );
#endif
    _Context_Switch(
      &executing->Registers,
      &_Thread_Get_executing()->Registers
    );
  /* switch to Floating_point_task_2 */

  context_switch_save_idle_restore_initted_time = benchmark_timer_read();

  FP_LOAD( 1.0 );

  executing = _Thread_Get_executing();

  ready_queues      = (Chain_Control *) _Scheduler.information;
  set_thread_executing(
    (Thread_Control *) _Chain_First(&ready_queues[FP2_PRIORITY])
  );

  benchmark_timer_initialize();
#if (CPU_HARDWARE_FP == 1) || (CPU_SOFTWARE_FP == 1)
    _Context_Save_fp( &executing->fp_context );
    _Context_Restore_fp( &_Thread_Get_executing()->fp_context );
#endif
    _Context_Switch(
      &executing->Registers,
      &_Thread_Get_executing()->Registers
    );
  /* switch to Floating_point_task_2 */
}

rtems_task Floating_point_task_2(
  rtems_task_argument argument
)
{
  Chain_Control  *ready_queues;
  Thread_Control *executing;
  FP_DECLARE;

  context_switch_save_restore_idle_time = benchmark_timer_read();

  executing = _Thread_Get_executing();

  ready_queues      = (Chain_Control *) _Scheduler.information;
  set_thread_executing(
    (Thread_Control *) _Chain_First(&ready_queues[FP1_PRIORITY])
  );

  FP_LOAD( 1.0 );

  benchmark_timer_initialize();
#if (CPU_HARDWARE_FP == 1) || (CPU_SOFTWARE_FP == 1)
    _Context_Save_fp( &executing->fp_context );
    _Context_Restore_fp( &_Thread_Get_executing()->fp_context );
#endif
    _Context_Switch(
      &executing->Registers,
      &_Thread_Get_executing()->Registers
    );
  /* switch to Floating_point_task_1 */

  context_switch_save_restore_initted_time = benchmark_timer_read();

  complete_test();
}

void complete_test( void )
{
  uint32_t    index;
  rtems_id          task_id;

  benchmark_timer_initialize();
    thread_resume( Middle_tcb );
  thread_resume_time = benchmark_timer_read();

  thread_set_state( Middle_tcb, STATES_WAITING_FOR_MESSAGE );

  benchmark_timer_initialize();
    thread_unblock( Middle_tcb );
  thread_unblock_time = benchmark_timer_read();

  thread_set_state( Middle_tcb, STATES_WAITING_FOR_MESSAGE );

  benchmark_timer_initialize();
    thread_ready( Middle_tcb );
  thread_ready_time = benchmark_timer_read();

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) benchmark_timer_empty_function();
  overhead = benchmark_timer_read();

  task_id = Middle_tcb->Object.id;

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) _Thread_Get( task_id, &location );
  thread_get_time = benchmark_timer_read();

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) _Semaphore_Get( Semaphore_id, &location );
  semaphore_get_time = benchmark_timer_read();

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) _Thread_Get( 0x3, &location );
  thread_get_invalid_time = benchmark_timer_read();

  /*
   *  This is the running task and we have tricked RTEMS out enough where
   *  we need to set some internal tracking information to match this.
   */

  set_thread_heir( _Thread_Get_executing() );
  set_thread_dispatch_necessary( false );

  _Thread_Dispatch_set_disable_level( 0 );

  /*
   *  Now dump all the times
   */

  put_time(
    "_ISR_Disable",
    isr_disable_time,
    1,
    0,
    0
  );

  put_time(
    "_ISR_Flash",
    isr_flash_time,
    1,
    0,
    0
  );

  put_time(
    "_ISR_Enable",
    isr_enable_time,
    1,
    0,
    0
  );

  put_time(
    "_Thread_Disable_dispatch",
    thread_disable_dispatch_time,
    1,
    0,
    0
  );

  put_time(
    "_Thread_Enable_dispatch",
    thread_enable_dispatch_time,
    1,
    0,
    0
  );

  put_time(
    "_Thread_Set_state",
    thread_set_state_time,
    1,
    0,
    0
  );

  put_time(
    "_Thread_Dispatch (NO FP)",
    thread_dispatch_no_fp_time,
    1,
    0,
    0
  );

  put_time(
    "context switch: no floating point contexts",
    context_switch_no_fp_time,
    1,
    0,
    0
  );

  put_time(
    "context switch: self",
    context_switch_self_time,
    1,
    0,
    0
  );

  put_time(
    "context switch: to another task",
    context_switch_another_task_time,
    1,
    0,
    0
  );

#if (CPU_HARDWARE_FP == 1) || (CPU_SOFTWARE_FP == 1)
  put_time(
    "fp context switch: restore 1st FP task",
    context_switch_restore_1st_fp_time,
    1,
    0,
    0
  );

  put_time(
    "fp context switch: save idle, restore initialized",
    context_switch_save_idle_restore_initted_time,
    1,
    0,
    0
  );

  put_time(
    "fp context switch: save idle, restore idle",
    context_switch_save_restore_idle_time,
    1,
    0,
    0
  );

  put_time(
    "fp context switch: save initialized, restore initialized",
    context_switch_save_restore_initted_time,
    1,
    0,
    0
  );
#else
    puts( "fp context switch: restore 1st FP task - NA" );
    puts( "fp context switch: save idle, restore initialized - NA" );
    puts( "fp context switch: save idle, restore idle - NA" );
    puts( "fp context switch: save initialized, restore initialized - NA" );
#endif

  put_time(
    "_Thread_Resume",
    thread_resume_time,
    1,
    0,
    0
  );

  put_time(
    "_Thread_Unblock",
    thread_unblock_time,
    1,
    0,
    0
  );

  put_time(
    "_Thread_Ready",
    thread_ready_time,
    1,
    0,
    0
  );

  put_time(
    "_Thread_Get",
    thread_get_time,
    OPERATION_COUNT,
    0,
    0
  );

  put_time(
    "_Semaphore_Get",
    semaphore_get_time,
    OPERATION_COUNT,
    0,
    0
  );

  put_time(
    "_Thread_Get: invalid id",
    thread_get_invalid_time,
    OPERATION_COUNT,
    0,
    0
  );

  puts( "*** END OF TEST 26 ***" );
  rtems_test_exit( 0 );
}
