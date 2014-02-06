/**
 * @file
 *
 * @brief Pthread Attribute Get Affinity
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_DECL_PTHREAD_ATTR_GETAFFINITY_NP

#define  _GNU_SOURCE
#include <pthread.h>
#include <errno.h>

#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/priorityimpl.h>
#include <rtems/score/threadimpl.h>

int pthread_attr_getaffinity_np(
  const pthread_attr_t *attr,
  size_t                cpusetsize,
  cpu_set_t            *cpuset
)
{
  if ( !cpuset )
    return EFAULT;
  if ( !attr )
    return EFAULT;

  if ( cpusetsize != attr->affinitysetsize)
    return EINVAL;

  CPU_COPY( cpuset, attr->affinityset );
  return 0;
}
#endif
