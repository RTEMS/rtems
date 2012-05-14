/**
 *  @file
 *
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
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq.h>
#include <bspopts.h>
#include <libcpu/cpuModel.h>

#define CLOCK_VECTOR 0

volatile uint32_t pc386_microseconds_per_isr;
volatile uint32_t pc386_isrs_per_tick;
uint32_t pc386_clock_click_count;

/* forward declaration */
void Clock_isr(void *param);
void Clock_driver_support_at_tick_tsc(void);
void Clock_driver_support_at_tick_empty(void);
uint32_t bsp_clock_nanoseconds_since_last_tick_tsc(void);
uint32_t bsp_clock_nanoseconds_since_last_tick_i8254(void);
void Clock_isr_handler(rtems_irq_hdl_param param);
int clockIsOn(const rtems_irq_connect_data* unused);
void clockOff(const rtems_irq_connect_data* unused);
void Clock_driver_install_handler(void);
void Clock_driver_support_initialize_hardware(void);

/*
 * Roughly the number of cycles per tick and per nanosecond. Note that these
 * will be wildly inaccurate if the chip speed changes due to power saving
 * or thermal modes.
 *
 * NOTE: These are only used when the TSC method is used.
 */
uint64_t pc586_tsc_per_tick;
uint64_t pc586_nanoseconds_per_tick;

uint64_t pc586_tsc_at_tick;

/* this driver may need to count ISRs per tick */
#define CLOCK_DRIVER_ISRS_PER_TICK pc386_isrs_per_tick

/* if so, the driver may use the count in Clock_driver_support_at_tick */
#ifdef CLOCK_DRIVER_ISRS_PER_TICK
extern volatile uint32_t Clock_driver_isrs;
#endif

#define READ_8254( _lsb, _msb )                               \
  do { outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_LATCH);      \
     inport_byte(TIMER_CNTR0, _lsb);                          \
     inport_byte(TIMER_CNTR0, _msb);                          \
  } while (0)


/*
 *  Hooks which get swapped based upon which nanoseconds since last
 *  tick method is preferred.
 */
void     (*Clock_driver_support_at_tick)(void) = NULL;
uint32_t (*Clock_driver_nanoseconds_since_last_tick)(void) = NULL;

/*
 *  What do we do at each clock tick?
 */
void Clock_driver_support_at_tick_tsc(void)
{
#ifdef CLOCK_DRIVER_ISRS_PER_TICK
  /*
   *  The driver is multiple ISRs per clock tick.
  */
  if (!Clock_driver_isrs)
    pc586_tsc_at_tick = rdtsc();
#else
  /*
   *  The driver is one ISR per clock tick.
   */
  pc586_tsc_at_tick = rdtsc();
#endif
}

void Clock_driver_support_at_tick_empty(void)
{
}

#define Clock_driver_support_install_isr( _new, _old ) \
  do { \
    _old = NULL; \
  } while(0)

extern volatile uint32_t Clock_driver_isrs;

/*
 * Get nanoseconds using Pentium-compatible TSC register
 */
uint32_t bsp_clock_nanoseconds_since_last_tick_tsc(void)
{
  uint64_t                 diff_nsec;

  diff_nsec = rdtsc() - pc586_tsc_at_tick;

  /*
   * At this point, with a hypothetical 10 GHz CPU clock and 100 Hz tick
   * clock, diff_nsec <= 27 bits.
   */
  diff_nsec *= pc586_nanoseconds_per_tick; /* <= 54 bits */
  diff_nsec /= pc586_tsc_per_tick;

  if (diff_nsec > pc586_nanoseconds_per_tick)
    /*
     * Hmmm... Some drift or rounding. Pin the value to 1 nanosecond before
     * the next tick.
     */
    /*    diff_nsec = pc586_nanoseconds_per_tick - 1; */
    diff_nsec = 12345;

  return (uint32_t)diff_nsec;
}

/*
 * Get nanoseconds using 8254 timer chip
 */
