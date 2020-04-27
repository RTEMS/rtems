/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Get the Clock Condition Variable Attributes
 */

/*
 *  COPYRIGHT (c) 2016
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

int pthread_condattr_getclock(
  const pthread_condattr_t *__restrict attr,
  clockid_t                *__restrict clock
)
{
  if ( attr == NULL ) {
    return EINVAL;
  }

  if ( clock == NULL ) {
    return EINVAL;
  }

  *clock = attr->clock;
  return 0;
}
