/**
 * @file
 *
 * @brief Obtaining process-shared Attribute Value from the Attributes Object
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

/*
 *  11.3.1 Mutex Initialization Attributes, P1003.1c/Draft 10, p. 81
 */

int pthread_mutexattr_getpshared(
  const pthread_mutexattr_t *attr,
  int                       *pshared
)
{
  if ( attr == NULL || !attr->is_initialized || pshared == NULL )
    return EINVAL;

  *pshared = attr->process_shared;
  return 0;
}
