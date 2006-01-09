/*
 *  Clock Tick Device Driver
 *
 *  This routine initializes LEON timer 1 which used for the clock tick.
 *
 *  The tick frequency is directly programmed to the configured number of
 *  microseconds per tick.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Ported to LEON implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space 
 *  Agency (ESA).
 *
 *  LEON modifications of respective RTEMS file: COPYRIGHT (c) 1995. 
 *  European Space Agency.
 *
 *  $Id$
 */

#include <stdlib.h>

#include <bsp.h>
#include <rtems/libio.h>

/*
 *  The Real Time Clock Counter Timer uses this trap type.
 */

#define CLOCK_VECTOR LEON_TRAP_TYPE( LEON_INTERRUPT_TIMER1 )

/*
 *  Clock ticks since initialization
 */

volatile rtems_unsigned32 Clock_driver_ticks;

/*
 *  This is the value programmed into the count down timer.  It
 *  is artificially lowered when SIMSPARC_FAST_IDLE is defined to
 *  cut down how long we spend in the idle task while executing on
 *  the simulator.
 */

extern rtems_unsigned32 CPU_SPARC_CLICKS_PER_TICK;

rtems_isr_entry  Old_ticker;

void Clock_exit( void );
 
/*
 * These are set by clock driver during its init
 */
 
rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

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

rtems_isr Clock_isr(
  rtems_vector_number vector
)
{
  /*
   *  If we are in "fast idle" mode, then the value for clicks per tick
   *  is lowered to decrease the amount of time spent executing the idle
   *  task while using the SPARC Instruction Simulator.
   */

#if SIMSPARC_FAST_IDLE
  LEON_REG.Real_Time_Clock_Counter = CPU_SPARC_CLICKS_PER_TICK;
  LEON_REG_Set_Real_Time_Clock_Timer_Control(
    LEON_REG_TIMER_COUNTER_ENABLE_COUNTING | 
      LEON_REG_TIMER_COUNTER_LOAD_COUNTER
  );
#endif

  /*
   *  The driver has seen another tick.
   */

  Clock_driver_ticks += 1;

  /*
   *  Real Time Clock counter/timer is set to automatically reload.
   */

  rtems_clock_tick();
}

/*
 *  Install_clock
 *
 *  This routine actually performs the hardware initialization for the clock.
 *
 *  Input parameters:
 *    clock_isr - clock interrupt service routine entry point
 *
 *  Output parameters:  NONE
 *
 *  Return values:      NONE
 *
 */

void Install_clock(
  rtems_isr_entry clock_isr
)
{
  Clock_driver_ticks = 0;

  if ( BSP_Configuration.ticks_per_timeslice ) {
    Old_ticker = (rtems_isr_entry) set_vector( clock_isr, CLOCK_VECTOR, 1 );

    LEON_REG.Timer_Reload_1  = CPU_SPARC_CLICKS_PER_TICK - 1;

    LEON_REG.Timer_Control_1 = (
      LEON_REG_TIMER_COUNTER_ENABLE_COUNTING | 
        LEON_REG_TIMER_COUNTER_RELOAD_AT_ZERO |
        LEON_REG_TIMER_COUNTER_LOAD_COUNTER 
    );
 
    atexit( Clock_exit );
  }

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
  if ( BSP_Configuration.ticks_per_timeslice ) {
    LEON_Mask_interrupt( LEON_INTERRUPT_TIMER1 );

    LEON_REG.Timer_Control_1 = 0;

    /* do not restore old vector */
  }
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
  Install_clock( Clock_isr );
 
  /*
   * make major/minor avail to others such as shared memory driver
   */
 
  rtems_clock_major = major;
  rtems_clock_minor = minor;
 
  return RTEMS_SUCCESSFUL;
}
 
/*
 *  Clock_control
 *
 *  This routine is the clock device driver control entry point.
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

rtems_device_driver Clock_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
    rtems_unsigned32 isrlevel;
    rtems_libio_ioctl_args_t *args = pargp;
 
    if (args == 0)
        goto done;
 
    /*
     * This is hokey, but until we get a defined interface
     * to do this, it will just be this simple...
     */
 
    if (args->command == rtems_build_name('I', 'S', 'R', ' '))
    {
        Clock_isr(CLOCK_VECTOR);
    }
    else if (args->command == rtems_build_name('N', 'E', 'W', ' '))
    {
      rtems_interrupt_disable( isrlevel );
       (void) set_vector( args->buffer, CLOCK_VECTOR, 1 );
      rtems_interrupt_enable( isrlevel );
    }
 
done:
    return RTEMS_SUCCESSFUL;
}
