/*  
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>
#include <stdlib.h>

int Ttimer_val;
bool benchmark_timer_find_average_overhead;

extern void timerisr(void);
extern int ClockIsOn(const rtems_raw_irq_connect_data*);

#define TMR0	  0xF040
#define TMR1	  0xF041
#define TMR2	  0xF042
#define TMRCON	  0xF043
#define TMRCFG    0xF834

void TimerOn(const rtems_raw_irq_connect_data* used)
{

  Ttimer_val = 0;                           /* clear timer ISR count */

  outport_byte  ( TMRCON , 0xb0 ); /* select tmr2, stay in mode 0 */
  outport_byte  ( TMR1   , 0xfa ); /* set to 250 usec interval */
  outport_byte	( TMR1   , 0x00 );
  outport_byte  ( TMRCON , 0x64 ); /* change to mode 2 ( starts timer ) */
                                   /* interrupts ARE enabled */
  /*  outport_byte( IERA, 0x41 );             enable interrupt */
  /*
   * enable interrrupt at i8259 level
   */
  BSP_irq_enable_at_i8259s(used->idtIndex - BSP_IRQ_VECTOR_BASE);
}

void TimerOff(const rtems_raw_irq_connect_data* used)
{
    /*
     * disable interrrupt at i8259 level
     */
     BSP_irq_disable_at_i8259s(used->idtIndex - BSP_IRQ_VECTOR_BASE);
     /* reset timer mode to standard (DOS) value */
}

static rtems_raw_irq_connect_data timer_raw_irq_data = {
  BSP_RT_TIMER3 + BSP_IRQ_VECTOR_BASE,
  timerisr,
  TimerOn,
  TimerOff,
  ClockIsOn
};

void Timer_exit(void)
{
 if (!i386_delete_idt_entry(&timer_raw_irq_data)) {
      printk("Timer_exit:Timer raw handler removal failed\n");
      rtems_fatal_error_occurred(1);
 }
}

void benchmark_timer_initialize(void)
{

  static bool First = true;

  if (First)
  {
    First = false;

    atexit(Timer_exit); /* Try not to hose the system at exit. */
    if (!i386_set_idt_entry (&timer_raw_irq_data)) {
      printk("benchmark_timer_initialize: raw handler installation failed\n");
      rtems_fatal_error_occurred(1);
    }
  }
  /* wait for ISR to be called at least once */
  Ttimer_val = 0;
  while (Ttimer_val == 0)
    continue;
  Ttimer_val = 0;
}

#define AVG_OVERHEAD      3  /* It typically takes 3.0 microseconds */
                             /* (3 ticks) to start/stop the timer. */
#define LEAST_VALID       4  /* Don't trust a value lower than this */

int benchmark_timer_read(void)
{
  register uint32_t         clicks;
  register uint32_t         total;

/*  outport_byte( TBCR, 0x00 );  stop the timer -- not needed on intel */

  outport_byte ( TMRCON, 0x40 );   /* latch the count */
  inport_byte  ( TMR1,   clicks ); /* read the count */

  total = Ttimer_val + 250 - clicks;

/*  outport_byte( TBCR, 0x00 );   initial value */
/*  outport_byte( IERA, 0x40 );   disable interrupt */

  /* ??? Is "do not restore old vector" causing problems? */

  if ( benchmark_timer_find_average_overhead == true )
    return total;          /* in one microsecond units */

  else {
    if ( total < LEAST_VALID )
      return 0;            /* below timer resolution */
    return (total - AVG_OVERHEAD);
  }
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_flag
)
{
  benchmark_timer_find_average_overhead = find_flag;
}
