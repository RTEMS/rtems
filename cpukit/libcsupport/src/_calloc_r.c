/**
 *  @file
 *
 *  @brief Allocate Space for Array in Memory
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(RTEMS_NEWLIB) && !defined(HAVE__CALLOC_R)
#include <sys/reent.h>
#include <stdlib.h>

void *_calloc_r(
  struct _reent *ignored,
  size_t         elements,
  size_t         size
)
{
  (void) ignored;
  return calloc( elements, size );
}
#endif
