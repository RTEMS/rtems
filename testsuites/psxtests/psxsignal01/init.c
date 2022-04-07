/* SPDX-License-Identifier: BSD-2-Clause */

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

#define CONFIGURE_INIT
#include "system.h"
#include <signal.h>
#include <errno.h>
#include <reent.h>

const char rtems_test_name[] = "PSXSIGNAL 1";

/* forward declarations to avoid warnings */
int test_main(void);
void Handler_1(int signo);
void Signal_handler(int signo);
void Signal_info_handler(int signo, siginfo_t *info, void *context);
rtems_timer_service_routine Signal_duringISR_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
);

typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);
extern void _POSIX_signals_Abnormal_termination_handler( int signo );

volatile int Signal_occurred;
volatile int Signal_count;

static void block_all_signals(void)
{
  int               sc;
  sigset_t          mask;

  sc = sigfillset( &mask );
  rtems_test_assert( !sc );

  sc = pthread_sigmask( SIG_BLOCK, &mask, NULL );
  rtems_test_assert( !sc );
}

void Handler_1(
  int signo
)
{
  Signal_count++;
  printf(
    "Handler_1: Signal: %d caught by 0x%" PRIxpthread_t " (%d)\n",
    signo,
    pthread_self(),
    Signal_count
  );
  Signal_occurred = 1;
}

void Signal_handler(
  int signo
)
{
  Signal_count++;
  printf(
    "Signal: %d caught by 0x%"PRIxpthread_t " (%d)\n",
    signo,
    pthread_self(),
    Signal_count
  );
  Signal_occurred = 1;
}

void Signal_info_handler(
  int        signo,
  siginfo_t *info,
  void      *context
)
{
  Signal_count++;
  printf(
    "Signal_info: %d caught by 0x%" PRIxpthread_t " (%d) si_signo= %d si_code= %d value= %d\n",
    signo,
    pthread_self(),
    Signal_count,
    info->si_signo,
    info->si_code,
    info->si_value.sival_int
  );
  Signal_occurred = 1;
}

rtems_timer_service_routine Signal_duringISR_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  int  status;

  status = pthread_kill( pthread_self(), SIGUSR1 );
  rtems_test_assert( status == 0 );
}


void *POSIX_Init(
  void *argument
)
{
  int               status;
  struct sigaction  act;
  sigset_t          mask;
  sighandler_t      oldHandler;
  sighandler_t      newHandler;
  rtems_interval start, end;

  TEST_BEGIN();

  block_all_signals();

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08" PRIxpthread_t "\n", Init_id );

  Signal_occurred = 0;
  Signal_count = 0;
  act.sa_handler = Handler_1;
  act.sa_flags   = 0;
  sigaction( SIGUSR1, &act, NULL );
  sigaction( SIGFPE, &act, NULL );
  sigaction( SIGILL, &act, NULL );
  sigaction( SIGSEGV, &act, NULL );


  /*
   * If we have the signal pending with default, we will die.
   */
  puts("Validate signal with SIG_DFL");
  signal( SIGUSR1, SIG_DFL );
  status = kill( getpid(), SIGUSR1 );
  status = sleep( 1 );

  puts("Validate signal with SIG_IGN");
  signal( SIGUSR1, SIG_IGN );
  status = kill( getpid(), SIGUSR1 );
  status = sleep( 1 );

/* unblock Signal and see if it happened */
  status = sigemptyset( &mask );
  rtems_test_assert( !status );

  status = sigaddset( &mask, SIGUSR1 );
  rtems_test_assert( !status );

  status = sigaddset( &mask, SIGFPE );
  rtems_test_assert( !status );

  status = sigaddset( &mask, SIGILL );
  rtems_test_assert( !status );

  status = sigaddset( &mask, SIGSEGV );
  rtems_test_assert( !status );

  puts( "Init: Unblock SIGUSR1 SIGFPE SIGILL SIGSEGV" );
  status = sigprocmask( SIG_UNBLOCK, &mask, NULL );
  rtems_test_assert( !status );

/* install a signal handler for SIGUSR1 */
  Signal_occurred = 0;
  Signal_count = 0;
  act.sa_handler = Handler_1;
  act.sa_flags   = 0;
  sigaction( SIGUSR1, &act, NULL );

  Signal_count = 0;
  Signal_occurred = 0;

  newHandler = Signal_handler;
  oldHandler = signal( SIGUSR1, newHandler );
  if (oldHandler == Handler_1 )
    puts("Init: signal return value verified");
  else
    puts("Init: ERROR==> signal unexpected return value" );
  status = sleep( 1 );

  puts( "Init: send SIGUSR1 to process" );
  status = kill( getpid(), SIGUSR1 );
  status = sleep( 5 );

  puts( "Init: send SIGFPE to process" );
  status = _kill_r( NULL, getpid(), SIGFPE );
  status = sleep(5);

  puts( "Init: send SIGILL to process" );
  status = _kill_r( NULL, getpid(), SIGILL );
  status = sleep(5);

  puts( "Init: send SIGSEGV to process" );
  status = _kill_r( NULL, getpid(), SIGSEGV );
  status = sleep(5);

  Timer_name[0]= rtems_build_name( 'T', 'M', '1', ' ' );
  status = rtems_timer_create( Timer_name[0], &Timer_id[0]);

  Signal_count = 0;
  Signal_occurred = 0;
  puts( "Init: send SIGUSR1 to process from a TSR (interruptible sleep)" );
  status = rtems_timer_fire_after(
    Timer_id[ 0 ],
    1,
    Signal_duringISR_TSR,
    NULL
  );
  sleep(5);
  /* signal occurs during interruptible sleep */

  /* now schedule another one to fire but do not sleep */

  puts( "Init: send SIGUSR1 to process from a TSR (spin)" );
  start = rtems_clock_get_ticks_since_boot();
  Signal_count = 0;
  Signal_occurred = 0;
  status = rtems_timer_fire_after(
    Timer_id[ 0 ],
    10,
    Signal_duringISR_TSR,
    NULL
  );
  do {
    end = rtems_clock_get_ticks_since_boot();
  } while ( !Signal_occurred && ((end - start) <= 800));

  if ( !Signal_occurred ) {
    puts( "Signal did not occur" );
    rtems_test_exit(0);
  }

/* end of install a signal handler for SIGUSR1 */

  Signal_occurred = 0;

  puts("*** Validate unexpected program termination ***");
  TEST_END();
  _POSIX_signals_Abnormal_termination_handler( SIGUSR1 );
  status = sleep( 1 );

  puts( "ERROR==> Expected program termination");
  rtems_test_exit(0);
  return NULL; /* just so the compiler thinks we returned something */
}
