/*  Timer_init()
 *
 *  This routine initializes the timer on the FORCE CPU-386 board.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  NOTE: This routine will not work if the optimizer is enabled
 *        for some compilers.  The multiple writes to the Z8036
 *        may be optimized away.
 *
 *        It is important that the timer start/stop overhead be
 *        determined when porting or modifying this code.
 *
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

volatile rtems_unsigned32  Ttimer_val;  /* Updated from ISR!!! */
rtems_boolean Timer_driver_Find_average_overhead;

extern void timerisr();

/* 
 * Number of us per timer interrupt. Note: 1 us == 1 tick.
 */

#define  US_PER_ISR   250

void TimerOn(const rtems_raw_irq_connect_data* used)
{

  Ttimer_val = 0;                           /* clear timer ISR count */

  /* Select timer, stay in mode 0 */
  outport_byte (TIMER_MODE, TIMER_SEL0|TIMER_16BIT|TIMER_RATEGEN);

  /* Set to 250 usec interval */
  outport_byte (TIMER_CNTR0, US_PER_ISR & 0xFF );
  outport_byte (TIMER_CNTR0, (US_PER_ISR >> 8) & 0xFF );

  /* Enable all timers */
  outport_byte (TIMER_CONFIG, 0x00);

  /*
   * enable interrrupt at i8259 level
   */
  BSP_irq_enable_at_i8259s(used->idtIndex - BSP_IRQ_VECTOR_BASE);
}

static int TimerIsOn (const rtems_raw_irq_connect_data *used)
{
  return BSP_irq_enabled_at_i8259s(used->idtIndex - BSP_IRQ_VECTOR_BASE);
}

void TimerOff(const rtems_raw_irq_connect_data* used)
{
  /* Disable all timers */
  outport_byte (TIMER_CONFIG, 0x80);

  /*
   * disable interrrupt at i8259 level
   */
  BSP_irq_disable_at_i8259s(used->idtIndex - BSP_IRQ_VECTOR_BASE);
  /* reset timer mode to standard (DOS) value */
}

static rtems_raw_irq_connect_data timer_raw_irq_data = {
  BSP_PERIODIC_TIMER + BSP_IRQ_VECTOR_BASE,
  timerisr,
  TimerOn,
  TimerOff,
  TimerIsOn
};

static rtems_raw_irq_connect_data old_raw_irq_data = {
  BSP_PERIODIC_TIMER + BSP_IRQ_VECTOR_BASE,
};  

void Timer_exit()
{
 if (!i386_delete_idt_entry(&timer_raw_irq_data)) {
      printk("Timer_exit:Timer raw handler removal failed\n");
      rtems_fatal_error_occurred(1);
 }
}

void Timer_initialize(void)
{

  static rtems_boolean First = TRUE;

  if (First)
  {
    First = FALSE;

    if (!i386_get_current_idt_entry (&old_raw_irq_data)) {
      printk("Timer_initialize: failed to get old raw irq entry.\n");
      rtems_fatal_error_occurred(1);
    }

    if (!i386_delete_idt_entry (&old_raw_irq_data)) {
      printk("Timer_initialize: failed to delete old raw irq entry.\n");
      rtems_fatal_error_occurred(1);
    }

    atexit(Timer_exit); /* Try not to hose the system at exit. */
    if (!i386_set_idt_entry (&timer_raw_irq_data)) {
      printk("Timer_initialize: raw handler installation failed.\n");
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

int Read_timer(void)
{
  register rtems_unsigned32 clicks, total;
  register rtems_unsigned8 lsb, msb;

  /* latch the count */
  outport_byte (TIMER_MODE, TIMER_SEL0|TIMER_LATCH );

  /* read the count */ 
  inport_byte (TIMER_CNTR0, lsb );
  inport_byte (TIMER_CNTR0, msb );

  /*
   * Timer ISR increments Ttimer_val every US_PER_ISR clock ticks,
   * where 1 tick == 1 us. Below, 'click' is in microseconds.
   *
   * This assumes the timer input clocks are sourced from the system's
   * prescaled clock (PSCLK), which is set to be at 1MHz.
   */
  clicks = (msb << 8) | lsb;
  total = Ttimer_val * US_PER_ISR + (US_PER_ISR - clicks);

  if ( Timer_driver_Find_average_overhead == 1 )
    return total;          /* in one microsecond units */
  else if ( total < LEAST_VALID )
    return 0;            /* below timer resolution */
  else
    return (total - AVG_OVERHEAD);
}

rtems_status_code Empty_function( void )
{
  return RTEMS_SUCCESSFUL;
}

void Set_find_average_overhead(
  rtems_boolean find_flag
)
{
  Timer_driver_Find_average_overhead = find_flag;
}

/*
 *  Wait_X_ms: a delay loop used by the network driver...
 */

#define SLOW_DOWN_IO  0x80      /* I/O port which does nothing */
#define LOOP1MS        320      /* Just a guess....  */

void Wait_X_ms (unsigned timeToWait)
{
  unsigned i, j;

  for (j=0; j<timeToWait; j++)
    for (i=0; i<LOOP1MS; i++)
      outport_byte (SLOW_DOWN_IO, 0);
}
