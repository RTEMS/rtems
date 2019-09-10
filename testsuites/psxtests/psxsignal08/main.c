/*
 * @brief psxsignal08 - Ensure initial signal mask and Classic Task inheritance
 */

/*
 * COPYRIGHT (c) 2019.
 * On-Line Applications Research Corporation (OAR).
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include "test_support.h"
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include <rtems.h>

const char rtems_test_name[] = "PSXSIGNAL 8";

static sigset_t main_sigmask;

#define SIGSET_NBYTES (sizeof(sigset_t))

/* #define DEBUG_TEST */

static void fetch_sigmask(const char *name, sigset_t *sigset_p)
{
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

static rtems_task task_body(rtems_task_argument arg)
{
  sigset_t    mask;

  /*
   * There is no assurance that sigset_t is a primitive type so
   * we have to use memcmp().
   */
  fetch_sigmask("main", &main_sigmask);
  fetch_sigmask((void *)arg, &mask);
  rtems_test_assert(main_sigmask == mask);
  
  (void) rtems_task_delete(RTEMS_SELF);
}

int main(int argc, char **argv)
{
  sigset_t          empty;
  rtems_status_code status;
  rtems_id          id;

  TEST_BEGIN();

  /*
   * Verify first task has empty signal mask
   */
  fetch_sigmask("main", &main_sigmask);

  sigemptyset(&empty);
  fetch_sigmask("empty", &empty);

  puts("Ensure main task's mask equals sigemptyset");
  rtems_test_assert(main_sigmask == empty);

  /*
   * Create a task and see if it inherits empty signal mask.
   */
  puts("Ensure parent's empty mask is inherited by thread");
  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '1', ' ' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create" );

  status = rtems_task_start( id, task_body, 1 );
  directive_failed( status, "rtems_task_start" );

  sleep(1);

  /*
   * Create a task and see if it inherits non-empty signal mask.
   */
  puts("Ensure parent's mask with SIGUSR1 mask is inherited by thread");
  block_sigmask(SIGUSR1, &main_sigmask);

  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '1', ' ' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create" );

  status = rtems_task_start( id, task_body, 1 );
  directive_failed( status, "rtems_task_start" );

  sleep(1);

  
  TEST_END();

  rtems_test_exit(0);
  return 0;
}

