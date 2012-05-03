/**
 *  @file
 *  
 *  Clock Tick Device Driver Shell
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <stdlib.h>

#include <bsp.h>

#if defined(CLOCK_DRIVER_USE_FAST_IDLE) && defined(CLOCK_DRIVER_ISRS_PER_TICK)
#error "clockdrv_shell.h: Fast Idle PLUS n ISRs per tick is not supported"
#endif

/*
 * This method is rarely used so default it.
 */
#ifndef Clock_driver_support_find_timer
  #define Clock_driver_support_find_timer()
#endif

/*
 *  ISRs until next clock tick
 */
#ifdef CLOCK_DRIVER_ISRS_PER_TICK
  volatile uint32_t  Clock_driver_isrs;
#endif

/*
 *  Clock ticks since initialization
 */
volatile uint32_t    Clock_driver_ticks;

void Clock_exit( void );

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
 */
#if defined(BSP_FEATURE_IRQ_EXTENSION) || \
    (CPU_SIMPLE_VECTORED_INTERRUPTS != TRUE)
void Clock_isr(void *arg)
{
#else 
rtems_isr Clock_isr(rtems_vector_number vector);
rtems_isr Clock_isr(
  rtems_vector_number vector
)
{
#endif
  /*
   *  Accurate count of ISRs
   */
  Clock_driver_ticks += 1;

  #ifdef CLOCK_DRIVER_USE_FAST_IDLE
    do {
      rtems_clock_tick();
    } while ( _Thread_Executing == _Thread_Idle &&
            _Thread_Heir == _Thread_Executing);

    Clock_driver_support_at_tick();
    return;
  #else
    /*
     *  Do the hardware specific per-tick action.
     *
     *  The counter/timer may or may not be set to automatically reload.
     */
    Clock_driver_support_at_tick();

    #ifdef CLOCK_DRIVER_ISRS_PER_TICK
      /*
       *  The driver is multiple ISRs per clock tick.
       */
      if ( !Clock_driver_isrs ) {
        rtems_clock_tick();

        Clock_driver_isrs = CLOCK_DRIVER_ISRS_PER_TICK;
      }
      Clock_driver_isrs--;
    #else
      /*
       *  The driver is one ISR per clock tick.
       */
      rtems_clock_tick();
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
  Clock_driver_support_shutdown_hardware();

  /* do not restore old vector */
}

/*
 *  Clock_initialize
 *
 *  This routine initializes the clock driver.
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
  rtems_isr_entry  Old_ticker;

  Clock_driver_ticks = 0;

  /*
   *  Find timer -- some BSPs search buses for hardware timer
   */
  Clock_driver_support_find_timer();

  /*
   *  Install vector
   */
  Clock_driver_support_install_isr( Clock_isr, Old_ticker );

  #if defined(Clock_driver_nanoseconds_since_last_tick)
    rtems_clock_set_nanoseconds_extension(
      Clock_driver_nanoseconds_since_last_tick
    );
  #endif

  /*
   *  Now initialize the hardware that is the source of the tick ISR.
   */
  Clock_driver_support_initialize_hardware();

  atexit( Clock_exit );

  /*
   *  If we are counting ISRs per tick, then initialize the counter.
   */
  #ifdef CLOCK_DRIVER_ISRS_PER_TICK
    Clock_driver_isrs = CLOCK_DRIVER_ISRS_PER_TICK;
  #endif

  return RTEMS_SUCCESSFUL;
}
