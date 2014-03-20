/**
 *  @file
 *
 *  @brief Get The Schedpolicy Attribute
 *  @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

/**
 *  13.5.1 Thread Creation Scheduling Parameters, P1003.1c/Draft 10, p. 120
 */
int pthread_attr_getschedpolicy(
  const pthread_attr_t  *attr,
  int                   *policy
)
{
  if ( !attr || !attr->is_initialized || !policy )
    return EINVAL;

  *policy = attr->schedpolicy;
  return 0;
}
