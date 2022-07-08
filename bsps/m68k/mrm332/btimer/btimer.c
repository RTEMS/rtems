/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-1999.
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

#include <bsp.h>
#include <rtems/btimer.h>

bool benchmark_timer_find_average_overhead;

extern rtems_isr Clock_isr(void);

void benchmark_timer_initialize( void )
{
}

/*
 *  The following controls the behavior of benchmark_timer_read().
 *
 *  FIND_AVG_OVERHEAD *  instructs the routine to return the "raw" count.
 *
 *  AVG_OVEREHAD is the overhead for starting and stopping the timer.  It
 *  is usually deducted from the number returned.
 *
 *  LEAST_VALID is the lowest number this routine should trust.  Numbers
 *  below this are "noise" and zero is returned.
 */

#define AVG_OVERHEAD      0  /* It typically takes X.X microseconds */
                             /* (Y countdowns) to start/stop the timer. */
                             /* This value is in microseconds. */
#define LEAST_VALID       1  /* Don't trust a clicks value lower than this */

/*
 * Return timer value in 1/2-microsecond units
 */
benchmark_timer_t benchmark_timer_read( void )
{
  uint32_t         total;
  total = 0;

  if ( benchmark_timer_find_average_overhead == true )
    return total;          /* in XXX microsecond units */

  if ( total < LEAST_VALID )
    return 0;            /* below timer resolution */

  return (total - AVG_OVERHEAD);
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_flag
)
{
  benchmark_timer_find_average_overhead = find_flag;
}
