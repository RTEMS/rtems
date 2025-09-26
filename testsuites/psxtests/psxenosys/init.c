/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2010.
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

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <pthread.h>

#define CONFIGURE_INIT
#include "system.h"
#include "tmacros.h"

#include <aio.h>
#include <time.h>
#include <unistd.h>
#include <sched.h>

const char rtems_test_name[] = "PSXENOSYS";

void check_enosys(int status);

void check_enosys(int status)
{
  if ( (status == -1) && (errno == ENOSYS) )
    return;
  rtems_test_exit(0);
}

void *POSIX_Init(
  void *argument
)
{
  (void) argument;

  int             sc;

  TEST_BEGIN();

#ifndef RTEMS_POSIX_API
  sc = lio_listio( 0, NULL, 0, NULL );
  check_enosys( sc );
#endif

  sc = clock_getcpuclockid( 0, NULL );
  check_enosys( sc );

  sc = execl( NULL, NULL, (char*)0 );
  check_enosys( sc );

  sc = execle( NULL, NULL, (char*)0, NULL );
  check_enosys( sc );

  sc = execlp( NULL, NULL, (char*)0 );
  check_enosys( sc );

  sc = execv( NULL, NULL );
  check_enosys( sc );

  sc = execve( NULL, NULL, NULL );
  check_enosys( sc );

  sc = execvp( NULL, NULL );
  check_enosys( sc );

  sc = fork();
  check_enosys( sc );

  /*
   * The behavior of pthread_atfork() in single process environments was
   * undefined by POSIX but the fACE Technical Standard required returning
   * 0. Before ticket #4713, this did return ENOSYS. Just leaving this test
   * case here for convenience.
   */
  sc = pthread_atfork( NULL, NULL, NULL );
  rtems_test_assert( !sc );

  sc = pthread_getcpuclockid( 0, NULL );
  check_enosys( sc );

  sc = sched_setparam( 0, NULL );
  check_enosys( sc );

  sc = sched_getparam( 0, NULL );
  check_enosys( sc );

  sc = sched_setscheduler( 0, 0, NULL );
  check_enosys( sc );

  sc = sched_getscheduler( 0 );
  check_enosys( sc );

  sc = wait( NULL );
  check_enosys( sc );

  sc = waitpid( 0, NULL, 0 );
  check_enosys( sc );

  sc = mprotect( NULL, 0, 0 );
  posix_service_failed( sc, "mprotect" );

  sc = vfork();
  if ( sc != -1 ) {
    rtems_test_exit( 0 );
  }

  TEST_END();
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
