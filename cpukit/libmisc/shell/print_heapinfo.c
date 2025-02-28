/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief Print Heap Information Structure
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
