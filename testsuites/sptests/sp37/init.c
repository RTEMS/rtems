/**
 *  @brief Test for Bodies of Macros
 *
 *  Interrupt Disable/Enable Tests
 *  Clock Tick from task level
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

const char rtems_test_name[] = "SP 37";

/* prototypes */
void test_interrupt_inline(void);
void check_isr_in_progress_inline(void);
rtems_task blocked_task(rtems_task_argument argument);
rtems_timer_service_routine test_unblock_task(
  rtems_id  timer,
  void     *arg
);
rtems_timer_service_routine test_unblock_task(
  rtems_id  timer,
  void     *arg
);
void check_isr_worked(
  char *s,
  int   result
);
rtems_timer_service_routine test_isr_in_progress(
  rtems_id  timer,
  void     *arg
);

/* test bodies */

#define TEST_ISR_EVENT RTEMS_EVENT_0

typedef struct {
  ISR_Level actual_level;
  rtems_id master_task_id;
} test_isr_level_context;

static void isr_level_check_task( rtems_task_argument arg )
{
  test_isr_level_context *ctx = (test_isr_level_context *) arg;
  rtems_status_code sc;

  ctx->actual_level = _ISR_Get_level();

  sc = rtems_event_send( ctx->master_task_id,  TEST_ISR_EVENT );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  ( void ) rtems_task_suspend( RTEMS_SELF );
  rtems_test_assert( 0 );
}

