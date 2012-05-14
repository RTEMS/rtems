/*
 *  _realloc_r Implementation
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(RTEMS_NEWLIB) && !defined(HAVE__REALLOC_R)
#include <sys/reent.h>
#include <stdlib.h>

void *_realloc_r(
  struct _reent *ignored __attribute__((unused)),
  void          *ptr,
  size_t         size
)
{
  return realloc( ptr, size );
}
#endif
