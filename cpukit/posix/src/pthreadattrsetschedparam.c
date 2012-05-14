/*
 *  13.5.1 Thread Creation Scheduling Parameters, P1003.1c/Draft 10, p. 120
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

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
