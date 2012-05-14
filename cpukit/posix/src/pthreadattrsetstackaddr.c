/*
 *  16.1.1 Thread Creation Attributes, P1003.1c/Draft 10, p, 140
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

int pthread_attr_setstackaddr(
  pthread_attr_t  *attr,
  void            *stackaddr
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  attr->stackaddr = stackaddr;
  return 0;
}
