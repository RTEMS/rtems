/*
 *  malloc_get_statistics Implementation
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

#ifdef RTEMS_NEWLIB
#include "malloc_p.h"

int malloc_get_statistics(
  rtems_malloc_statistics_t *stats
)
{
  if ( !stats )
    return -1;
  _RTEMS_Lock_allocator();
  *stats = rtems_malloc_statistics;
  _RTEMS_Unlock_allocator();
  return 0;
}

#endif
