/*  Clock
 *
 *  This routine initializes the interval timer on the
 *  PA-RISC CPU.  The tick frequency is specified by the bsp.
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
#include <rtems/libio.h>

/* should get this from bsp.h, but it is not installed yet */
rtems_isr_entry set_vector(rtems_isr_entry, rtems_vector_number, int);

#include <stdlib.h>                     /* for atexit() */

typedef unsigned long long hppa_click_count_t;

/*
 * These are set by clock driver during its init
 */

rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

/*
 * CPU_HPPA_CLICKS_PER_TICK is either a #define or an rtems_unsigned32
 *   allocated and set by bsp_start()
 */

#ifndef CPU_HPPA_CLICKS_PER_TICK
extern rtems_unsigned32 CPU_HPPA_CLICKS_PER_TICK;
#endif

volatile rtems_unsigned32 Clock_driver_ticks;
rtems_unsigned32 Clock_isrs;              /* ISRs until next tick */

rtems_unsigned32   most_recent_itimer_value;

rtems_unsigned64   Clock_clicks;        /* running total of cycles */

rtems_unsigned32   Clock_clicks_interrupt;

void  Clock_exit(void);

void
ReInstall_clock(rtems_isr_entry new_clock_isr)
{
    rtems_unsigned32 isrlevel = 0;

    rtems_interrupt_disable(isrlevel);
    (void) set_vector(
      new_clock_isr,
      HPPA_INTERRUPT_EXTERNAL_INTERVAL_TIMER,
      1
    );
    rtems_interrupt_enable(isrlevel);
}

/*
 * read itimer and update Clock_clicks as appropriate
 */

rtems_unsigned32
Clock_read_itimer()
{
    rtems_unsigned32 isrlevel;
    rtems_unsigned32 itimer_value;
    rtems_unsigned32 wrap_count;
    rtems_unsigned32 recent_count;

    rtems_interrupt_disable(isrlevel);

    wrap_count = (Clock_clicks & 0xFFFFFFFF00000000ULL) >> 32;
    recent_count = (rtems_unsigned32) Clock_clicks;

    itimer_value = get_itimer();

    if (itimer_value < recent_count)
        wrap_count++;
    Clock_clicks = (((rtems_unsigned64) wrap_count) << 32) + itimer_value;

    rtems_interrupt_enable(isrlevel);

    return itimer_value;
}


void Install_clock(rtems_isr_entry clock_isr)
{
    Clock_driver_ticks = 0;
    Clock_clicks_interrupt = 0;
    Clock_clicks = 0;

    Clock_isrs = rtems_configuration_get_milliseconds_per_tick();

    /*
     * initialize the interval here
     * First tick is set to right amount of time in the future
     * Future ticks will be incremented over last value set
     * in order to provide consistent clicks in the face of
     * interrupt overhead
     */

    Clock_clicks_interrupt = Clock_read_itimer() + CPU_HPPA_CLICKS_PER_TICK;
    set_itimer((rtems_unsigned32) Clock_clicks_interrupt);

    (void) set_vector(clock_isr, HPPA_INTERRUPT_EXTERNAL_INTERVAL_TIMER, 1);
    atexit(Clock_exit);
}

rtems_isr
Clock_isr(rtems_vector_number vector)
{
    rtems_unsigned32 clicks_til_next_interrupt;
    rtems_unsigned32 itimer_value;

    /*
     * setup for next interrupt; making sure the new value is reasonably
     * in the future.... in case we lost out on an interrupt somehow
     */

    itimer_value = Clock_read_itimer();
    Clock_clicks_interrupt += CPU_HPPA_CLICKS_PER_TICK;

    /*
     * how far away is next interrupt *really*
     * It may be a long time; this subtraction works even if
     * Clock_clicks_interrupt < Clock_clicks_low_order via
     * the miracle of unsigned math.
     */
    clicks_til_next_interrupt = Clock_clicks_interrupt - itimer_value;

    /*
     * If it is too soon then bump it up.
     * This should only happen if CPU_HPPA_CLICKS_PER_TICK is too small.
     * But setting it low is useful for debug, so...
     */

    if (clicks_til_next_interrupt < 400)
    {
        Clock_clicks_interrupt = itimer_value + 1000;
        /* XXX: count these! this should be rare */
    }

    /*
     * If it is too late, that means we missed the interrupt somehow.
     * Rather than wait 35-50s for a wrap, we just fudge it here.
     */

    if (clicks_til_next_interrupt > CPU_HPPA_CLICKS_PER_TICK)
    {
        Clock_clicks_interrupt = itimer_value + 1000;
        /* XXX: count these! this should never happen :-) */
    }

    set_itimer((rtems_unsigned32) Clock_clicks_interrupt);

    Clock_driver_ticks++;

    if (Clock_isrs == 1)
    {
        rtems_clock_tick();
        Clock_isrs = rtems_configuration_get_milliseconds_per_tick();
        if (Clock_isrs == 0)
            Clock_isrs = 1;
    }
    else
        Clock_isrs--;
}

/*
 * Called via atexit()
 * Remove the clock interrupt handler by setting handler to NULL
 */

void
Clock_exit(void)
{
  (void) set_vector(0, HPPA_INTERRUPT_EXTERNAL_INTERVAL_TIMER, 1);
}

/*
 * spin delay for specified number of microseconds
 * used by RTEMS delay macro
 */

void
Clock_delay(rtems_unsigned32 microseconds)
{
    rtems_unsigned64 future_time;

    (void) Clock_read_itimer();
    future_time = Clock_clicks +
      ((rtems_unsigned64) microseconds) *
        rtems_cpu_configuration_get_itimer_clicks_per_microsecond();

    for (;;)
    {
        (void) Clock_read_itimer();
        if (future_time <= Clock_clicks)
            break;
    }
}

rtems_device_driver Clock_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
    Install_clock(Clock_isr);

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
    rtems_libio_ioctl_args_t *args = pargp;

    if (args == 0)
        goto done;

    /*
     * This is hokey, but until we get a defined interface
     * to do this, it will just be this simple...
     */

    if (args->command == rtems_build_name('I', 'S', 'R', ' '))
    {
        Clock_isr(HPPA_INTERRUPT_EXTERNAL_INTERVAL_TIMER);
    }
    else if (args->command == rtems_build_name('N', 'E', 'W', ' '))
    {
        ReInstall_clock(args->buffer);
    }
    
done:
    return RTEMS_SUCCESSFUL;
}
