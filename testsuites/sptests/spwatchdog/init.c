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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
  ISR_LOCK_DEFINE( , lock, "Test" )
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
