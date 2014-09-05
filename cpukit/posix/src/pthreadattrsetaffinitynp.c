#if HAVE_CONFIG_H
#include "config.h"
#endif

/**
 * @file
 *
 * @brief Pthread Attribute Set Affinity
 * @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_DECL_PTHREAD_ATTR_SETAFFINITY_NP

#define  _GNU_SOURCE
#include <pthread.h>
#include <errno.h>

#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/priorityimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/cpusetimpl.h>

int pthread_attr_setaffinity_np(
  pthread_attr_t    *attr,
  size_t             cpusetsize,
  const cpu_set_t   *cpuset
)
{
  if ( !cpuset )
    return EFAULT;
  if ( !attr )
    return EFAULT;

  if (! _CPU_set_Is_valid( cpuset, cpusetsize ) )
    return EINVAL;

  CPU_COPY( attr->affinityset, cpuset );

  return 0;
}

#endif
