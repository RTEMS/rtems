/*
 *  RTEMS Malloc Family -- Dirty Memory from Malloc
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

#include <rtems.h>
#include <rtems/malloc.h>
#include "malloc_p.h"

#include <errno.h>

void rtems_malloc_dirty_memory(
  void   *start,
  size_t  size
)
{
  (void) memset(start, 0xCF, size);
}
