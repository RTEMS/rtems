/*
 *  16.1.1 Thread Creation Attributes
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_DECL_PTHREAD_ATTR_GETSTACK
#include <pthread.h>
#include <errno.h>

int pthread_attr_getstack(
  const pthread_attr_t   *attr,
  void                  **stackaddr,
  size_t                 *stacksize
)
{
  if ( !attr || !attr->is_initialized || !stackaddr || !stacksize )
    return EINVAL;

  *stackaddr = attr->stackaddr;
  *stacksize = attr->stacksize;
  return 0;
}
#endif
