/*
 *  COPYRIGHT (c) 2017.
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
#include <signal.h>
#include <string.h>
#include <stdio.h>

const char rtems_test_name[] = "PSXSTRSIGNAL 1";
typedef struct {
  char *sigstr;
  int   signal;
} Signals_t;

Signals_t Signals[] = {
  { "SIGHUP",   SIGHUP },
  { "SIGINT",   SIGINT },
  { "SIGQUIT",  SIGQUIT },
  { "SIGILL",   SIGILL },
  { "SIGTRAP",  SIGTRAP },
  { "SIGIOT",   SIGIOT },
  { "SIGABRT",  SIGABRT },
#ifdef SIGEMT
  { "SIGEMT",   SIGEMT },
#endif
  { "SIGFPE",   SIGFPE },
  { "SIGKILL",  SIGKILL },
  { "SIGBUS",   SIGBUS },
  { "SIGSEGV",  SIGSEGV },
  { "SIGSYS",   SIGSYS },
  { "SIGPIPE",  SIGPIPE },
  { "SIGALRM",  SIGALRM },
  { "SIGTERM",  SIGTERM },
  { "SIGURG",   SIGURG },
  { "SIGSTOP",  SIGSTOP },
  { "SIGTSTP",  SIGTSTP },
  { "SIGCONT",  SIGCONT },
  { "SIGCHLD",  SIGCHLD },
  { "SIGCLD",   SIGCLD },
  { "SIGTTIN",  SIGTTIN },
  { "SIGTTOU",  SIGTTOU },
  { "SIGIO",    SIGIO },
  { "SIGPOLL",  SIGPOLL },
  { "SIGWINCH", SIGWINCH },
  { "SIGUSR1",  SIGUSR1 },
  { "SIGUSR2",  SIGUSR2 },
  { NULL,       -1 },
};

void do_test(void);
void do_test_rt(void);
void *POSIX_Init(void *unused);

void do_test(void)
{
  int i;

  puts( "=== Normal Signals" );
  for (i=0 ; Signals[i].sigstr ; i++) {
    printf(
      "signal=%s => %s\n",
      Signals[i].sigstr,
      strsignal(Signals[i].signal)
    );
  }
}

void do_test_rt(void)
{
  int sig;

  puts( "=== Real-Time Signals" );
  for (sig=SIGRTMIN ; sig <= SIGRTMAX ; sig++) {
    printf(
      "signal=SIGRTMIN+%d => %s\n",
      sig-SIGRTMIN,
      strsignal(sig)
    );
  }
}

void *POSIX_Init(
  void *unused
)
{
  TEST_BEGIN();

  do_test();
  puts("");
  do_test_rt();

  TEST_END();
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}

/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS 1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
