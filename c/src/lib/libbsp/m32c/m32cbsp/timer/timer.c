/*
 *  This file implements a stub benchmark timer that is sufficient to
 *  satisfy linking the RTEMS Benchmarks.
 *
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 */

#include <bsp.h>
#include <rtems/btimer.h>

#include <varvects.h>

bool benchmark_timer_find_average_overhead;
uint32_t benchmark_timer_overhead = 10;

#define TABSR *((uint8_t *)0x340)
#define TA0MR *((uint8_t *)0x356)
#define TA0   *((uint16_t *)0x346)
#define TA0IC *((uint8_t *)0x6c)

static int benchmark_timer_interrupts;

#define ivec_timer_a0 12

void __attribute__((interrupt))
timer_ra_interrupt(void)
{
  benchmark_timer_interrupts++;
  TA0IC = 0x05;

}

void benchmark_timer_initialize(void)
{
  benchmark_timer_interrupts = 0;
  _set_var_vect (timer_ra_interrupt, ivec_timer_a0);
  TA0MR = 0x00;
  TA0   = 0xffff;
  TA0IC = 0x05;
  TABSR = 0x55;
}

uint32_t benchmark_timer_read(void)
{
  uint32_t count;

  count = 0xFFFF - TA0;
  count += benchmark_timer_interrupts * 0xFFFFL;

  if (!benchmark_timer_find_average_overhead) {
    if ( count > benchmark_timer_overhead )
      count -= benchmark_timer_overhead;
    else
      count = 0;
  }
  return count;
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_flag
)
{
  benchmark_timer_find_average_overhead = find_flag;
}
