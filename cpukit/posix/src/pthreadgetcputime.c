/*
 *  20.1.7 CPU-time Clock Thread Creation Attribute, P1003.4b/Draft 8, p. 59
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <pthread.h>
#include <errno.h>

int pthread_attr_getcputime(
  pthread_attr_t  *attr,
  int             *clock_allowed
)
{
  if ( !attr || !attr->is_initialized || !clock_allowed )
    return EINVAL;

  *clock_allowed = attr->cputime_clock_allowed;
  return 0;
}

