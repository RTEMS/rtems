/*
 *  Code Modified for the MC68230 by Doug McBride, Colorado Space Grant College
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/btimer.h>
#include <bsp.h>
#include <rtems/motorola/mc68230.h>

#define TIMER_VECTOR 0x4D

int Ttimer_val;
bool benchmark_timer_find_average_overhead;

rtems_isr_entry timerisr(rtems_vector_number);

void benchmark_timer_initialize(void)
{
  (void) set_vector( timerisr, TIMER_VECTOR, 0 );  /* install ISR */

  Ttimer_val = 0;                          /* clear timer ISR count */

  /* some PI/T initialization stuff here */
  /* Set up the interrupt vector on the MC68230 chip:
     TIVR = TIMER_VECTOR; */
  MC68230_WRITE (MC68230_TIVR, TIMER_VECTOR);

  /* Set CPRH through CPRL to maximum count to reduce interrupt overhead
      CPRH = 0xFF;
      CPRM = 0xFF;
      CPRL = 0xFF; */
  MC68230_WRITE (MC68230_CPRH, 0xFF);
  MC68230_WRITE (MC68230_CPRM, 0xFF);
  MC68230_WRITE (MC68230_CPRL, 0xFF);

  /* Enable timer and use it as an external periodic interrupt generator
      TCR = 0xA1; */
  MC68230_WRITE (MC68230_TCR, 0xA1);

}

#define AVG_OVERHEAD      9  /* may not be right -- do this later */
#define LEAST_VALID       10 /* Don't trust a value lower than this */

uint32_t benchmark_timer_read(void)
{
  uint8_t         data;
  uint8_t          msb, osb, lsb;
  uint32_t         remaining, total;

  /* Disable timer so that timer can be read
        data = MC68230_TCR;
        MC68230_TCR = (data & 0xFE); */
  MC68230_READ (MC68230_TCR, data);
  MC68230_WRITE (MC68230_TCR, (data & 0xFE));

  /* Read the counter value
        msb = MC68230_CNTRH;
        osb = MC68230_CNTRM;
        lsb = MC68230_CNTRL; */
  MC68230_READ (MC68230_CNTRH, msb);
  MC68230_READ (MC68230_CNTRM, osb);
  MC68230_READ (MC68230_CNTRL, lsb);

  /* Calculate the time so far */
  remaining = 0x1000000 - ((msb << 16) + (osb << 8) + lsb);
  total = (Ttimer_val * 0x1000000) + remaining;

  /* Enable timer so that timer can continue
	 	MC68230_TCR = 0xA1; */
  MC68230_WRITE (MC68230_TCR, 0xA1);

  /* do not restore old vector */
  if ( benchmark_timer_find_average_overhead == true )
    return total;          /* in countdown units */

  if ( total < LEAST_VALID )
    return 0;            /* below timer resolution */

  /* Clocked at 6.5 Mhz */
  /* Avoid floating point problems, be lazy, and return the total minus
     the average overhead */
  return (total - AVG_OVERHEAD);
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_flag
)
{
  benchmark_timer_find_average_overhead = find_flag;
}
