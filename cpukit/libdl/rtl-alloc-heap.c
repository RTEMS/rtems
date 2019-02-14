/*
 *  COPYRIGHT (c) 2012 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems_rtl
 *
 * @brief RTEMS Run-Time Linker Allocator for the standard heap.
 */

#include <stdlib.h>

#include "rtl-alloc-heap.h"

void
rtems_rtl_alloc_heap (rtems_rtl_alloc_cmd cmd,
                      rtems_rtl_alloc_tag tag,
                      void**              address,
                      size_t              size)
{
  if (cmd == RTEMS_RTL_ALLOC_NEW)
    *address = malloc (size);
  else if (cmd == RTEMS_RTL_ALLOC_DEL)
  {
    free (*address);
    *address = NULL;
  }
}
