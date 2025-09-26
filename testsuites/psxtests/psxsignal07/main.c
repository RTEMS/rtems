/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * @brief psxsignal07 - Ensure initial signal mask and thread inheritance
 */

/*
 * COPYRIGHT (c) 2019.
 * On-Line Applications Research Corporation (OAR).
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

#include <tmacros.h>
#include "test_support.h"
#include <errno.h>
#include <pthread.h>
#include <signal.h>

const char rtems_test_name[] = "PSXSIGNAL 7";

static sigset_t main_sigmask;

#define SIGSET_NBYTES (sizeof(sigset_t))

/* #define DEBUG_TEST */

static void fetch_sigmask(const char *name, sigset_t *sigset_p)
{
  (void) name;

  int   rc;

  rc = sigemptyset(sigset_p);
  rtems_test_assert(rc == 0);

  rc = sigprocmask(SIG_SETMASK, NULL, sigset_p);
  rtems_test_assert(rc == 0);

#ifdef DEBUG_TEST
  /*
   * There is no assurance that sigset_t is a primitive type so
   * we have to print it a long at a time.
   */
  int            i;
  unsigned char *p = (unsigned char *) sigset_p;

  printf("%s signal mask (in hex):\n    ", name);
  for (i=0 ; i < SIGSET_NBYTES ; i++) {
    printf("%02x%s", *p++, (i % 16 == 15) ? "\n    " : " ");
  }
  printf("\n");
#endif
}

static void block_sigmask(int signo, sigset_t *sigset_p)
{
  sigset_t sigset;
  int      rc;

  /*
   * Block the requested signal
   */
  rc = sigemptyset(&sigset);
  rtems_test_assert(rc == 0);
  rc = sigaddset(&sigset, signo);
  rtems_test_assert(rc == 0);

  rc = sigprocmask(SIG_BLOCK, &sigset, NULL);
  rtems_test_assert(rc == 0);

  /*
   * Fetch the current signal mask reflecting the requested update
   */
  sigemptyset(sigset_p);
  rc = sigprocmask(SIG_SETMASK, NULL, sigset_p);
  rtems_test_assert(rc == 0);
}

static void *thread_body(void *arg)
{
  (void) arg;

  sigset_t    mask;

  /*
   * There is no assurance that sigset_t is a primitive type so
   * we have to use memcmp().
   */
  fetch_sigmask("main", &main_sigmask);
  fetch_sigmask(arg, &mask);
  rtems_test_assert(main_sigmask == mask);
  
  return NULL;
}

int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  int        rc;
  pthread_t  id;
  sigset_t   empty;

  TEST_BEGIN();

  /*
   * Verify first thread has empty signal mask
   */
  fetch_sigmask("main", &main_sigmask);

  sigemptyset(&empty);
  fetch_sigmask("empty", &empty);

  puts("Ensure main thread's mask equals sigemptyset");
  rtems_test_assert(main_sigmask == empty);

  /*
   * Verify first thread has empty signal mask
   */
  puts("Ensure parent's empty mask is inherited by thread");
  rc = pthread_create(&id, NULL, thread_body, "thread1");
  rtems_test_assert(rc == 0);
  sleep(1);

  /*
   * Create a thread and see if it inherits non-empty signal mask.
   */
  puts("Ensure parent's mask with SIGUSR1 mask is inherited by thread");
  block_sigmask(SIGUSR1, &main_sigmask);
  rc = pthread_create(&id, NULL, thread_body, "thread2");
  rtems_test_assert(rc == 0);
  sleep(1);
  
  TEST_END();

  rtems_test_exit(0);
  return 0;
}

