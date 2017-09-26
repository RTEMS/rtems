/**
 * @file
 *
 * @brief Function Destroys a read-write lock Attributes object 
 * @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <pthread.h>
#include <stddef.h>
#include <stdbool.h>

/*
 *  RWLock Initialization Attributes
 */

int pthread_rwlockattr_destroy(
  pthread_rwlockattr_t *attr
)
{
  if ( attr == NULL || !attr->is_initialized )
    return EINVAL;

  attr->is_initialized = false;
  return 0;
}
