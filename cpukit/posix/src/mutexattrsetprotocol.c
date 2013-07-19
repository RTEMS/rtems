/**
 * @file
 *
 * @brief Mutex Initialization Scheduling Attributes
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
 *  13.6.1 Mutex Initialization Scheduling Attributes, P1003.1c/Draft 10, p. 128
 */

int pthread_mutexattr_setprotocol(
  pthread_mutexattr_t   *attr,
  int                    protocol
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  switch ( protocol ) {
    case PTHREAD_PRIO_NONE:
    case PTHREAD_PRIO_INHERIT:
    case PTHREAD_PRIO_PROTECT:
      attr->protocol = protocol;
      return 0;

    default:
      return EINVAL;
  }
}
