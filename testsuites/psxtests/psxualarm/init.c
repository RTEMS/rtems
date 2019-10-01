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
#include <signal.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

const char rtems_test_name[] = "PSXUALARM";

static volatile int Signal_count;

static void Signal_handler(
  int signo
)
{
  rtems_test_assert( signo == SIGALRM );
  ++Signal_count;
}

void *POSIX_Init(
  void *argument
)
{
  int               status;
  useconds_t        result;
  struct sigaction  act;
  sigset_t          mask;

  TEST_BEGIN();

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08" PRIxpthread_t "\n", Init_id );

  /* Block SIGALRM */
  status = sigemptyset( &mask );
  rtems_test_assert( status == 0 );
  status = sigaddset( &mask, SIGALRM );
  rtems_test_assert( !status );
  status = sigprocmask( SIG_BLOCK, &mask, NULL );
  rtems_test_assert( status == 0 );

  /* Validate ualarm is ignored if signal not caught */
  act.sa_handler = Signal_handler;
  act.sa_flags   = 0;
  sigaction( SIGALRM, &act, NULL );
  puts( "Init: ualarm in 500000 us" );
  result = ualarm( 500000, 0 );
  rtems_test_assert( result == 0 );

  status = sleep( 1 );
  rtems_test_assert( status == 0 );
  rtems_test_assert( Signal_count == 0 );

  /* unblock Signal and see if it happened */
  status = sigemptyset( &mask );
  rtems_test_assert( !status );
  status = sigaddset( &mask, SIGALRM );
  rtems_test_assert( !status );
  puts( "Init: Unblock SIGALRM" );
  status = sigprocmask( SIG_UNBLOCK, &mask, NULL );
  rtems_test_assert( !status );

  status = sleep( 1 );
  rtems_test_assert( status == 0 );
  rtems_test_assert( Signal_count == 0 );

  result = ualarm( 500000, 0 );
  rtems_test_assert( result == 0 );

  status = sleep( 1 );
  rtems_test_assert( status == 0 );
  rtems_test_assert( Signal_count == 1 );

  /* stop ularm */
  puts( "Init: clear ualarm with 0, 0" );
  result = ualarm( 0, 0 );
  rtems_test_assert( result == 0 );

  result = ualarm( 500000, 1000000 );
  rtems_test_assert( result == 0 );

  status = sleep( 1 );
  rtems_test_assert( status == 0 );
  rtems_test_assert( Signal_count == 2 );

  status = sleep( 1 );
  rtems_test_assert( status == 0 );
  rtems_test_assert( Signal_count == 3 );

  result = ualarm( 0, 0 );
  rtems_test_assert( result == 1000000 );

  TEST_END();
  rtems_test_exit(0);

  return NULL; /* just so the compiler thinks we returned something */
}