static void test_isr_level_for_new_threads( ISR_Level last_proper_level )
{
  ISR_Level mask = CPU_MODES_INTERRUPT_MASK;
  ISR_Level current;
  test_isr_level_context ctx = {
    .master_task_id = rtems_task_self()
  };

  for ( current = 0 ; current <= mask ; ++current ) {
    rtems_mode initial_modes;
    rtems_id id;
    rtems_status_code sc;
    rtems_event_set events;

    initial_modes = RTEMS_INTERRUPT_LEVEL(current);

#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
    if ( initial_modes != 0 ) {
      break;
    }
#endif

    ctx.actual_level = 0xffffffff;

    sc = rtems_task_create(
      rtems_build_name('I', 'S', 'R', 'L'),
      RTEMS_MINIMUM_PRIORITY,
      RTEMS_MINIMUM_STACK_SIZE,
      initial_modes,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    rtems_test_assert( sc == RTEMS_SUCCESSFUL );

    sc = rtems_task_start(
      id,
      isr_level_check_task,
      (rtems_task_argument) &ctx
    );
    rtems_test_assert( sc == RTEMS_SUCCESSFUL );

    sc = rtems_event_receive(
      TEST_ISR_EVENT,
      RTEMS_EVENT_ALL | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
    );
    rtems_test_assert( sc == RTEMS_SUCCESSFUL );
    rtems_test_assert( events == TEST_ISR_EVENT );

    if ( current <= last_proper_level ) {
      rtems_test_assert( ctx.actual_level == current );
    } else {
      rtems_test_assert( ctx.actual_level == last_proper_level );
    }

    sc = rtems_task_delete( id ) ;
    rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  }
}

static void test_isr_level( void )
{
  ISR_Level mask = CPU_MODES_INTERRUPT_MASK;
  ISR_Level normal = _ISR_Get_level();
  ISR_Level current = 0;
  ISR_Level last_proper_level;

  _ISR_Set_level( current );
  rtems_test_assert( _ISR_Get_level() == current );

  for ( current = current + 1 ; current <= mask ; ++current ) {
    ISR_Level actual;

    _ISR_Set_level( current );

    actual = _ISR_Get_level();
    rtems_test_assert( actual == current || actual == ( current - 1 ) );

    if ( _ISR_Get_level() != current ) {
      break;
    }
  }

  last_proper_level = current - 1;

  for ( current = current + 1 ; current <= mask ; ++current ) {
    _ISR_Set_level( current );
    rtems_test_assert( _ISR_Get_level() == current );
  }

  _ISR_Set_level( normal );

  /*
   * Now test that the ISR level specified for _Thread_Initialize() propagates
   * properly to the thread.
   */
  test_isr_level_for_new_threads( last_proper_level );
}

static void test_isr_locks( void )
{
  static const char name[] = "test";
  ISR_Level normal_interrupt_level = _ISR_Get_level();
  ISR_lock_Control initialized = ISR_LOCK_INITIALIZER( name );
  ISR_lock_Control zero_initialized;
  union {
    ISR_lock_Control lock;
    uint8_t bytes[ sizeof( ISR_lock_Control ) ];
  } container;
  ISR_lock_Context lock_context;
  size_t i;
  const uint8_t *bytes;
  ISR_Level interrupt_level;

  memset( &container, 0xff, sizeof( container ) );
  _ISR_lock_Initialize( &container.lock, name );
  bytes = (const uint8_t *) &initialized;

  for ( i = 0; i < sizeof( container ); ++i ) {
    if ( container.bytes[ i ] != 0xff ) {
      rtems_test_assert( container.bytes[ i ] == bytes[ i ] );
    }
  }

  memset( &zero_initialized, 0, sizeof( zero_initialized ) );
  _ISR_lock_Set_name( &zero_initialized, name );
  bytes = (const uint8_t *) &zero_initialized;

  for ( i = 0; i < sizeof( container ); ++i ) {
    if ( container.bytes[ i ] != 0xff ) {
      rtems_test_assert( container.bytes[ i ] == bytes[ i ] );
    }
  }

  _ISR_lock_ISR_disable_and_acquire( &container.lock, &lock_context );
  rtems_test_assert( normal_interrupt_level != _ISR_Get_level() );
  _ISR_lock_Release_and_ISR_enable( &container.lock, &lock_context );

  rtems_test_assert( normal_interrupt_level == _ISR_Get_level() );

  _ISR_lock_ISR_disable( &lock_context );
  rtems_test_assert( normal_interrupt_level != _ISR_Get_level() );
  _ISR_lock_ISR_enable( &lock_context );

  rtems_test_assert( normal_interrupt_level == _ISR_Get_level() );

#if defined(RTEMS_DEBUG)
  _ISR_lock_ISR_disable( &lock_context );
#endif
  interrupt_level = _ISR_Get_level();
  _ISR_lock_Acquire( &container.lock, &lock_context );
  rtems_test_assert( interrupt_level == _ISR_Get_level() );
#if !defined(RTEMS_DEBUG)
  rtems_test_assert( normal_interrupt_level == _ISR_Get_level() );
#endif
  _ISR_lock_Release( &container.lock, &lock_context );
  rtems_test_assert( interrupt_level == _ISR_Get_level() );
#if defined(RTEMS_DEBUG)
  _ISR_lock_ISR_enable( &lock_context );
#endif

  rtems_test_assert( normal_interrupt_level == _ISR_Get_level() );

  _ISR_lock_Destroy( &container.lock );
  _ISR_lock_Destroy( &initialized );
}

static rtems_mode get_interrupt_level( void )
{
  rtems_status_code sc;
  rtems_mode mode;

  sc = rtems_task_mode( RTEMS_CURRENT_MODE, RTEMS_CURRENT_MODE, &mode );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  return mode & RTEMS_INTERRUPT_MASK;
}

static void test_interrupt_locks( void )
{
  rtems_mode normal_interrupt_level = get_interrupt_level();
  rtems_interrupt_lock initialized = RTEMS_INTERRUPT_LOCK_INITIALIZER("test");
  union {
    rtems_interrupt_lock lock;
    uint8_t bytes[ sizeof( rtems_interrupt_lock ) ];
  } container;
  rtems_interrupt_lock_context lock_context;
  size_t i;
  const uint8_t *initialized_bytes;

  rtems_interrupt_lock_initialize( &container.lock, "test" );
  initialized_bytes = (const uint8_t *) &initialized;

  for ( i = 0; i < sizeof( container ); ++i ) {
    if ( container.bytes[ i ] != 0xff ) {
      rtems_test_assert( container.bytes[ i ] == initialized_bytes[ i] );
    }
  }

  rtems_interrupt_lock_acquire( &container.lock, &lock_context );
  rtems_test_assert( normal_interrupt_level != get_interrupt_level() );
  rtems_interrupt_lock_release( &container.lock, &lock_context );

  rtems_test_assert( normal_interrupt_level == get_interrupt_level() );

  rtems_interrupt_lock_acquire_isr( &container.lock, &lock_context );
  rtems_test_assert( normal_interrupt_level == get_interrupt_level() );
  rtems_interrupt_lock_release_isr( &container.lock, &lock_context );

  rtems_test_assert( normal_interrupt_level == get_interrupt_level() );

  rtems_interrupt_lock_destroy( &container.lock );
  rtems_interrupt_lock_destroy( &initialized );
}

static void test_clock_tick_functions( void )
{
  rtems_interrupt_level level;
  Watchdog_Interval saved_ticks;

  rtems_interrupt_local_disable( level );

  saved_ticks = _Watchdog_Ticks_since_boot;

  _Watchdog_Ticks_since_boot = 0xdeadbeef;
  rtems_test_assert( rtems_clock_get_ticks_since_boot() == 0xdeadbeef );

  rtems_test_assert( rtems_clock_tick_later( 0 ) == 0xdeadbeef );
  rtems_test_assert( rtems_clock_tick_later( 0x8160311e ) == 0x600df00d );

  _Watchdog_Ticks_since_boot = 0;
  rtems_test_assert( rtems_clock_tick_later_usec( 0 ) == 1 );
  rtems_test_assert( rtems_clock_tick_later_usec( 1 ) == 2 );
  rtems_test_assert( rtems_clock_tick_later_usec( US_PER_TICK ) == 2 );
  rtems_test_assert( rtems_clock_tick_later_usec( US_PER_TICK + 1 ) == 3 );

  _Watchdog_Ticks_since_boot = 0;
  rtems_test_assert( !rtems_clock_tick_before( 0xffffffff ) );
  rtems_test_assert( !rtems_clock_tick_before( 0 ) );
  rtems_test_assert( rtems_clock_tick_before( 1 ) );

  _Watchdog_Ticks_since_boot = 1;
  rtems_test_assert( !rtems_clock_tick_before( 0 ) );
  rtems_test_assert( !rtems_clock_tick_before( 1 ) );
  rtems_test_assert( rtems_clock_tick_before( 2 ) );

  _Watchdog_Ticks_since_boot = 0x7fffffff;
  rtems_test_assert( !rtems_clock_tick_before( 0x7ffffffe ) );
  rtems_test_assert( !rtems_clock_tick_before( 0x7fffffff ) );
  rtems_test_assert( rtems_clock_tick_before( 0x80000000 ) );

  _Watchdog_Ticks_since_boot = 0x80000000;
  rtems_test_assert( !rtems_clock_tick_before( 0x7fffffff ) );
  rtems_test_assert( !rtems_clock_tick_before( 0x80000000 ) );
  rtems_test_assert( rtems_clock_tick_before( 0x80000001 ) );

  _Watchdog_Ticks_since_boot = 0xffffffff;
  rtems_test_assert( !rtems_clock_tick_before( 0xfffffffe ) );
  rtems_test_assert( !rtems_clock_tick_before( 0xffffffff ) );
  rtems_test_assert( rtems_clock_tick_before( 0 ) );

  _Watchdog_Ticks_since_boot = saved_ticks;

  rtems_interrupt_local_enable( level );
}

void test_interrupt_inline(void)
{
  rtems_interrupt_level level;
  rtems_interrupt_level level_1;
  rtems_mode            level_mode_body;
  rtems_mode            level_mode_macro;
  bool                  in_isr;
  uint32_t              isr_level_0;
  uint32_t              isr_level_1;
  uint32_t              isr_level_2;

  puts( "interrupt is in progress (use body)" );
  in_isr = rtems_interrupt_is_in_progress();
  if ( in_isr ) {
    puts( "interrupt reported to be is in progress (body)" );
    rtems_test_exit( 0 );
  }

#if !defined(RTEMS_SMP)
  puts( "interrupt disable (use inline)" );
  rtems_interrupt_disable( level );

  puts( "interrupt flash (use inline)" );
  rtems_interrupt_flash( level );

  puts( "interrupt enable (use inline)" );
  rtems_interrupt_enable( level );
#endif /* RTEMS_SMP */

  isr_level_0 = _ISR_Get_level();
  rtems_test_assert( isr_level_0 == 0 );

  rtems_interrupt_local_disable( level );
  isr_level_1 = _ISR_Get_level();
  rtems_test_assert( isr_level_1 != isr_level_0 );
  rtems_test_assert( _ISR_Is_enabled( level ) );

  rtems_interrupt_local_disable( level_1 );
  isr_level_2 = _ISR_Get_level();
  rtems_test_assert( isr_level_2 == isr_level_1 );
  rtems_test_assert( !_ISR_Is_enabled( level_1 ) );

  rtems_interrupt_local_enable( level_1 );
  rtems_test_assert( _ISR_Get_level() == isr_level_1 );

  rtems_interrupt_local_enable( level );
  rtems_test_assert( _ISR_Get_level() == isr_level_0 );

  puts( "interrupt level mode (use inline)" );
  level_mode_body = rtems_interrupt_level_body( level );
  level_mode_macro = RTEMS_INTERRUPT_LEVEL(level);
  if ( level_mode_macro == level_mode_body ) {
    puts( "test case working.." );
  }
}

volatile int isr_in_progress_body;

volatile int isr_in_progress_inline;

void check_isr_in_progress_inline(void)
{
  isr_in_progress_inline = rtems_interrupt_is_in_progress() ? 1 : 2;
}

void check_isr_worked(
  char *s,
  int   result
)
{
  switch (result) {
    case 0:
      printf( "isr_in_progress(%s) timer did not fire\n", s );
      rtems_test_exit(0);
      break;
    case 1:
      printf( "isr_in_progress(%s) from ISR -- OK\n", s );
      break;
    default:
      printf( "isr_in_progress(%s) from ISR -- returned bad value\n", s);
      rtems_test_exit(0);
      break;
  }
}

volatile int blocked_task_status;
rtems_id     blocked_task_id;

rtems_task blocked_task(
  rtems_task_argument argument
)
{
  rtems_status_code     status;

  puts( "Blocking task... suspending self" );
  blocked_task_status = 1;
  status = rtems_task_suspend( RTEMS_SELF );
  directive_failed( status, "rtems_task_suspend" );

  blocked_task_status = 3;
  rtems_task_exit();
}

/*
 *  Timer Service Routine
 *
 *  If we are in an ISR, then this is a normal clock tick.
 *  If we are not, then it is the test case.
 */
rtems_timer_service_routine test_unblock_task(
  rtems_id  timer,
  void     *arg
)
{
  bool               in_isr;
  rtems_status_code  status;
  Per_CPU_Control   *cpu_self;

  in_isr = rtems_interrupt_is_in_progress();
  status = rtems_task_is_suspended( blocked_task_id );
  if ( in_isr ) {
    status = rtems_timer_fire_after( timer, 1, test_unblock_task, NULL );
    directive_failed( status, "timer_fire_after failed" );
    return;
  }

  if ( (status != RTEMS_ALREADY_SUSPENDED) ) {
    status = rtems_timer_fire_after( timer, 1, test_unblock_task, NULL );
    directive_failed( status, "timer_fire_after failed" );
    return;
  }

  blocked_task_status = 2;
  cpu_self = _Thread_Dispatch_disable();
  status = rtems_task_resume( blocked_task_id );
  _Thread_Dispatch_enable( cpu_self );
  directive_failed( status, "rtems_task_resume" );
}

#undef rtems_interrupt_disable
extern rtems_interrupt_level rtems_interrupt_disable(void);
#undef rtems_interrupt_enable
extern void rtems_interrupt_enable(rtems_interrupt_level previous_level);
#undef rtems_interrupt_flash
extern void rtems_interrupt_flash(rtems_interrupt_level previous_level);
#undef rtems_interrupt_is_in_progress
extern bool rtems_interrupt_is_in_progress(void);

static void test_interrupt_body(void)
{
#if !defined(RTEMS_SMP)
  rtems_interrupt_level level_0;
  rtems_interrupt_level level_1;
  rtems_mode            level_mode_body;
  rtems_mode            level_mode_macro;
  bool                  in_isr;

  puts( "interrupt disable (use body)" );
  level_0 = rtems_interrupt_disable();

  puts( "interrupt disable (use body)" );
  level_1 = rtems_interrupt_disable();

  puts( "interrupt flash (use body)" );
  rtems_interrupt_flash( level_1 );

  puts( "interrupt enable (use body)" );
  rtems_interrupt_enable( level_1 );

  puts( "interrupt level mode (use body)" );
  level_mode_body = rtems_interrupt_level_body( level_0 );
  level_mode_macro = RTEMS_INTERRUPT_LEVEL( level_0 );
  if ( level_mode_macro == level_mode_body ) {
    puts("test seems to work");
  }

  /*
   *  Test interrupt bodies
   */
  puts( "interrupt is in progress (use body)" );
  in_isr = rtems_interrupt_is_in_progress();

  puts( "interrupt enable (use body)" );
  rtems_interrupt_enable( level_0 );

  if ( in_isr ) {
    puts( "interrupt reported to be is in progress (body)" );
    rtems_test_exit( 0 );
  }
#endif /* RTEMS_SMP */
}

rtems_timer_service_routine test_isr_in_progress(
  rtems_id  timer,
  void     *arg
)
{
  check_isr_in_progress_inline();

  isr_in_progress_body = rtems_interrupt_is_in_progress() ? 1 : 2;
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_time_of_day     time;
  rtems_status_code     status;
  rtems_id              timer;
  int                   i;

  TEST_BEGIN();

  test_isr_level();
  test_isr_locks();
  test_interrupt_locks();

  build_time( &time, 12, 31, 1988, 9, 0, 0, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  /*
   *  Timer used in multiple ways
   */
  status = rtems_timer_create( 1, &timer );
  directive_failed( status, "rtems_timer_create" );

  /*
   *  Test clock tick from outside ISR
   */
  status = rtems_clock_tick();
  directive_failed( status, "rtems_clock_tick" );
  puts( "clock_tick from task level" );

  test_clock_tick_functions();

  /*
   *  Now do a dispatch directly out of a clock tick that is
   *  called from a task.  We need to create a task that will
   *  block so we have one to unblock.  Then we schedule a TSR
   *  to run in the clock tick but it has to be careful to
   *  make sure it is not called from an ISR and that the
   *  dispatching critical section is managed properly to
   *  make the dispatch happen.
   */

  blocked_task_status = -1;

  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '1', ' ' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &blocked_task_id
  );
  directive_failed( status, "rtems_task_create" );

  status = rtems_task_start( blocked_task_id, blocked_task, 0 );
  directive_failed( status, "rtems_task_start" );

  status = rtems_task_wake_after( 10 );
  directive_failed( status, "rtems_task_wake_after" );

  status = rtems_timer_fire_after( timer, 1, test_unblock_task, NULL );
  directive_failed( status, "timer_fire_after failed" );

  /* we expect to be preempted from this call */
  for ( i=0 ; i<100 && blocked_task_status != 3 ; i++ ) {
    status = rtems_clock_tick();
    directive_failed( status, "rtems_clock_tick" );
  }
  switch ( blocked_task_status ) {
     case -1:
       puts(
         "clock_tick with task preempt -- task blocked, timer did not fire"
       );
       rtems_test_exit(0);
       break;
     case 1:
       puts( "clock_tick with task preempt -- timer fired case 1" );
       rtems_test_exit(0);
       break;
     case 2:
       puts( "clock_tick with task preempt -- timer fired case 2" );
       rtems_test_exit(0);
       break;
     case 3:
       puts( "clock_tick from task level with preempt -- OK" );
       break;
  }

  test_interrupt_inline();
  test_interrupt_body();

  /*
   * Test ISR in progress from actual ISR
   */
  status = rtems_timer_fire_after( timer, 10, test_isr_in_progress, NULL );
  directive_failed( status, "timer_fire_after failed" );

  status = rtems_task_wake_after( 11 );
  directive_failed( status, "wake_after failed" );

  check_isr_worked( "inline", isr_in_progress_inline );

  check_isr_worked( "body", isr_in_progress_body );

  TEST_END();
  rtems_test_exit( 0 );
}
