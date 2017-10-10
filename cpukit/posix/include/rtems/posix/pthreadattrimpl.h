/**
 * @file
 *
 * @brief POSIX Threads Private Support
 *
 * This include file contains all the private support information for
 * POSIX threads.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_PTHREADATTRIMPL_H
#define _RTEMS_POSIX_PTHREADATTRIMPL_H

#include <errno.h>
#include <pthread.h>

#include <rtems/score/basedefs.h>
#include <rtems/score/assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup POSIX_PTHREAD
 */
/**@{**/

/**
 * This variable contains the default POSIX Thread attributes.
 */
extern const pthread_attr_t _POSIX_Threads_Default_attributes;

RTEMS_INLINE_ROUTINE void _POSIX_Threads_Copy_attributes(
  pthread_attr_t        *dst_attr,
  const pthread_attr_t  *src_attr
)
{
  *dst_attr = *src_attr;
  _Assert(
    dst_attr->affinitysetsize == sizeof(dst_attr->affinitysetpreallocated)
  );
  dst_attr->affinityset = &dst_attr->affinitysetpreallocated;
}

RTEMS_INLINE_ROUTINE void _POSIX_Threads_Initialize_attributes(
  pthread_attr_t  *attr
)
{
  _POSIX_Threads_Copy_attributes(
    attr,
    &_POSIX_Threads_Default_attributes
  );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
