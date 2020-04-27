/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Set the Process-Shared Condition Variable Attributes
 */

/*
 *  COPYRIGHT (c) 1989-2007, 2016.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

int pthread_condattr_setpshared(
  pthread_condattr_t *attr,
  int                 pshared
)
{
  if ( !attr )
    return EINVAL;

  switch ( pshared ) {
    case PTHREAD_PROCESS_SHARED:
    case PTHREAD_PROCESS_PRIVATE:
      attr->process_shared = pshared;
      return 0;

    default:
      return EINVAL;
  }
}
