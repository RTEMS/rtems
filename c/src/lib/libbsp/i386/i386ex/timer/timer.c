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

int Ttimer_val;
rtems_boolean Timer_driver_Find_average_overhead;

extern void timerisr();
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

void Timer_exit()
{
 if (!i386_delete_idt_entry(&timer_raw_irq_data)) {
      printk("Timer_exit:Timer raw handler removal failed\n");
      rtems_fatal_error_occurred(1);
 }
}

void Timer_initialize()
{

  static rtems_boolean First = TRUE;

  if (First)
  {
    First = FALSE;

    atexit(Timer_exit); /* Try not to hose the system at exit. */
    if (!i386_set_idt_entry (&timer_raw_irq_data)) {
      printk("Timer_initialize: raw handler installation failed\n");
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

int Read_timer()
{
  register rtems_unsigned32 clicks;
  register rtems_unsigned32 total;

/*  outport_byte( TBCR, 0x00 );  stop the timer -- not needed on intel */

  outport_byte ( TMRCON, 0x40 );   /* latch the count */
  inport_byte  ( TMR1,   clicks ); /* read the count */ 

  total = Ttimer_val + 250 - clicks;

/*  outport_byte( TBCR, 0x00 );   initial value */
/*  outport_byte( IERA, 0x40 );   disable interrupt */

  /* ??? Is "do not restore old vector" causing problems? */

  if ( Timer_driver_Find_average_overhead == 1 )
    return total;          /* in one microsecond units */

  else {
    if ( total < LEAST_VALID )
      return 0;            /* below timer resolution */
    return (total - AVG_OVERHEAD);
  }
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
