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

static void test_watchdog_routine( Objects_Id id, void *arg )
{
  (void) id;
  (void) arg;

  rtems_test_assert( 0 );
}

static void init_watchdogs(
  Watchdog_Header *header,
  Watchdog_Control watchdogs[3]
)
{
  Watchdog_Control *a = &watchdogs[0];
  Watchdog_Control *b = &watchdogs[1];
  Watchdog_Control *c = &watchdogs[2];
  Watchdog_Control *d = &watchdogs[3];

  _Watchdog_Header_initialize( header );
  rtems_test_assert( _Watchdog_Is_empty( header ) );
  rtems_test_assert( _Chain_Is_empty( &header->Iterators ) );

  _Watchdog_Initialize( c, NULL, 0, NULL );
  c->initial = 6;
  _Watchdog_Insert( header, c );
  rtems_test_assert( c->delta_interval == 6 );

  rtems_test_assert( !_Watchdog_Is_empty( header ) );
  rtems_test_assert( _Chain_Is_empty( &header->Iterators ) );

  _Watchdog_Initialize( a, NULL, 0, NULL );
  a->initial = 2;
  _Watchdog_Insert( header, a );
  rtems_test_assert( a->delta_interval == 2 );
  rtems_test_assert( c->delta_interval == 4 );

  _Watchdog_Initialize( b, NULL, 0, NULL );
  b->initial = 4;
  _Watchdog_Insert( header, b );
  rtems_test_assert( a->delta_interval == 2 );
  rtems_test_assert( b->delta_interval == 2 );
  rtems_test_assert( c->delta_interval == 2 );

  _Watchdog_Initialize( d, NULL, 0, NULL );
}

static void destroy_watchdogs(
  Watchdog_Header *header
)
{
  _ISR_lock_Destroy( &header->Lock );
}

static void add_iterator(
  Watchdog_Header *header,
  Watchdog_Iterator *i,
  Watchdog_Control *w
)
{
  _Chain_Append_unprotected( &header->Iterators, &i->Node );
  i->delta_interval = 2;
  i->current = &w->Node;
}

static void test_watchdog_insert_and_remove( void )
{
  Watchdog_Header header;
  Watchdog_Control watchdogs[4];
  Watchdog_Control *a = &watchdogs[0];
  Watchdog_Control *b = &watchdogs[1];
  Watchdog_Control *c = &watchdogs[2];
  Watchdog_Control *d = &watchdogs[3];
  Watchdog_Iterator i;

  init_watchdogs( &header, watchdogs );
  add_iterator( &header, &i, c );

  /* Remove next watchdog of iterator */
  _Watchdog_Remove( &header, c );
  rtems_test_assert( i.delta_interval == 2 );
  rtems_test_assert( i.current == &b->Node );

  /* Remove watchdog before the current watchdog of iterator */
  _Watchdog_Remove( &header, a );
  rtems_test_assert( i.delta_interval == 4 );
  rtems_test_assert( i.current == &b->Node );

  /* Remove current (= last) watchdog of iterator */
  _Watchdog_Remove( &header, b );
  rtems_test_assert( i.delta_interval == 4 );
  rtems_test_assert( i.current == _Chain_Head( &header.Watchdogs ) );

  /* Insert first watchdog */
  a->initial = 1;
  _Watchdog_Insert( &header, a );
  rtems_test_assert( i.delta_interval == 4 );
  rtems_test_assert( i.current == _Chain_Head( &header.Watchdogs ) );

  destroy_watchdogs( &header );
  init_watchdogs( &header, watchdogs );
  add_iterator( &header, &i, b );

  /* Insert right before current watchdog of iterator */
  d->initial = 3;
  _Watchdog_Insert( &header, d );
  rtems_test_assert( i.delta_interval == 1 );
  rtems_test_assert( i.current == &b->Node );

  destroy_watchdogs( &header );
  init_watchdogs( &header, watchdogs );
  add_iterator( &header, &i, b );

  /* Insert right after current watchdog of iterator */
  d->initial = 5;
  _Watchdog_Insert( &header, d );
  rtems_test_assert( i.delta_interval == 2 );
  rtems_test_assert( i.current == &b->Node );

  destroy_watchdogs( &header );
}

static void test_watchdog_static_init( void )
{
  #if defined(RTEMS_USE_16_BIT_OBJECT)
    #define JUNK_ID 0x1234
  #else
    #define JUNK_ID 0x12345678
  #endif

  static Watchdog_Control a = WATCHDOG_INITIALIZER(
    test_watchdog_routine,
    JUNK_ID,
    (void *) 0xdeadbeef
  );
  Watchdog_Control b;

  memset( &b, 0, sizeof( b ) );
  _Watchdog_Initialize(
    &b,
    test_watchdog_routine,
    JUNK_ID,
    (void *) 0xdeadbeef
  );

  rtems_test_assert( memcmp( &a, &b, sizeof( a ) ) == 0 );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_time_of_day  time;
  rtems_status_code  status;

  TEST_BEGIN();

  test_watchdog_static_init();
  test_watchdog_insert_and_remove();

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

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );


  rtems_test_exit( 0 );
}
