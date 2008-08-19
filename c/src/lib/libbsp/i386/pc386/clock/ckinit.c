/*
 *  Clock Tick Device Driver
 *
 *  History:
 *    + Original driver was go32 clock by Joel Sherrill
 *    + go32 clock driver hardware code was inserted into new
 *      boilerplate when the pc386 BSP by:
 *        Pedro Miguel Da Cruz Neto Romano <pmcnr@camoes.rnl.ist.utl.pt>
 *        Jose Rufino <ruf@asterix.ist.utl.pt>
 *    + Reworked by Joel Sherrill to use clock driver template.
 *      This removes all boilerplate and leave original hardware
 *      code I developed for the go32 BSP.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>
#include <bsp/irq.h>
#include <bspopts.h>

#define CLOCK_VECTOR 0

volatile uint32_t pc386_microseconds_per_isr;
volatile uint32_t pc386_isrs_per_tick;
uint32_t pc386_clock_click_count;


/* this driver may need to count ISRs per tick */

#define CLOCK_DRIVER_ISRS_PER_TICK pc386_isrs_per_tick

#define Clock_driver_support_at_tick()

#define Clock_driver_support_install_isr( _new, _old ) \
  do { \
  } while(0)

extern volatile uint32_t Clock_driver_isrs;

uint32_t bsp_clock_nanoseconds_since_last_tick(void)
{
  uint32_t                 usecs, clicks, isrs;
  uint32_t                 usecs1, usecs2;
  uint8_t                  lsb, msb;
  rtems_interrupt_level    level;

  /*
   * Fetch all the data in an interrupt critical section.
   */
  rtems_interrupt_disable(level);
    outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_LATCH);
    inport_byte(TIMER_CNTR0, lsb);
    inport_byte(TIMER_CNTR0, msb);
    isrs = Clock_driver_isrs;
  rtems_interrupt_enable(level);

  /*
   *  Now do the math
   */
  /* convert values read into counter clicks */
  clicks = ((msb << 8) | lsb);

  /* whole ISRs we have done since the last tick */
  usecs1 = (pc386_isrs_per_tick - isrs - 1) * pc386_microseconds_per_isr;

  /* the partial ISR we in the middle of now */
  usecs2 = pc386_microseconds_per_isr - TICK_TO_US(clicks);

  /* total microseconds */
  usecs = usecs1 + usecs2;
  #if 0
    printk( "usecs1=%d usecs2=%d ", usecs1, usecs2 );
    printk( "maxclicks=%d clicks=%d ISRs=%d ISRsper=%d usersPer=%d usecs=%d\n",
    pc386_clock_click_count, clicks,
    Clock_driver_isrs, pc386_isrs_per_tick,
    pc386_microseconds_per_isr, usecs );
  #endif

  /* return it in nanoseconds */
  return usecs * 1000;
}

#define Clock_driver_nanoseconds_since_last_tick \
  bsp_clock_nanoseconds_since_last_tick

static void clockOn(
  const rtems_irq_connect_data* unused
)
{
  pc386_isrs_per_tick        = 1;
  pc386_microseconds_per_isr = rtems_configuration_get_microseconds_per_tick();

  while (US_TO_TICK(pc386_microseconds_per_isr) > 65535) {
    pc386_isrs_per_tick  *= 10;
    pc386_microseconds_per_isr /= 10;
  }
  pc386_clock_click_count = US_TO_TICK(pc386_microseconds_per_isr);

  #if 0
    printk( "configured usecs per tick=%d \n", 
      rtems_configuration_get_microseconds_per_tick() );
    printk( "Microseconds per ISR =%d\n", pc386_microseconds_per_isr );
    printk( "final ISRs per=%d\n", pc386_isrs_per_tick );
    printk( "final timer counts=%d\n", pc386_clock_click_count );
  #endif

  outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_16BIT|TIMER_RATEGEN);
  outport_byte(TIMER_CNTR0, pc386_clock_click_count >> 0 & 0xff);
  outport_byte(TIMER_CNTR0, pc386_clock_click_count >> 8 & 0xff);
}

void clockOff(const rtems_irq_connect_data* unused)
{
  /* reset timer mode to standard (BIOS) value */
  outport_byte(TIMER_MODE, TIMER_SEL0 | TIMER_16BIT | TIMER_RATEGEN);
  outport_byte(TIMER_CNTR0, 0);
  outport_byte(TIMER_CNTR0, 0);
} /* Clock_exit */

int clockIsOn(const rtems_irq_connect_data* unused)
{
  return ((i8259s_cache & 0x1) == 0);
}

/* a bit of a hack since the ISR models do not match */
rtems_isr Clock_isr(
  rtems_vector_number vector
);
static rtems_irq_connect_data clockIrqData = {
  BSP_PERIODIC_TIMER,
  (void *)Clock_isr,
  0,
  clockOn,
  clockOff,
  clockIsOn
};

#define Clock_driver_support_initialize_hardware() \
  do { \
    if (!BSP_install_rtems_irq_handler (&clockIrqData)) { \
      printk("Unable to initialize system clock\n"); \
      rtems_fatal_error_occurred(1); \
    } \
  } while (0)

#define Clock_driver_support_shutdown_hardware() \
  do { \
    BSP_remove_rtems_irq_handler (&clockIrqData); \
  } while (0)

#include "../../../shared/clockdrv_shell.c"

