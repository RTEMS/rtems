/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2017.
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
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS 1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
