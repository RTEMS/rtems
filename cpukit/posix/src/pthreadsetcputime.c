/*
 *  20.1.7 CPU-time Clock Thread Creation Attribute, P1003.4b/Draft 8, p. 59
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

int pthread_attr_setcputime(
  pthread_attr_t  *attr,
  int              clock_allowed
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  switch ( clock_allowed ) {
    case CLOCK_ENABLED:
    case CLOCK_DISABLED:
      attr->cputime_clock_allowed = clock_allowed;
      return 0;

    default:
      return EINVAL;
  }
}

