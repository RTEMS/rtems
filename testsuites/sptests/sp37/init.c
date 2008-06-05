/*
 *  Interrupt Disable/Enable Tests
 *  Clock Tick from task level
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#define CONFIGURE_INIT
#include "system.h"

void test_interrupt_inline(void)
{
  rtems_interrupt_level level;
  boolean               in_isr;

  puts( "interrupt is in progress (use body)" );
  in_isr = rtems_interrupt_is_in_progress();
  if ( in_isr ) {
    puts( "interrupt reported to be is in progress (body)" );
    rtems_test_exit( 0 );
  }

  puts( "interrupt disable (use inline)" );
  rtems_interrupt_disable( level );

  puts( "interrupt flash (use inline)" );
  rtems_interrupt_flash( level );

  puts( "interrupt enable (use inline)" );
  rtems_interrupt_enable( level );
}

volatile int isr_in_progress_body;
volatile int isr_in_progress_inline;

void check_isr_in_progress_inline(void)
{
  boolean    in_isr;

  in_isr = rtems_interrupt_is_in_progress();
  isr_in_progress_inline = ( in_isr ) ? 1 : 2;
}

#undef rtems_interrupt_disable
#undef rtems_interrupt_enable
#undef rtems_interrupt_flash
#undef rtems_interrupt_is_in_progress

rtems_timer_service_routine test_isr_in_progress(
  rtems_id  timer,
  void     *arg
)
{
  boolean    in_isr;

  check_isr_in_progress_inline();

  in_isr = rtems_interrupt_is_in_progress();
  isr_in_progress_body = ( in_isr ) ? 1 : 2;
}

void check_isr_worked(
  char *s,
  int   result
)
{
  switch (result) {
    case -1:
      printf( "isr_in_progress(%s) timer did not fire\n", s );
      break;
    case 1:
      printf( "isr_in_progress(%s) from ISR -- OK\n", s );
      break;
    case 2:
      printf( "isr_in_progress(%s) from ISR -- returned bad value\n");
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
  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete" );
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
  boolean           in_isr;
  rtems_status_code status;

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
  _Thread_Disable_dispatch();
  status = rtems_task_resume( blocked_task_id );
  _Thread_Unnest_dispatch();
  directive_failed( status, "rtems_task_resume" );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_time_of_day     time;
  rtems_status_code     status;
  rtems_interrupt_level level;
  boolean               in_isr;
  rtems_id              timer;
  int                   i;

  puts( "\n\n*** TEST 37 ***" );

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
       puts( "clock_tick with task preempt -- task blocked, timer did not fire" );
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

  /*
   *  Test interrupt inline versions
   */
  test_interrupt_inline();

  /*
   *  Test interrupt bodies
   */
  puts( "interrupt is in progress (use body)" );
  in_isr = rtems_interrupt_is_in_progress();
  if ( in_isr ) {
    puts( "interrupt reported to be is in progress (body)" );
    rtems_test_exit( 0 );
  }

  puts( "interrupt disable (use body)" );
  level = rtems_interrupt_disable();

  puts( "interrupt disable (use body)" );
  level = rtems_interrupt_disable();

  puts( "interrupt flash (use body)" );
  rtems_interrupt_flash( level );

  puts( "interrupt enable (use body)" );
  rtems_interrupt_enable( level );

  /*
   * Test ISR in progress from actual ISR
   */
  isr_in_progress_body   = -1;
  isr_in_progress_inline = -1;

  status = rtems_timer_fire_after( timer, 10, test_isr_in_progress, NULL );
  directive_failed( status, "timer_fire_after failed" );

  status = rtems_task_wake_after( 100 );
  directive_failed( status, "wake_after failed" );
  
  check_isr_worked( "inline", isr_in_progress_body );

  check_isr_worked( "body", isr_in_progress_body );


  puts( "*** END OF TEST 37 ***" );
  rtems_test_exit( 0 );
}
