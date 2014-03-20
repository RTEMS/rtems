/**
 * @file
 *
 * @brief Thread Creation Attributes
 * @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

int pthread_attr_setdetachstate(
  pthread_attr_t  *attr,
  int              detachstate
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  switch ( detachstate ) {
    case PTHREAD_CREATE_DETACHED:
    case PTHREAD_CREATE_JOINABLE:
      attr->detachstate = detachstate;
      return 0;

    default:
      return EINVAL;
  }
}
