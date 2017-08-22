/*
 *  Print Heap Information Structure
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>
#include <stdio.h>

#include "internal.h"

void rtems_shell_print_heap_info(
  const char             *c,
  const Heap_Information *h
)
{
  printf(
    "Number of %s blocks:                    %12" PRIuPTR "\n"
    "Largest %s block:                       %12" PRIuPTR "\n"
    "Total bytes %s:                         %12" PRIuPTR "\n",
    c, h->number,
    c, h->largest,
    c, h->total
  );
}

void rtems_shell_print_heap_stats(
  const Heap_Statistics *s
)
{
  printf(
    "Size of the allocatable area in bytes:    %12" PRIuPTR "\n"
    "Minimum free size ever in bytes:          %12" PRIuPTR "\n"
    "Maximum number of free blocks ever:       %12" PRIu32 "\n"
    "Maximum number of blocks searched ever:   %12" PRIu32 "\n"
    "Lifetime number of bytes allocated:       %12" PRIu64 "\n"
    "Lifetime number of bytes freed:           %12" PRIu64 "\n"
    "Total number of searches:                 %12" PRIu32 "\n"
    "Total number of successful allocations:   %12" PRIu32 "\n"
    "Total number of failed allocations:       %12" PRIu32 "\n"
    "Total number of successful frees:         %12" PRIu32 "\n"
    "Total number of successful resizes:       %12" PRIu32 "\n",
    s->size,
    s->min_free_size,
    s->max_free_blocks,
    s->max_search,
    s->lifetime_allocated,
    s->lifetime_freed,
    s->searches,
    s->allocs,
    s->failed_allocs,
    s->frees,
    s->resizes
  );
}
