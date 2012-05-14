/*
 *  _calloc_r Implementation
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

#include <sys/reent.h>
#include <stdlib.h>


static void rtems_malloc_statistics_initialize( void )
{
  /*
   * Zero all the statistics
   */
  (void) memset(&rtems_malloc_statistics, 0, sizeof(rtems_malloc_statistics));
}

static void rtems_malloc_statistics_at_malloc(
  void *pointer
)
{
  uintptr_t actual_size = 0;
  uint32_t current_depth;
  rtems_malloc_statistics_t *s = &rtems_malloc_statistics;

  if ( !pointer )
    return;

  _Protected_heap_Get_block_size(RTEMS_Malloc_Heap, pointer, &actual_size);

  MSBUMP(lifetime_allocated, actual_size);

  current_depth = (uint32_t) (s->lifetime_allocated - s->lifetime_freed);
  if (current_depth > s->max_depth)
      s->max_depth = current_depth;
}

/*
 *  If the pointer is not in the heap, then we won't be able to get its
 *  size and thus we skip updating the statistics.
 */
static void rtems_malloc_statistics_at_free(
  void *pointer
)
{
  uintptr_t size;

  if (_Protected_heap_Get_block_size(RTEMS_Malloc_Heap, pointer, &size) ) {
    MSBUMP(lifetime_freed, size);
  }
}

rtems_malloc_statistics_functions_t rtems_malloc_statistics_helpers_table = {
  rtems_malloc_statistics_initialize,
  rtems_malloc_statistics_at_malloc,
  rtems_malloc_statistics_at_free,
};

#endif
