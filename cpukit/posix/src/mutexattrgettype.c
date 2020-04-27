/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Function gets the Mutex Type Attribute
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <pthread.h>
#include <stddef.h>

#if defined(_UNIX98_THREAD_MUTEX_ATTRIBUTES)
int pthread_mutexattr_gettype(
  const pthread_mutexattr_t *attr,
  int                       *type
)
{
  if ( attr == NULL || !attr->is_initialized || type == NULL )
    return EINVAL;

  *type = attr->type;
  return 0;
}
#endif

