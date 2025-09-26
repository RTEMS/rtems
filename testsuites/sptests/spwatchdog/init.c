/* SPDX-License-Identifier: BSD-2-Clause */

/*  Init
 *
 *  This routine is the XXX.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

#include <rtems/score/watchdogimpl.h>

const char rtems_test_name[] = "SPWATCHDOG";

typedef struct {
  Watchdog_Control Base;
  int counter;
} test_watchdog;

static void test_watchdog_routine( Watchdog_Control *base )
{
  test_watchdog *watchdog = (test_watchdog *) base;

  ++watchdog->counter;
}

static void test_watchdog_static_init( void )
{
  static Watchdog_Control a = WATCHDOG_INITIALIZER(
    test_watchdog_routine
  );
  Watchdog_Control b;

  memset( &b, 0, sizeof( b ) );
  _Watchdog_Preinitialize( &b, _Per_CPU_Get_by_index( 0 ) );
  _Watchdog_Initialize(
    &b,
    test_watchdog_routine
  );

  rtems_test_assert( memcmp( &a, &b, sizeof( a ) ) == 0 );
}

static void test_watchdog_config( void )
{
  rtems_test_assert( _Watchdog_Nanoseconds_per_tick == 10000000 );
  rtems_test_assert( _Watchdog_Ticks_per_second == 100 );
  rtems_test_assert( rtems_clock_get_ticks_per_second() == 100 );
  #undef rtems_clock_get_ticks_per_second
  rtems_test_assert( rtems_clock_get_ticks_per_second() == 100 );
}

static bool test_watchdog_is_inactive( test_watchdog *watchdog )
{
  return _Watchdog_Get_state( &watchdog->Base ) == WATCHDOG_INACTIVE;
}

static void test_watchdog_init( test_watchdog *watchdog, int counter )
{
  _Watchdog_Preinitialize( &watchdog->Base, _Per_CPU_Get_snapshot() );
  _Watchdog_Initialize( &watchdog->Base, test_watchdog_routine );
  rtems_test_assert( test_watchdog_is_inactive( watchdog ) ) ;
  watchdog->counter = counter;
}

static uint64_t test_watchdog_tick( Watchdog_Header *header, uint64_t now )
{
#if ISR_LOCK_NEEDS_OBJECT
  ISR_lock_Control lock = ISR_LOCK_INITIALIZER( "Test" );
#endif
  ISR_lock_Context lock_context;
  Watchdog_Control *first;

  _ISR_lock_ISR_disable_and_acquire( &lock, &lock_context );

  ++now;
  first = _Watchdog_Header_first( header );

  if ( first != NULL ) {
    _Watchdog_Tickle( header, first, now, &lock, &lock_context );
  }

  _ISR_lock_Release_and_ISR_enable( &lock, &lock_context );
  _ISR_lock_Destroy( &lock );

  return now;
}

static void test_watchdog_operations( void )
{
  Watchdog_Header header;
  uint64_t now;
  test_watchdog a;
  test_watchdog b;
  test_watchdog c;

  _Watchdog_Header_initialize( &header );
  rtems_test_assert( _RBTree_Is_empty( &header.Watchdogs ) );
  rtems_test_assert( header.first == NULL );

  test_watchdog_init( &a, 10 );
  test_watchdog_init( &b, 20 );
  test_watchdog_init( &c, 30 );

  now = 0;
  now = test_watchdog_tick( &header, now );

  _Watchdog_Insert( &header, &a.Base, now + 1 );
  rtems_test_assert( header.first == &a.Base.Node.RBTree );
  rtems_test_assert( !test_watchdog_is_inactive( &a ) ) ;
  rtems_test_assert( a.Base.expire == 2 );
  rtems_test_assert( a.counter == 10 );

  _Watchdog_Remove( &header, &a.Base );
  rtems_test_assert( header.first == NULL );
  rtems_test_assert( test_watchdog_is_inactive( &a ) ) ;
  rtems_test_assert( a.Base.expire == 2 );
  rtems_test_assert( a.counter == 10 );

  _Watchdog_Remove( &header, &a.Base );
  rtems_test_assert( header.first == NULL );
  rtems_test_assert( test_watchdog_is_inactive( &a ) ) ;
  rtems_test_assert( a.Base.expire == 2 );
  rtems_test_assert( a.counter == 10 );

  _Watchdog_Insert( &header, &a.Base, now + 1 );
  rtems_test_assert( header.first == &a.Base.Node.RBTree );
  rtems_test_assert( !test_watchdog_is_inactive( &a ) ) ;
  rtems_test_assert( a.Base.expire == 2 );
  rtems_test_assert( a.counter == 10 );

  _Watchdog_Insert( &header, &b.Base, now + 1 );
  rtems_test_assert( header.first == &a.Base.Node.RBTree );
  rtems_test_assert( !test_watchdog_is_inactive( &b ) ) ;
  rtems_test_assert( b.Base.expire == 2 );
  rtems_test_assert( b.counter == 20 );

  _Watchdog_Insert( &header, &c.Base, now + 2 );
  rtems_test_assert( header.first == &a.Base.Node.RBTree );
  rtems_test_assert( !test_watchdog_is_inactive( &c ) ) ;
  rtems_test_assert( c.Base.expire == 3 );
  rtems_test_assert( c.counter == 30 );

  _Watchdog_Remove( &header, &a.Base );
  rtems_test_assert( header.first == &b.Base.Node.RBTree );
  rtems_test_assert( test_watchdog_is_inactive( &a ) ) ;
  rtems_test_assert( a.Base.expire == 2 );
  rtems_test_assert( a.counter == 10 );

  _Watchdog_Remove( &header, &b.Base );
  rtems_test_assert( header.first == &c.Base.Node.RBTree );
  rtems_test_assert( test_watchdog_is_inactive( &b ) ) ;
  rtems_test_assert( b.Base.expire == 2 );
  rtems_test_assert( b.counter == 20 );

  _Watchdog_Remove( &header, &c.Base );
  rtems_test_assert( header.first == NULL );
  rtems_test_assert( test_watchdog_is_inactive( &c ) ) ;
  rtems_test_assert( c.Base.expire == 3 );
  rtems_test_assert( c.counter == 30 );

  _Watchdog_Insert( &header, &a.Base, now + 2 );
  rtems_test_assert( header.first == &a.Base.Node.RBTree );
  rtems_test_assert( !test_watchdog_is_inactive( &a ) ) ;
  rtems_test_assert( a.Base.expire == 3 );
  rtems_test_assert( a.counter == 10 );

  _Watchdog_Insert( &header, &b.Base, now + 2 );
  rtems_test_assert( header.first == &a.Base.Node.RBTree );
  rtems_test_assert( !test_watchdog_is_inactive( &b ) ) ;
  rtems_test_assert( b.Base.expire == 3 );
  rtems_test_assert( b.counter == 20 );

  _Watchdog_Insert( &header, &c.Base, now + 3 );
  rtems_test_assert( header.first == &a.Base.Node.RBTree );
  rtems_test_assert( !test_watchdog_is_inactive( &c ) ) ;
  rtems_test_assert( c.Base.expire == 4 );
  rtems_test_assert( c.counter == 30 );

  now = test_watchdog_tick( &header, now );
  rtems_test_assert( !_RBTree_Is_empty( &header.Watchdogs ) );
  rtems_test_assert( header.first == &a.Base.Node.RBTree );
  rtems_test_assert( !test_watchdog_is_inactive( &a ) ) ;
  rtems_test_assert( a.Base.expire == 3 );
  rtems_test_assert( a.counter == 10 );
  rtems_test_assert( !test_watchdog_is_inactive( &b ) ) ;
  rtems_test_assert( b.Base.expire == 3 );
  rtems_test_assert( b.counter == 20 );
  rtems_test_assert( !test_watchdog_is_inactive( &c ) ) ;
  rtems_test_assert( c.Base.expire == 4 );
  rtems_test_assert( c.counter == 30 );

  now = test_watchdog_tick( &header, now );
  rtems_test_assert( !_RBTree_Is_empty( &header.Watchdogs ) );
  rtems_test_assert( header.first == &c.Base.Node.RBTree );
  rtems_test_assert( test_watchdog_is_inactive( &a ) ) ;
  rtems_test_assert( a.Base.expire == 3 );
  rtems_test_assert( a.counter == 11 );
  rtems_test_assert( test_watchdog_is_inactive( &b ) ) ;
  rtems_test_assert( b.Base.expire == 3 );
  rtems_test_assert( b.counter == 21 );
  rtems_test_assert( !test_watchdog_is_inactive( &c ) ) ;
  rtems_test_assert( c.Base.expire == 4 );
  rtems_test_assert( c.counter == 30 );

  now = test_watchdog_tick( &header, now );
  rtems_test_assert( _RBTree_Is_empty( &header.Watchdogs ) );
  rtems_test_assert( header.first == NULL );
  rtems_test_assert( test_watchdog_is_inactive( &a ) ) ;
  rtems_test_assert( a.Base.expire == 3 );
  rtems_test_assert( a.counter == 11 );
  rtems_test_assert( test_watchdog_is_inactive( &b ) ) ;
  rtems_test_assert( b.Base.expire == 3 );
  rtems_test_assert( b.counter == 21 );
  rtems_test_assert( test_watchdog_is_inactive( &c ) ) ;
  rtems_test_assert( c.Base.expire == 4 );
  rtems_test_assert( c.counter == 31 );

  _Watchdog_Header_destroy( &header );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_time_of_day  time;
  rtems_status_code  status;

  TEST_BEGIN();

  test_watchdog_operations();
  test_watchdog_static_init();
  test_watchdog_config();

  build_time( &time, 12, 31, 1988, 9, 0, 0, 0 );

  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  Task_name[ 1 ]  = rtems_build_name( 'T', 'A', '1', ' ' );
  Timer_name[ 1 ] = rtems_build_name( 'T', 'M', '1', ' ' );

  status = rtems_task_create(
    Task_name[ 1 ],
    1,
    RTEMS_MINIMUM_STACK_SIZE * 2,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TA1" );

  status = rtems_task_start( Task_id[ 1 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  puts( "INIT - rtems_timer_create - creating timer 1" );
  status = rtems_timer_create( Timer_name[ 1 ], &Timer_id[ 1 ] );
  directive_failed( status, "rtems_timer_create" );

  printf( "INIT - timer 1 has id (0x%" PRIxrtems_id ")\n", Timer_id[ 1 ] );

  /* Task_1() will end the test */
  rtems_task_exit();
}
