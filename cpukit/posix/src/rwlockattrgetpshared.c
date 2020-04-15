/**
 *  @file
 *
 *  @brief Get the Process-Shared Attribute of the RWLock
 *  @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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

int pthread_rwlockattr_getpshared(
  const pthread_rwlockattr_t *attr,
  int                         *pshared
)
{
  if ( !attr )
    return EINVAL;

  if ( !attr->is_initialized )
    return EINVAL;

  *pshared = attr->process_shared;
  return 0;
}
