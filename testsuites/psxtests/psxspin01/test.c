/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  This test exercises the POSIX Spinlock manager.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
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
