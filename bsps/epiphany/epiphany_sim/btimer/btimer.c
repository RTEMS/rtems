/*
 * Copyright (c) 2015 University of York.
 * Hesham ALMatary <hmka501@york.ac.uk>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/btimer.h>
#include <rtems/score/epiphany-utility.h>

extern char bsp_start_vector_table_begin[];

bool benchmark_timer_find_average_overhead;

static void benchmark_timer1_interrupt_handler(void)
{
  unsigned int val = 0xFFFFFFFF;
  unsigned int event_type = 0x1;

  /* Embed assembly code for setting timer1 */
  __asm__ __volatile__ ("movts ctimer1, %[val] \t \n" :: [val] "r" (val));

  __asm__ __volatile__ ("movfs r16, config; \t \n"
                "mov   r17, %%low(0xfffff0ff);\t \n"
                "movt  r17, %%high(0xffff0ff);\t \n"
                "lsl   r18, %[event_type], 0x8; \t \n"
                "and   r16, r16, r17; \t \n"
                "orr   r16, r16, r18; \t \n"
                "movts config, r16; \t \n"
                :: [event_type] "r" (event_type));
}

/* Start eCore tiemr 1 usef for profiling and timing analysis */
void benchmark_timer_initialize( void )
{
  /* Install interrupt handler for timer 1 */

  void (**table)(void) = (void (**)(void)) bsp_start_vector_table_begin;

  table[TIMER1] = benchmark_timer1_interrupt_handler;

  benchmark_timer1_interrupt_handler();
}

/*
 *  The following controls the behavior of benchmark_timer_read().
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

benchmark_timer_t benchmark_timer_read( void )
{
  uint32_t timer_val = 0;
  uint32_t total;

  __asm__ __volatile__ ("movfs %[timer_val], ctimer1 \t \n"
    :[timer_val] "=r" (timer_val):);

  total = (0xFFFFFFFF - timer_val);

  if ( benchmark_timer_find_average_overhead == true )
    return total;
  else {
    if ( total < LEAST_VALID )
      return 0;            /* below timer resolution */
  /*
   *  Somehow convert total into microseconds
   */
      return (total - AVG_OVERHEAD);
    }
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_flag
)
{
  benchmark_timer_find_average_overhead = find_flag;
}
