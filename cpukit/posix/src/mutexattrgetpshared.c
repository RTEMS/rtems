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
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <pthread.h>

#include <rtems/system.h>
#include <rtems/score/coremuteximpl.h>
#include <rtems/score/watchdog.h>
#include <rtems/posix/muteximpl.h>
#include <rtems/posix/priorityimpl.h>
#include <rtems/posix/time.h>

/*
 *  11.3.1 Mutex Initialization Attributes, P1003.1c/Draft 10, p. 81
 */

int pthread_mutexattr_getpshared(
  const pthread_mutexattr_t *attr,
  int                       *pshared
)
{
  if ( !attr || !attr->is_initialized || !pshared )
    return EINVAL;

  *pshared = attr->process_shared;
  return 0;
}
