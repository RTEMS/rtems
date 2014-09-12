/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <intrcritical.h>

#define INTERRUPT_CRITICAL_NAME rtems_build_name( 'I', 'C', 'R', 'I' )

typedef struct {
  rtems_interval minimum;
  rtems_interval maximum;
  rtems_interval maximum_current;
  rtems_timer_service_routine_entry tsr;
  rtems_id timer;
  uint64_t t0;
  uint64_t t1;
} interrupt_critical_control;

static interrupt_critical_control interrupt_critical;

static rtems_interval estimate_busy_loop_maximum( void )
{
  rtems_interval units = 0;
  rtems_interval initial = rtems_clock_get_ticks_since_boot();

  while ( initial == rtems_clock_get_ticks_since_boot() ) {
    ++units;
  }

  return units;
}

static rtems_interval wait_for_tick_change( void )
{
  rtems_interval initial = rtems_clock_get_ticks_since_boot();
  rtems_interval now;

  do {
    now = rtems_clock_get_ticks_since_boot();
  } while ( now == initial );

  return now;
}

/*
 * It is important that we use actually use the same busy() function at the
 * various places, since otherwise the obtained maximum value might be wrong.
 * So the compiler must not inline this function.
 */
static __attribute__( ( noinline ) ) void busy( rtems_interval max )
{
  rtems_interval i = 0;

  do {
    __asm__ volatile ("");
    ++i;
  } while ( i < max );
}

static bool interrupt_critical_busy_wait( void )
{
  rtems_interval max = interrupt_critical.maximum_current;
  bool reset = max <= interrupt_critical.minimum;

  if ( reset ) {
    interrupt_critical.maximum_current = interrupt_critical.maximum;
  } else {
    interrupt_critical.maximum_current = max - 1;
  }

  busy( max );

  return reset;
}

void interrupt_critical_section_test_support_initialize(
  rtems_timer_service_routine_entry tsr
)
{
  rtems_interval last;
  rtems_interval now;
  rtems_interval a;
  rtems_interval b;
  rtems_interval m;

  interrupt_critical.tsr = tsr;

  if ( tsr != NULL && interrupt_critical.timer == 0 ) {
    rtems_status_code sc = rtems_timer_create(
      INTERRUPT_CRITICAL_NAME,
      &interrupt_critical.timer
    );
    rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  }

  /* Choose a lower bound */
  a = 1;

  /* Estimate an upper bound */

  wait_for_tick_change();
  b = 2 * estimate_busy_loop_maximum();

  while ( true ) {
    last = wait_for_tick_change();
    busy( b );
    now = rtems_clock_get_ticks_since_boot();

    if ( now != last ) {
      break;
    }

    b *= 2;
    last = now;
  }

  /* Find a good value */
  do {
    m = ( a + b ) / 2;

    last = wait_for_tick_change();
    busy( m );
    now = rtems_clock_get_ticks_since_boot();

    if ( now != last ) {
      b = m;
    } else {
      a = m;
    }
  } while ( b - a > 1 );

  interrupt_critical.minimum = 0;
  interrupt_critical.maximum = m;
  interrupt_critical.maximum_current = m;
}

static void timer_fire_after(void)
{
  if ( interrupt_critical.tsr != NULL ) {
    rtems_status_code sc = rtems_timer_fire_after(
      interrupt_critical.timer,
      1,
      interrupt_critical.tsr,
      NULL
    );
    rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  }
}

bool interrupt_critical_section_test_support_delay(void)
{
  timer_fire_after();

  return interrupt_critical_busy_wait();
}

static bool is_idle( const Thread_Control *thread )
{
  return thread->Start.entry_point
    == (Thread_Entry) rtems_configuration_get_idle_task();
}

static void thread_switch( Thread_Control *executing, Thread_Control *heir )
{
  (void) executing;
  (void) heir;

  if ( interrupt_critical.t1 == 0 && is_idle( heir ) ) {
    interrupt_critical.t1 = rtems_clock_get_uptime_nanoseconds();
  }
}

static const rtems_extensions_table extensions = {
  .thread_switch = thread_switch
};

bool interrupt_critical_section_test(
  bool                              ( *test_body )( void * ),
  void                                *test_body_arg,
  rtems_timer_service_routine_entry    tsr
)
{
  bool done;
  rtems_status_code sc;
  rtems_id id;
  uint64_t delta;
  rtems_interval busy_delta;
  int retries = 3;

  interrupt_critical_section_test_support_initialize( tsr );

  sc = rtems_extension_create(
    INTERRUPT_CRITICAL_NAME,
    &extensions,
    &id
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  wait_for_tick_change();
  timer_fire_after();

  /* Get estimate for test body duration */
  interrupt_critical.t0 = rtems_clock_get_uptime_nanoseconds();
  done = ( *test_body )( test_body_arg );
  if ( interrupt_critical.t1 == 0 ) {
    interrupt_critical.t1 = rtems_clock_get_uptime_nanoseconds();
  }

  /* Update minimum */

  delta = interrupt_critical.t1 - interrupt_critical.t0;
  busy_delta = (rtems_interval)
    ( ( interrupt_critical.maximum * ( 2 * delta ) )
      / rtems_configuration_get_nanoseconds_per_tick() );

  if ( busy_delta < interrupt_critical.maximum ) {
    interrupt_critical.minimum = interrupt_critical.maximum - busy_delta;
  }

  sc = rtems_extension_delete( id );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  while ( !done && retries >= 0 ) {
    wait_for_tick_change();

    if ( interrupt_critical_section_test_support_delay() ) {
      --retries;
    }

    done = ( *test_body )( test_body_arg );
  }

  return done;
}
