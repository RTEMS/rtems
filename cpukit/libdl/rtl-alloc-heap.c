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

#include <rtems/score/apimutex.h>

void
rtems_rtl_alloc_heap (rtems_rtl_alloc_cmd cmd,
                      rtems_rtl_alloc_tag tag,
                      void**              address,
                      size_t              size)
{
  switch (cmd)
  {
    case RTEMS_RTL_ALLOC_NEW:
      *address = malloc (size);
      break;
    case RTEMS_RTL_ALLOC_DEL:
      free (*address);
      *address = NULL;
      break;
    case RTEMS_RTL_ALLOC_LOCK:
      _RTEMS_Lock_allocator();
      break;
    case RTEMS_RTL_ALLOC_UNLOCK:
      _RTEMS_Unlock_allocator();
      break;
    default:
      break;
  }
}
