/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Set the Clock Condition Variable Attributes
 */

/*
 *  COPYRIGHT (c) 2016.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

int pthread_condattr_setclock(
  pthread_condattr_t *attr,
  clockid_t           clock
)
{
  if ( attr == NULL ) {
    return EINVAL;
  }

  switch ( clock ) {
    case CLOCK_REALTIME:
    case CLOCK_MONOTONIC:
      attr->clock = clock;
      return 0;

    case CLOCK_PROCESS_CPUTIME_ID:
    case CLOCK_THREAD_CPUTIME_ID:
    default:
      break;
  }
  return EINVAL;
}
