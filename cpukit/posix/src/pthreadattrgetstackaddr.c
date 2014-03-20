/**
 * @file
 *
 * @brief Thread Creation Attributes
 * @ingroup POSIXAPI
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

int pthread_attr_getstackaddr(
  const pthread_attr_t   *attr,
  void                  **stackaddr
)
{
  if ( !attr || !attr->is_initialized || !stackaddr )
    return EINVAL;

  *stackaddr = attr->stackaddr;
  return 0;
}