uint32_t bsp_clock_nanoseconds_since_last_tick_i8254(void)
{
  uint32_t                 usecs, clicks, isrs;
  uint32_t                 usecs1, usecs2;
  uint8_t                  lsb, msb;
  rtems_interrupt_level    level;

  /*
   * Fetch all the data in an interrupt critical section.
   */
  rtems_interrupt_disable(level);
    READ_8254(lsb, msb);
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

/*
 * Calibrate CPU cycles per tick. Interrupts should be disabled.
 */
static void calibrate_tsc(void)
{
  uint64_t              begin_time;
  uint8_t               then_lsb, then_msb, now_lsb, now_msb;
  uint32_t              i;

  pc586_nanoseconds_per_tick =
    rtems_configuration_get_microseconds_per_tick() * 1000;

  /*
   * We just reset the timer, so we know we're at the beginning of a tick.
   */

  /*
   * Count cycles. Watching the timer introduces a several microsecond
   * uncertaintity, so let it cook for a while and divide by the number of
   * ticks actually executed.
   */

  begin_time = rdtsc();

  for (i = rtems_clock_get_ticks_per_second() * pc386_isrs_per_tick;
       i != 0; --i ) {
    /* We know we've just completed a tick when timer goes from low to high */
    then_lsb = then_msb = 0xff;
    do {
      READ_8254(now_lsb, now_msb);
      if ((then_msb < now_msb) ||
          ((then_msb == now_msb) && (then_lsb < now_lsb)))
        break;
      then_lsb = now_lsb;
      then_msb = now_msb;
    } while (1);
  }

  pc586_tsc_per_tick = rdtsc() - begin_time;

  /* Initialize "previous tick" counters */
  pc586_tsc_at_tick = rdtsc();

#if 0
  printk( "CPU clock at %u MHz\n", (uint32_t)(pc586_tsc_per_tick / 1000000));
#endif

  pc586_tsc_per_tick /= rtems_clock_get_ticks_per_second();
}

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

  BSP_irq_enable_at_i8259s( BSP_PERIODIC_TIMER - BSP_IRQ_VECTOR_BASE );

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

  /*
   * Now calibrate cycles per tick. Do this every time we
   * turn the clock on in case the CPU clock speed has changed.
   */
  if ( x86_has_tsc() )
    calibrate_tsc();
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

bool Clock_isr_enabled = false;
void Clock_isr_handler(
  rtems_irq_hdl_param param 
)
{
  if ( Clock_isr_enabled )
    Clock_isr( param );
}

static rtems_irq_connect_data clockIrqData = {
  BSP_PERIODIC_TIMER,
  Clock_isr_handler,
  0,
  clockOn,
  clockOff,
  clockIsOn
};

void Clock_driver_install_handler(void)
{
  if (!BSP_install_rtems_irq_handler (&clockIrqData)) {
    printk("Unable to install system clock ISR handler\n");
    rtems_fatal_error_occurred(1);
  }
}

void Clock_driver_support_initialize_hardware(void)
{
  bool use_tsc = false;
  bool use_8254 = false;

  #if (CLOCK_DRIVER_USE_TSC == 1)
    use_tsc = true;
  #endif

  #if (CLOCK_DRIVER_USE_8254 == 1)
    use_8254 = true;
  #endif

  if ( !use_tsc && !use_8254 ) {
    if ( x86_has_tsc() ) use_tsc  = true;
    else                 use_8254 = true;
  }

  if ( use_8254 ) {
    /* printk( "Use 8254\n" ); */
    Clock_driver_support_at_tick = Clock_driver_support_at_tick_empty;
    Clock_driver_nanoseconds_since_last_tick =
      bsp_clock_nanoseconds_since_last_tick_i8254;
  } else {
    /* printk( "Use TSC\n" ); */
    Clock_driver_support_at_tick = Clock_driver_support_at_tick_tsc;
    Clock_driver_nanoseconds_since_last_tick =
      bsp_clock_nanoseconds_since_last_tick_tsc;
  }

  /* Shell installs nanosecond handler before calling
   * Clock_driver_support_initialize_hardware() :-(
   * so we do it again now that we're ready.
   */
  rtems_clock_set_nanoseconds_extension(
    Clock_driver_nanoseconds_since_last_tick
  );

  Clock_isr_enabled = true;
}

#define Clock_driver_support_shutdown_hardware() \
  do { \
    BSP_remove_rtems_irq_handler (&clockIrqData); \
  } while (0)

#include "../../../shared/clockdrv_shell.h"

