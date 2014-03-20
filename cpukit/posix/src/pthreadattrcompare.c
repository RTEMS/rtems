/**
 * @file
 *
 * @brief RTEMS specific pthread attribute comparison
 * @ingroup POSIX_PTHREADS Private Threads
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>

#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <rtems/posix/pthreadimpl.h>

int rtems_pthread_attribute_compare(
  const pthread_attr_t *attr1,
  const pthread_attr_t *attr2
)
{
  if ( attr1->is_initialized  !=  attr2->is_initialized )
    return 1;

  if ( attr1->stackaddr != attr2->stackaddr )
    return 1;

  if ( attr1->stacksize != attr2->stacksize )
    return 1;

  if ( attr1->contentionscope != attr2->contentionscope )
    return 1;

  if ( attr1->inheritsched != attr2->inheritsched )
    return 1;

  if ( attr1->schedpolicy != attr2->schedpolicy )
    return 1;

  if (memcmp(
    &attr1->schedparam,
    &attr2->schedparam,
    sizeof(struct sched_param)
  ))
    return 1;

  #if HAVE_DECL_PTHREAD_ATTR_SETGUARDSIZE
    if ( attr1->guardsize != attr2->guardsize )
      return 1;
  #endif

  #if defined(_POSIX_THREAD_CPUTIME)
    if ( attr1->cputime_clock_allowed != attr2->cputime_clock_allowed )
      return 1;
  #endif

  if ( attr1->detachstate != attr2->detachstate )
    return 1;

  #if defined(__RTEMS_HAVE_SYS_CPUSET_H__)
    if ( attr1->affinitysetsize != attr2->affinitysetsize )
      return 1;

    if (!CPU_EQUAL_S(
      attr1->affinitysetsize,
      attr1->affinityset,
      attr2->affinityset
    ))
      return 1;

    if (!CPU_EQUAL_S(
      attr1->affinitysetsize,
      &attr1->affinitysetpreallocated,
      &attr2->affinitysetpreallocated
    ))
      return 1;
  #endif

  return 0;
}


