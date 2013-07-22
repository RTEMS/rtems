/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__ 1
#include <tmacros.h>
#include <intrcritical.h>

#include <rtems/rtems/semimpl.h>
#include <rtems/score/watchdogimpl.h>

static rtems_id Semaphore;
static bool case_hit;

static Thread_blocking_operation_States getState(void)
{
  Objects_Locations  location;
  Semaphore_Control *sem;

  sem = (Semaphore_Control *)_Objects_Get(
    &_Semaphore_Information, Semaphore, &location );
  if ( location != OBJECTS_LOCAL ) {
    puts( "Bad object lookup" );
    rtems_test_exit(0);
  }
  _Thread_Unnest_dispatch();

  return sem->Core_control.semaphore.Wait_queue.sync_state;
}

static rtems_timer_service_routine test_release_from_isr(
  rtems_id  timer,
  void     *arg
)
{
  Chain_Control *chain = &_Watchdog_Ticks_chain;

  if ( !_Chain_Is_empty( chain ) ) {
    Watchdog_Control *watchdog = _Watchdog_First( chain );

    if (
      watchdog->delta_interval == 0
        && watchdog->routine == _Thread_queue_Timeout
    ) {
      Watchdog_States state = _Watchdog_Remove( watchdog );

      rtems_test_assert( state == WATCHDOG_ACTIVE );
      (*watchdog->routine)( watchdog->id, watchdog->user_data );

      if ( getState() == THREAD_BLOCKING_OPERATION_TIMEOUT ) {
        case_hit = true;
      }
    }
  }
}

static rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code     sc;
  int                   resets;

  puts( "\n\n*** TEST INTERRUPT CRITICAL SECTION 09 ***" );

  puts( "Init - Test may not be able to detect case is hit reliably" );
  puts( "Init - Trying to generate timeout from ISR while blocking" );
  sc = rtems_semaphore_create(
    rtems_build_name( 'S', 'M', '1', ' ' ),
    0,
    RTEMS_DEFAULT_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    &Semaphore
  );
  directive_failed( sc, "rtems_semaphore_create of SM1" );

  interrupt_critical_section_test_support_initialize( test_release_from_isr );

  case_hit = false;

  for (resets=0 ; resets< 2 ;) {
    if ( interrupt_critical_section_test_support_delay() )
      resets++;

    (void) rtems_semaphore_obtain( Semaphore, RTEMS_DEFAULT_OPTIONS, 1 );
  }

  if ( case_hit ) {
    puts( "Init - It appears the case has been hit" );
    puts( "*** END OF TEST INTERRUPT CRITICAL SECTION 09 ***" );
  } else
    puts( "Init - Case not hit - ran too long" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS       1
#define CONFIGURE_MAXIMUM_TIMERS      1
#define CONFIGURE_MAXIMUM_SEMAPHORES  1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MICROSECONDS_PER_TICK  1000

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
