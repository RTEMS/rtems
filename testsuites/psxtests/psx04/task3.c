/* SPDX-License-Identifier: BSD-2-Clause */

/*  Task_3
 *
 *  This routine serves as a test task.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2011.
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

#include "system.h"
#include <signal.h>

void *Task_3(
  void *argument
)
{
  unsigned int  remaining;
  int           status;
  int           sig;
  volatile union sigval  value;
  sigset_t      mask;
  siginfo_t     info;

  value.sival_int = SIGUSR1;

  printf( "Task_3: sigqueue SIGUSR1 with value= %d\n", value.sival_int );
  status = sigqueue( getpid(), SIGUSR1, value );
  rtems_test_assert( !status );

     /* catch signal with sigwaitinfo */

  empty_line();

  status = sigemptyset( &mask );
  rtems_test_assert( !status );

  status = sigaddset( &mask, SIGUSR1 );
  rtems_test_assert( !status );

  printf( "Task_3: sigwaitinfo SIGUSR1 with value= %d\n", value.sival_int );
  status = sigwaitinfo( &mask, &info );

     /* switch to Init */

  rtems_test_assert( !(status==-1) );
  printf(
    "Task_3: si_signo= %d si_code= %d value= %d\n",
    info.si_signo,
    info.si_code,
    info.si_value.sival_int
  );

     /* catch signal with sigwait */

  empty_line();

  status = sigemptyset( &mask );
  rtems_test_assert( !status );

  status = sigaddset( &mask, SIGUSR1 );
  rtems_test_assert( !status );

  printf( "Task_3: sigwait SIGUSR1\n" );
  status = sigwait( &mask, &sig );

     /* switch to Init */

  rtems_test_assert( !status );
  printf( "Task_3: signo= %d\n", sig );

     /* catch signal with pause */

  empty_line();

  status = sigemptyset( &mask );
  rtems_test_assert( !status );

  status = sigaddset( &mask, SIGUSR1 );
  rtems_test_assert( !status );

  printf( "Task_3: pause\n" );
  status = pause( );

     /* switch to Init */

  rtems_test_assert( !(status==-1) );
  printf( "Task_3: pause= %d\n", status );


     /* send signal to Init task before it has pended for a signal */

  empty_line();

  printf( "Task_3: sending SIGUSR2\n" );
  status = pthread_kill( Init_id, SIGUSR2 );
  rtems_test_assert( !status );

  printf( "Task_3: sleep so the Init task can reguest a signal\n" );
  remaining = sleep( 1 );
  rtems_test_assert( !status );
  rtems_test_assert( remaining == 0 );

     /* end of task 3 */
  printf( "Task_3: exit\n" );
  pthread_exit( NULL );

     /* switch to Init */

  return NULL; /* just so the compiler thinks we returned something */
}
