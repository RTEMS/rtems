#if HAVE_CONFIG_H
#include "config.h"
#endif

/**
 * @file
 *
 * @brief Pthread Get/Set Concurrency
 * @ingroup POSIXAPI
 *
 * Per the Open Group specification, when user pthreads are mapped 1:1
 * onto kernel threads, the implementation simply tracks an internal
 * variable whose initial value is 0. If it is set, subsequent calls to
 * obtain the value return that previously set.
 */

/*
 *  COPYRIGHT (c) 2016.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <pthread.h>
#include <errno.h>

static int pthread_concurrency_level;

int pthread_getconcurrency(void)
{
  return pthread_concurrency_level;
}

int pthread_setconcurrency(
  int new_level
)
{
  pthread_concurrency_level = new_level;
  return 0;
}
