/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsOR1K
 *
 * @brief Benchmark timer support.
 */

/*
 * Copyright (c) 2014-2015 by Hesham ALMatary
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

#include <rtems.h>
#include <rtems/btimer.h>
#include <bsp/generic_or1k.h>
#include <rtems/score/or1k-utility.h>

#define OR1K_NANOSECONDS_PER_CLK_CYCLE 10

static bool benchmark_timer_find_average_overhead = false;
static uint64_t benchmark_timer_base;

void benchmark_timer_initialize(void)
{
  benchmark_timer_base = _OR1K_mfspr(CPU_OR1K_SPR_TTCR);
}

#define AVG_OVERHEAD  0
#define LEAST_VALID   1

benchmark_timer_t benchmark_timer_read( void )
{
  uint64_t         clicks;
  uint64_t         total;
  uint64_t         delta;
  /*
   *  Read the timer and see how many clicks (clock cycles)
   *  has passed since timer initialization.
   */
  clicks = _OR1K_mfspr(CPU_OR1K_SPR_TTCR);

  delta = clicks - benchmark_timer_base;

  /* total in nanoseconds */
  total = OR1K_NANOSECONDS_PER_CLK_CYCLE * (delta);

  if ( benchmark_timer_find_average_overhead == true )
    return total;          /* in nanoseconds microsecond units */
  else {
    if ( total < LEAST_VALID )
      return 0;            /* below timer resolution */

    return (total - AVG_OVERHEAD);
  }
}

void benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
{
  benchmark_timer_find_average_overhead = find_flag;
}
