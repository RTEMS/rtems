/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  MVME147 port for TNI - Telecom Bretagne
 *  by Dominique LE CAMPION (Dominique.LECAMPION@enst-bretagne.fr)
 *  May 1996
 */

#include <rtems/btimer.h>
#include <bsp.h>

#define TIMER_INT_LEVEL 6

#define COUNTDOWN_VALUE 0
/* Allows 0.4096 second delay betwin ints */
/* Each tick is 6.25 us */

int Ttimer_val;
bool benchmark_timer_find_average_overhead;

rtems_isr_entry timerisr(rtems_vector_number);

void benchmark_timer_initialize(void)
{
  (void) set_vector(timerisr, TIMER_1_VECTOR, 0); /* install ISR */

  Ttimer_val = 0;                 /* clear timer ISR count */
  pcc->timer1_int_control = 0x00; /* Disable T1 Interr. */
  pcc->timer1_preload = COUNTDOWN_VALUE;
  /* write countdown preload value */
  pcc->timer1_control = 0x00; /* load preload value */
  pcc->timer1_control = 0x07; /* clear T1 overflow counter, enable counter */
  pcc->timer1_int_control = TIMER_INT_LEVEL|0x08;
  /* Enable Timer 1 and set its int. level */

}

#define AVG_OVERHEAD      0  /* No need to start/stop the timer to read
				its value on the MVME147 PCC: reads are not
				synchronized whith the counter updates*/
#define LEAST_VALID       10 /* Don't trust a value lower than this */

uint32_t benchmark_timer_read(void)
{
  uint32_t         total;
  uint16_t         counter_value;

  counter_value = pcc->timer1_count; /* read the counter value */

  total = ((Ttimer_val * 0x10000) + counter_value); /* in 6.25 us units */
  /* DC note : just look at the assembly generated
     to see gcc's impressive optimization ! */
  return total;

}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_flag
)
{
  benchmark_timer_find_average_overhead = find_flag;
}
