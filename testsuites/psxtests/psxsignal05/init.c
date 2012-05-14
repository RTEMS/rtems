/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define TEST_NAME                "05"
#define TEST_STRING              "User Signals"
#define SIGNAL_ONE               SIGUSR1
#define SIGNAL_TWO               SIGUSR2

#include <pmacros.h>
#include <signal.h>
#include <errno.h>
#include <errno.h>
#include <rtems/posix/psignal.h>

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void Signal_handler(int signo, siginfo_t *info, void *arg);
const char *signal_name(int signo);

void Signal_handler(
  int        signo,
  siginfo_t *info,
  void      *arg
)
{
  puts( "Signal handler hit" );
  rtems_test_exit(0);
}

const char *signal_name(int signo)
{
  if (signo == SIGUSR1)
    return "SIGUSR1";
  if (signo == SIGUSR2)
    return "SIGUSR2";
  if (signo == SIGRTMIN)
    return "SIGRTMIN";
  if (signo == SIGRTMAX)
    return "SIGRTMAX";
  return "unknown-signal";
}

void *POSIX_Init(
  void *argument
)
{
  struct sigaction    act;
  siginfo_t           info;
  bool                bc;

  puts( "\n\n*** POSIX TEST SIGNAL " TEST_NAME " ***" );

  act.sa_handler = NULL;
  act.sa_sigaction = Signal_handler;
  act.sa_flags   = SA_SIGINFO;
  sigaction( SIGNAL_ONE, &act, NULL );
  sigaction( SIGNAL_TWO, &act, NULL );

  printf(
   "Init - _POSIX_signals_Clear_signals when signals pending but\n"
   "Init -    not queued on SA_SIGINFO configured signal.\n"
  );

  /* cheat and put signal directly in */
  _POSIX_signals_Pending |= signo_to_mask( SIGUSR1 );

  bc = _POSIX_signals_Clear_signals(
    _Thread_Executing->API_Extensions[ THREAD_API_POSIX ],
    SIGNAL_ONE,
    &info,
    true,              /* is_global */
    false              /* check_blocked */
  );
  rtems_test_assert( bc );

  puts( "*** END OF POSIX TEST SIGNAL " TEST_NAME " ***" );
  rtems_test_exit(0);

  return NULL; /* just so the compiler thinks we returned something */
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS        1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
