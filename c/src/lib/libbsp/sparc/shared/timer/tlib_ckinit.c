/*
 *  Clock Tick Device Driver using Timer Library implemented
 *  by the GRLIB GPTIMER / LEON2 Timer drivers.
 *
 *  COPYRIGHT (c) 2010.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 */

#include <rtems.h>
#include <rtems/timecounter.h>
#include <rtems/score/percpu.h>
#include <stdlib.h>
#include <bsp.h>
#include <bsp/tlib.h>

#ifdef RTEMS_DRVMGR_STARTUP

/* Undefine this to save space in standard LEON configurations,
 * it will assume that Prescaler is running at 1MHz.
 */
#undef CLOCK_DRIVER_DONT_ASSUME_PRESCALER_1MHZ

/* Set the below defines from bsp.h if function needed.
#undef CLOCK_DRIVER_ISRS_PER_TICK
#undef CLOCK_DRIVER_USE_FAST_IDLE
*/

/*
 *  Number of Clock ticks since initialization
 */
volatile uint32_t Clock_driver_ticks;

/*
 *  Timer Number in Timer Library. Defaults to the first Timer in
 *  the System.
 */
int Clock_timer = 0;

/*
 * Timer Handle in Timer Library
 */
void *Clock_handle = NULL;

#ifdef CLOCK_DRIVER_DONT_ASSUME_PRESCALER_1MHZ
unsigned int Clock_basefreq;
#endif

void Clock_exit(void);
void Clock_isr(void *arg_unused);

static rtems_timecounter_simple tlib_tc;

static uint32_t tlib_tc_get(rtems_timecounter_simple *tc)
{
  unsigned int clicks = 0;

  if (Clock_handle != NULL) {
    tlib_get_counter(Clock_handle, &clicks);
  }

  return clicks;
}

static bool tlib_tc_is_pending(rtems_timecounter_simple *tc)
{
  bool pending = false;

  if (Clock_handle != NULL) {
    pending = tlib_interrupt_pending(Clock_handle, 0) != 0;
  }

  return pending;
}

static uint32_t tlib_tc_get_timecount(struct timecounter *tc)
{
  return rtems_timecounter_simple_downcounter_get(
    tc,
    tlib_tc_get,
    tlib_tc_is_pending
  );
}

static void tlib_tc_at_tick(rtems_timecounter_simple *tc)
{
  /* Nothing to do? */
}

static void tlib_tc_tick(void)
{
  rtems_timecounter_simple_downcounter_tick(
    &tlib_tc,
    tlib_tc_get,
    tlib_tc_at_tick
  );
}

/*
 *  Clock_isr
 *
 *  This is the clock tick interrupt handler.
 *
 *  Input parameters:
 *    vector - vector number
 *
 *  Output parameters:  NONE
 *
 *  Return values:      NONE
 *
 */

void Clock_isr(void *arg_unused)
{
  /*
   * Support for shared interrupts. Ack IRQ at source, only handle 
   * interrupts generated from the tick-timer.
   */
  if ( tlib_interrupt_pending(Clock_handle, 1) == 0 )
    return;

  /*
   *  Accurate count of ISRs
   */

  Clock_driver_ticks += 1;

#ifdef CLOCK_DRIVER_USE_FAST_IDLE
  do {
    tlib_tc_tick();
  } while ( _Thread_Executing == _Thread_Idle &&
          _Thread_Heir == _Thread_Executing);

  return;

#else

#ifdef CLOCK_DRIVER_ISRS_PER_TICK
  /*
   *  The driver is multiple ISRs per clock tick.
   */

  if ( !Clock_driver_isrs ) {

    tlib_tc_tick();

    Clock_driver_isrs = CLOCK_DRIVER_ISRS_PER_TICK;
  }
  Clock_driver_isrs--;
#else

  /*
   *  The driver is one ISR per clock tick.
   */
  tlib_tc_tick();
#endif
#endif
}

/*
 *  Clock_exit
 *
 *  This routine allows the clock driver to exit by masking the interrupt and
 *  disabling the clock's counter.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:      NONE
 *
 */

void Clock_exit( void )
{
  /* Stop all activity of the Timer, no more ISRs.
   * We could be using tlib_close(), however tlib_stop() is quicker
   * and independent of IRQ unregister code.
   */
  if ( Clock_handle ) {
    tlib_stop(Clock_handle);
    Clock_handle = NULL;
  }
}

/*
 *  Clock_initialize
 *
 *  This routine initializes the clock driver and starts the Clock.
 *
 *  Input parameters:
 *    major - clock device major number
 *    minor - clock device minor number
 *    parg  - pointer to optional device driver arguments
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    rtems_device_driver status code
 */

rtems_device_driver Clock_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  uint64_t frequency;
  unsigned int tick_hz;

  /*
   *  Take Timer that should be used as system timer.
   *
   */
  Clock_handle = tlib_open(Clock_timer);
  if ( Clock_handle == NULL ) {
    /* System Clock Timer not found */
    return RTEMS_NOT_DEFINED;
  }

  /*
   *  Install Clock ISR before starting timer
   */
  tlib_irq_register(Clock_handle, Clock_isr, NULL);

  /* Set Timer Frequency to tick at Configured value. The Timer
   * Frequency is set in multiples of the timer base frequency.
   *
   * In standard LEON3 designs the base frequency is is 1MHz, to
   * save instructions undefine CLOCK_DRIVER_DONT_ASSUME_PRESCALER_1MHZ
   * to avoid 64-bit calculation.
   */
#ifdef CLOCK_DRIVER_DONT_ASSUME_PRESCALER_1MHZ
  {
    uint64_t tmp;

    tlib_get_freq(Clock_handle, &Clock_basefreq, NULL);

    frequency = Clock_basefreq
    tmp = frequency * (uint64_t)rtems_configuration_get_microseconds_per_tick();
    tick_hz = tmp / 1000000;
  }
#else
  frequency = 1000000;
  tick_hz = rtems_configuration_get_microseconds_per_tick();
#endif

  tlib_set_freq(Clock_handle, tick_hz);

  rtems_timecounter_simple_install(
    &tlib_tc,
    frequency,
    tick_hz,
    tlib_tc_get_timecount
  );

  /*
   *  IRQ and Frequency is setup, now we start the Timer. IRQ is still
   *  disabled globally during startup, so IRQ will hold for a while.
   */
  tlib_start(Clock_handle, 0);

  /*
   *  Register function called at system shutdown
   */
  atexit( Clock_exit );

  /*
   *  If we are counting ISRs per tick, then initialize the counter.
   */

#ifdef CLOCK_DRIVER_ISRS_PER_TICK
  Clock_driver_isrs = CLOCK_DRIVER_ISRS_PER_TICK;
#endif

  return RTEMS_SUCCESSFUL;
}

/*** Timer Driver Interface ***/

/* Set system clock timer instance */
void Clock_timer_register(int timer_number)
{
  Clock_timer = timer_number;
}

#endif
