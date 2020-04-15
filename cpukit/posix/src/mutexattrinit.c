/**
 *  @file
 *
 *  @brief Mutex Initialization Attribute
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

#include <errno.h>
#include <pthread.h>
#include <stddef.h>

#include <rtems/posix/muteximpl.h>

/**
 *  11.3.1 Mutex Initialization Attributes, P1003.1c/Draft 10, p. 81
 */
int pthread_mutexattr_init(
  pthread_mutexattr_t *attr
)
{
  if ( attr == NULL )
    return EINVAL;

  *attr = _POSIX_Mutex_Default_attributes;
  return 0;
}
