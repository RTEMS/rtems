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

#if HAVE_DECL_PTHREAD_ATTR_GETGUARDSIZE
#include <pthread.h>
#include <errno.h>

int pthread_attr_getguardsize(
  const pthread_attr_t  *attr,
  size_t                *guardsize
)
{
  if ( !attr || !attr->is_initialized || !guardsize )
    return EINVAL;

  *guardsize = attr->guardsize;
  return 0;
}
#endif
