/**
 *  @file
 *
 *  This test exercises the POSIX Spinlock manager.
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

#include "tmacros.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include <pthread.h>

#include <rtems.h>  /* for task creation */

const char rtems_test_name[] = "PSXSPIN 1";

/* forward declarations to avoid warnings */
int test_main(void);

/*
 *  main entry point to the test
 */

#if defined(__rtems__)
int test_main(void)
#else
int main(
  int    argc,
  char **argv
)
#endif
{
  pthread_spinlock_t    spinlock;
  pthread_spinlock_t    spinlock2;
  int                   status;

  TEST_BEGIN();

  puts( "pthread_spin_init( &spinlock, PTHREAD_PROCESS_PRIVATE ) -- OK" );
  status = pthread_spin_init( &spinlock, PTHREAD_PROCESS_PRIVATE );
  rtems_test_assert( status == 0 );

  puts( "pthread_spin_destroy( &spinlock ) -- OK" );
  status = pthread_spin_destroy( &spinlock );
  rtems_test_assert( status == 0 );

  puts( "pthread_spin_init( &spinlock, PTHREAD_PROCESS_SHARED ) -- OK" );
  status = pthread_spin_init( &spinlock, PTHREAD_PROCESS_PRIVATE );
  rtems_test_assert( status == 0 );

  puts( "pthread_spin_destroy( &spinlock ) -- OK" );
  status = pthread_spin_destroy( &spinlock );
  rtems_test_assert( status == 0 );

  puts( "pthread_spin_init( &spinlock, 0x1234 ) -- OK" );
  status = pthread_spin_init( &spinlock, 0x1234 );
  rtems_test_assert( status == 0 );

  puts( "pthread_spin_init( &spinlock2, 0 ) -- OK" );
  status = pthread_spin_init( &spinlock2, 0 );
  rtems_test_assert( status == 0 );

  rtems_test_assert( _ISR_Get_level() == 0 );

  puts( "pthread_spin_lock( &spinlock ) -- OK" );
  status = pthread_spin_lock( &spinlock );
  rtems_test_assert( status == 0 );

  rtems_test_assert( _ISR_Get_level() != 0 );

  puts( "pthread_spin_lock( &spinlock2 ) -- OK" );
  status = pthread_spin_lock( &spinlock2 );
  rtems_test_assert( status == 0 );

  rtems_test_assert( _ISR_Get_level() != 0 );

  puts( "pthread_spin_unlock( &spinlock2 ) -- OK" );
  status = pthread_spin_unlock( &spinlock2 );
  rtems_test_assert( status == 0 );

  rtems_test_assert( _ISR_Get_level() != 0 );

  puts( "pthread_spin_unlock( &spinlock ) -- OK" );
  status = pthread_spin_unlock( &spinlock );
  rtems_test_assert( status == 0 );

  rtems_test_assert( _ISR_Get_level() == 0 );

  puts( "pthread_spin_trylock( &spinlock ) -- OK" );
  status = pthread_spin_trylock( &spinlock );
  rtems_test_assert( status == 0 );

  rtems_test_assert( _ISR_Get_level() != 0 );

  puts( "pthread_spin_trylock( &spinlock2 ) -- OK" );
  status = pthread_spin_trylock( &spinlock2 );
  rtems_test_assert( status == 0 );

  rtems_test_assert( _ISR_Get_level() != 0 );

  puts( "pthread_spin_unlock( &spinlock2 ) -- OK" );
  status = pthread_spin_unlock( &spinlock2 );
  rtems_test_assert( status == 0 );

  rtems_test_assert( _ISR_Get_level() != 0 );

  puts( "pthread_spin_unlock( &spinlock ) -- OK" );
  status = pthread_spin_unlock( &spinlock );
  rtems_test_assert( status == 0 );

  rtems_test_assert( _ISR_Get_level() == 0 );

  puts( "pthread_spin_destroy( &spinlock2 ) -- OK" );
  status = pthread_spin_destroy( &spinlock2 );
  rtems_test_assert( status == 0 );

  puts( "pthread_spin_destroy( &spinlock ) -- OK" );
  status = pthread_spin_destroy( &spinlock );
  rtems_test_assert( status == 0 );

  TEST_END();
  exit(0);
}
