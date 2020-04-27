/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Get protocol Attribute of Mutex Attribute Obect
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

#include <errno.h>
#include <pthread.h>
#include <stddef.h>

/*
 *  13.6.1 Mutex Initialization Scheduling Attributes, P1003.1c/Draft 10, p. 128
 */

int pthread_mutexattr_getprotocol(
  const pthread_mutexattr_t   *attr,
  int                         *protocol
)
{
  if ( attr == NULL || !attr->is_initialized || protocol == NULL )
    return EINVAL;

  *protocol = attr->protocol;
  return 0;
}
