/*  ckinit.c
 *
 *  This file provides a template for the clock device driver initialization.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <stdlib.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <bsp.h>

void Clock_exit( void );
rtems_isr Clock_isr( rtems_vector_number vector );


/*
 *  The interrupt vector number associated with the clock tick device
 *  driver.
 */

#define CLOCK_VECTOR    PPC_IRQ_DECREMENTER

/*
 *  Clock_driver_ticks is a monotonically increasing counter of the
 *  number of clock ticks since the driver was initialized.
 */

volatile rtems_unsigned32 Clock_driver_ticks;

/*
 *  Clocks_per_tick is the number of clocks of the decrementer needed
 *  to fill BSP_Configuration.microseconds_per_tick.
 *  
 *  The clock rate is 1/4 the bus clock, or 16.666667 MHz, so:
 *
 *      Clocks_per_tick = ((50/3)*1E6 clocks/sec) * (1 sec/1E6 us) * (X us/tick)
 *                      = X * 50 / 3  clocks/tick
 *             where X = BSP_Configuration.microseconds_per_tick
 */

rtems_signed32 Clocks_per_tick;

/*
 * These are set by clock driver during its init
 */
 
rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

/*
 *  The previous ISR on this clock tick interrupt vector.
 */

rtems_isr_entry  Old_ticker;

void Clock_exit( void );

/*
 *  Inline assembly routines to access the decrementer register 
 */
static inline rtems_signed32 read_decrementer(void) {
    rtems_signed32 result;
    asm volatile ("mfdec %0" : "=r" (result) :);
    return result;
}

static inline void write_decrementer(rtems_signed32 value) {
    asm volatile ("mtdec %0" : : "r" (value));
}


/*
 *  Isr Handler
 */

rtems_isr Clock_isr(
  rtems_vector_number vector
)
{
    rtems_signed32 count, repeat = 0;
    const rtems_signed32 period = Clocks_per_tick;

    /*
     * bump the number of clock driver ticks since initialization
     */
    Clock_driver_ticks++;

    /*
     * be very paranoid and count number of "ticks" in case we missed some
     *
     * update the decrementer and signal rtems the appropriate number of times
     */
    count = read_decrementer();
    while (count < 0) {
        count += period;
        repeat++;
    }
    write_decrementer(count);

    while (repeat-- > 0) {
        rtems_clock_tick();
    }
}

/*
 *  Install_clock
 *
 *  Install a clock tick handler and reprograms the chip.  This
 *  is used to initially establish the clock tick.
 */

void Install_clock(
  rtems_isr_entry clock_isr
)
{
    /*
     *  Initialize the clock tick device driver variables
     */

    Clock_driver_ticks = 0;
    switch (Falcon_SYSCR.SystemClock) {
        case SYSCLK_50_MHZ:
            Clocks_per_tick = 
                    (BSP_Configuration.microseconds_per_tick * 25 + 1) / 2;
            break;
        case SYSCLK_60_MHZ:
            Clocks_per_tick = BSP_Configuration.microseconds_per_tick * 15;
            break;
        case SYSCLK_67_MHZ:
            Clocks_per_tick = 
                    (BSP_Configuration.microseconds_per_tick * 50 + 1) / 3;
            break;
    }

    /*
     *  If ticks_per_timeslice is configured as non-zero, then the user
     *  wants a clock tick.
     */

    if ( BSP_Configuration.ticks_per_timeslice ) {
        Old_ticker = (rtems_isr_entry) set_vector( clock_isr, CLOCK_VECTOR, 1 );

        /*
         *  Hardware specific initialize goes here
         */
        write_decrementer(Clocks_per_tick);
    }

    /*
     *  Schedule the clock cleanup routine to execute if the application exits.
     */

    atexit( Clock_exit );
}

/*
 *  Clean up before the application exits
 */

void Clock_exit( void )
{
    if ( BSP_Configuration.ticks_per_timeslice ) {

        /* XXX: turn off the timer interrupts */

        /* we can't really disable the timer without disabling all external
        interupts.  we'll slow down the decrementer to it's minimum speed. */
        write_decrementer(~0);

        /* XXX: If necessary, restore the old vector */
    }
}

/*
 *  Clock_initialize
 *
 *  Device driver entry point for clock tick driver initialization.
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
