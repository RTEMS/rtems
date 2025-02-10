/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This file implements a benchmark timer using the count/compare
 *  CP0 registers.
*/

/*
 * Copyright (C) 2005 by Cogent Computer Systems
 * Written by Jay Monkman <jtm@lopingdog.com>
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

#include <assert.h>

#include <bsp.h>
#include <rtems/btimer.h>

bool benchmark_timer_find_average_overhead;
uint32_t tstart;

void benchmark_timer_initialize(void)
{
    __asm__ volatile ("mfc0 %0, $9\n" : "=r" (tstart));
    /* tick time in picooseconds */
}

#define AVG_OVERHEAD      0  /* It typically takes N instructions */
                             /*     to start/stop the timer. */
#define LEAST_VALID       1  /* Don't trust a value lower than this */
                             /* tx39 simulator can count instructions. :) */

benchmark_timer_t benchmark_timer_read(void)
{
  uint32_t  total;
  uint32_t  cnt;

  __asm__ volatile ("mfc0 %0, $9\n" : "=r" (cnt));

  total = cnt - tstart;
  total = (total * 1000) / 396; /* convert to nanoseconds */


  if ( benchmark_timer_find_average_overhead == true )
    return total;          /* in one microsecond units */

  if ( total < LEAST_VALID )
    return 0;            /* below timer resolution */

  return total - AVG_OVERHEAD;
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_flag
)
{
  benchmark_timer_find_average_overhead = find_flag;
}
