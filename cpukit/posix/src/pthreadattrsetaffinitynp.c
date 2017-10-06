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

int pthread_attr_setaffinity_np(
  pthread_attr_t    *attr,
  size_t             cpusetsize,
  const cpu_set_t   *cpuset
)
{
  if ( attr == NULL || !attr->is_initialized ) {
    return EINVAL;
  }

  if ( cpuset == NULL || cpusetsize != attr->affinitysetsize ) {
    return EINVAL;
  }

  CPU_COPY( cpuset, attr->affinityset );
  return 0;
}

#endif
