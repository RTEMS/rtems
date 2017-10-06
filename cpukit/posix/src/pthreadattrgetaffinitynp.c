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

int pthread_attr_getaffinity_np(
  const pthread_attr_t *attr,
  size_t                cpusetsize,
  cpu_set_t            *cpuset
)
{
  if ( attr == NULL || !attr->is_initialized ) {
    return EINVAL;
  }

  if ( cpuset == NULL || cpusetsize != attr->affinitysetsize ) {
    return EINVAL;
  }

  CPU_COPY( attr->affinityset, cpuset );
  return 0;
}
#endif
