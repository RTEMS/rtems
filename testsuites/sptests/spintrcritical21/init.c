/*
 *  Classic API Signal to Task from ISR
 *
 *  COPYRIGHT (c) 1989-2011.
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

#include <intrcritical.h>

#include <rtems/score/threadimpl.h>
#include <rtems/rtems/eventimpl.h>

const char rtems_test_name[] = "SPINTRCRITICAL 21";

/*
 *  ERROR CHECKING NOTE:
 *
 *  We are either at dispatch disable level 1 or 2.  Either way, it is
 *  safer not to check the dispatch level explicitly so we are using
 *  fatal_directive_check_status_only() not directive_failed().
 */

static volatile bool case_hit;

static rtems_id main_task;

static Thread_Control *main_thread;

static rtems_id other_task;

static bool is_case_hit( void )
{
  return _Thread_Wait_flags_get( main_thread)
    == ( THREAD_WAIT_CLASS_EVENT | THREAD_WAIT_STATE_INTEND_TO_BLOCK );
}

static rtems_timer_service_routine test_event_from_isr(
  rtems_id  timer,
  void     *arg
)
{
  rtems_status_code     status;

  if ( is_case_hit() ) {
    /*
     *  This event send hits the critical section but sends to
     *  another task so doesn't impact this critical section.
     */
    status = rtems_event_send( other_task, 0x02 );
    fatal_directive_check_status_only( status, RTEMS_SUCCESSFUL, "event send" );

    /*
     *  This event send hits the main task but doesn't satisfy
     *  it's blocking condition so it will still block
     */
    status = rtems_event_send( main_task, 0x02 );
    fatal_directive_check_status_only( status, RTEMS_SUCCESSFUL, "event send" );

    case_hit = true;
  }
  status = rtems_event_send( main_task, 0x01 );
  fatal_directive_check_status_only( status, RTEMS_SUCCESSFUL, "event send" );
}

static bool test_body_event_from_isr( void *arg )
{
  rtems_status_code status;
  rtems_event_set   out;

  (void) arg;

  status = rtems_event_receive( 0x01, RTEMS_DEFAULT_OPTIONS, 0, &out );
  rtems_test_assert( status == RTEMS_SUCCESSFUL );

  return case_hit;
}

static rtems_timer_service_routine test_event_with_timeout_from_isr(
  rtems_id  timer,
  void     *arg
)
{
  rtems_status_code     status;

  if ( is_case_hit() ) {
    /*
     *  We want to catch the task while it is blocking.  Otherwise
     *  just send and make it happy.
     */
    case_hit = true;
  }
  status = rtems_event_send( main_task, 0x01 );
  fatal_directive_check_status_only( status, RTEMS_SUCCESSFUL, "event send" );
}

static bool test_body_event_with_timeout_from_isr( void *arg )
{
  rtems_status_code status;
  rtems_event_set   out;

  (void) arg;

  status = rtems_event_receive( 0x01, RTEMS_DEFAULT_OPTIONS, 1, &out );
  rtems_test_assert( status == RTEMS_SUCCESSFUL || status == RTEMS_TIMEOUT );

  return case_hit;
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code     status;

  TEST_BEGIN();

  main_task = rtems_task_self();
  main_thread = _Thread_Get_executing();

  status = rtems_task_create(
    0xa5a5a5a5,
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &other_task
  );
  directive_failed( status, "rtems_task_create" );

  /*
   * Test Event send successful from ISR -- receive is forever
   */

  case_hit = false;
  interrupt_critical_section_test(
    test_body_event_from_isr,
    NULL,
    test_event_from_isr
  );

  printf(
    "Event sent from ISR hitting synchronization point has %soccurred\n",
    case_hit ? "" : "NOT "
  );

  rtems_test_assert( case_hit );

  /*
   * Test Event send successful from ISR -- receive has timeout
   */

  case_hit = false;
  interrupt_critical_section_test(
    test_body_event_with_timeout_from_isr,
    NULL,
    test_event_with_timeout_from_isr
  );

  printf(
    "Event sent from ISR (with timeout) hitting synchronization "
      "point has %soccurred\n",
    case_hit ? "" : "NOT "
  );

  rtems_test_assert( case_hit );

  TEST_END();
  rtems_test_exit( 0 );
}
