/*
 *  13.5.1 Thread Creation Scheduling Parameters, P1003.1c/Draft 10, p. 120
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

int pthread_attr_setschedparam(
  pthread_attr_t           *attr,
  const struct sched_param *param
)
{
  if ( !attr || !attr->is_initialized || !param )
    return EINVAL;

  attr->schedparam = *param;
  return 0;
}
