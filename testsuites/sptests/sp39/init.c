/*
 *  Classic API Signal to Task from ISR
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

#define TEST_INIT
#include "system.h"

volatile boolean case_hit;

rtems_id main_task;
rtems_id other_task;

rtems_timer_service_routine test_event_from_isr(
  rtems_id  timer,
  void     *arg
)
{
  rtems_status_code     status;

  status = rtems_event_send( main_task, 0x01 );

  if ( _Event_Sync_state == EVENT_SYNC_SATISFIED )
    case_hit = TRUE;
}

rtems_timer_service_routine test_timeout_from_isr(
  rtems_id  timer,
  void     *arg
)
{
  if ( _Event_Sync_state != EVENT_SYNC_NOTHING_HAPPENED )
    return;

  /*
   *  The main task should have timed out and we are in the
   *  event synchronization critical section with "timeout".
   */

  /*
   *  This event send hits the critical section but sends to
   *  another task so doesn't impact this critical section.
   */
  rtems_event_send( other_task, 0x02 );

  /*
   *  This event send hits the main task but doesn't satisfy
   *  it's blocking condition so it will still time out.
   */
  rtems_event_send( main_task, 0x02 );

  /*
   *  This event send should cancel the main task's time out
   *  and deliver the interrupt because both occurred simultaneously.
   */
  rtems_event_send( main_task, 0x01 );


  case_hit = TRUE;
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code     status;
  rtems_id              timer;
  rtems_event_set       out;
  int                   i;
  int                   max;
  int                   iterations = 0;

  puts( "\n\n*** TEST 39 ***" );

  main_task = rtems_task_self();

  /*
   *  Timer used in multiple ways
   */
  status = rtems_timer_create( 1, &timer );
  directive_failed( status, "rtems_timer_create" );

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
   * Test Event send successful from ISR
   */
  case_hit = FALSE;
  max = 1;

  while (1) {
    if ( case_hit )
      break;
    status = rtems_timer_fire_after( timer, 1, test_event_from_isr, NULL );
    directive_failed( status, "timer_fire_after failed" );

    for (i=0 ; i<max ; i++ )
      if ( _Event_Sync_state == EVENT_SYNC_SATISFIED )
        break;

    status = rtems_event_receive( 0x01, RTEMS_DEFAULT_OPTIONS, 0, &out );
    directive_failed( status, "rtems_event_receive" );
    if ( case_hit == TRUE )
      break;
    max += 2;
    if ( ++iterations >= 0x10000 )
      break;
  }

   printf(
     "Event sent from ISR hitting synchronization point has %soccurred\n",
     (( case_hit == TRUE ) ? "" : "NOT ")
  ); 

  /*
   *  Now try for a timeout case
   */
  case_hit = FALSE;
  max = 1;

  while (1) {
    status = rtems_timer_fire_after( timer, 1, test_timeout_from_isr, NULL );
    directive_failed( status, "timer_fire_after failed" );


    for (i=0 ; i<max ; i++ )
      if ( case_hit )
        break;

    status = rtems_event_receive( 0x01, RTEMS_DEFAULT_OPTIONS, 1, &out );
    if ( status == RTEMS_SUCCESSFUL ) {
      break;
    }
    fatal_directive_status( status, RTEMS_TIMEOUT, "event_receive timeout" );
    max += 1;
  }

  printf(
     "Event timeout hitting synchronization point has %soccurred\n",
     (( case_hit == TRUE ) ? "" : "NOT ")
  ); 

  puts( "*** END OF TEST 39 ***" );
  rtems_test_exit( 0 );
}
