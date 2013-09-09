/**
 * @file
 *
 * @brief Sets Scheduling policy Attributes of Thread Attributes Object
 * @ingroup POSIXAPI
 */

/*
 *  13.5.1 Thread Creation Scheduling Attributes, P1003.1c/Draft 10, p. 120
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/posix/pthreadimpl.h>

int pthread_attr_setschedpolicy(
  pthread_attr_t  *attr,
  int              policy
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  switch ( policy ) {
    case SCHED_OTHER:
    case SCHED_FIFO:
    case SCHED_RR:
    case SCHED_SPORADIC:
      attr->schedpolicy = policy;
      return 0;

    default:
      return ENOTSUP;
  }
}
