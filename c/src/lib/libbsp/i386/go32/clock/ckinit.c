/*  Clock_initialize
 *
 *  This routine initializes the 8254 timer under GO32.
 *  The tick frequency is 1 millisecond.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>

#include <stdlib.h>

volatile rtems_unsigned32 Clock_driver_ticks;
rtems_unsigned32 Clock_isrs_per_tick;  /* ISRs per tick */
rtems_unsigned32 Clock_isrs;   /* ISRs until next tick */
rtems_isr_entry  Old_ticker;

#define CLOCK_VECTOR 0x8

void Clock_exit( void );

/*
 * These are set by clock driver during its init
 */
 
rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

rtems_isr Clock_isr(
  rtems_vector_number vector
)
{
    /* touch interrupt controller for irq0 (0x20+0) */
    outport_byte( 0x20, 0x20 );

    Clock_driver_ticks += 1;

#if 0 && defined(pentium)
    {
      extern long long Last_RDTSC;
      __asm __volatile( ".byte 0x0F, 0x31" : "=A" (Last_RDTSC) );
    }
#endif

    if ( Clock_isrs == 1 ) {
      rtems_clock_tick();
      Clock_isrs = Clock_isrs_per_tick;
    } else {
      Clock_isrs -= 1;
    }
}

void Install_clock(
  rtems_isr_entry clock_isr
)
{
    unsigned int microseconds_per_isr;

#if 0
    /* Initialize clock from on-board real time clock.  This breaks the */
    /* test code which assumes which assumes the application will do it. */
    {
       rtems_time_of_day Now;
       extern void init_rtc( void );
       extern long rtc_read( rtems_time_of_day * tod );
       init_rtc();
       if ( rtc_read( &Now ) >= 0 )
         clock_set( &Now );
       }
#endif

    /* Start by assuming hardware counter is large enough, then */
    /* scale it until it actually fits.    */
    Clock_driver_ticks = 0;
    Clock_isrs_per_tick = 1;

    if ( BSP_Configuration.microseconds_per_tick == 0 )
       microseconds_per_isr = 10000; /* default 10 ms */
    else
       microseconds_per_isr = BSP_Configuration.microseconds_per_tick;
    while ( US_TO_TICK(microseconds_per_isr) > 65535 )  {
      Clock_isrs_per_tick  *= 10;
      microseconds_per_isr /= 10;
    }

    /* Initialize count in ckisr.c */
    Clock_isrs = Clock_isrs_per_tick;

#if 0
    /* This was dropped in the last revision.  Its a nice thing to know. */
    TICKS_PER_SECOND = 1000000 / (Clock_isrs_per_tick * microseconds_per_isr);
#endif

    if ( BSP_Configuration.ticks_per_timeslice )  {
      /* 105/88 approximates TIMER_TICK*1e-6 */
      unsigned int count = US_TO_TICK( microseconds_per_isr );

      Old_ticker = (rtems_isr_entry) set_vector( clock_isr, CLOCK_VECTOR, 1 );
      outport_byte( TIMER_MODE, TIMER_SEL0|TIMER_16BIT|TIMER_RATEGEN );
      outport_byte( TIMER_CNTR0, count >> 0 & 0xff );
      outport_byte( TIMER_CNTR0, count >> 8 & 0xff );
    } 
    atexit( Clock_exit );
}

void ReInstall_clock(
  rtems_isr_entry clock_isr
)
{
  rtems_unsigned32 isrlevel = 0;

  rtems_interrupt_disable( isrlevel );
   (void) set_vector( clock_isr, CLOCK_VECTOR, 1 );
  rtems_interrupt_enable( isrlevel );
}

void Clock_exit( void )
{
  if ( BSP_Configuration.ticks_per_timeslice ) {
 extern void rtc_set_dos_date( void );

 /* reset to DOS value: */
 outport_byte( TIMER_MODE, TIMER_SEL0|TIMER_16BIT|TIMER_RATEGEN );
 outport_byte( TIMER_CNTR0, 0 );
 outport_byte( TIMER_CNTR0, 0 );

 /* reset time-of-day */
 rtc_set_dos_date();
 
 /* re-enable old handler: assume it was one of ours */
 set_vector( (rtems_isr_entry)Old_ticker, CLOCK_VECTOR, 1 );
  }
}

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
        ReInstall_clock(args->buffer);
    }
 
done:
    return RTEMS_SUCCESSFUL;
}

#if 0 && defined(pentium)
/* This can be used to get extremely accurate timing on a pentium. */
/* It isn't supported. [bryce]      */
#define HZ 90.0
volatile long long Last_RDTSC;
#define RDTSC()\
  ({ long long _now; __asm __volatile (".byte 0x0F,0x31":"=A"(_now)); _now; })

long long Kernel_Time_ns( void )
{
    extern rtems_unsigned32 _TOD_Ticks_per_second;
    unsigned isrs_per_second = Clock_isrs_per_tick * _TOD_Ticks_per_second;
    long long now;
    int flags;
    disable_intr( flags );
    now = 1e9 * Clock_driver_ticks / isrs_per_second 
       + (RDTSC() - Last_RDTSC) * (1000.0/HZ);
    enable_intr( flags );
    return now;
}
#endif
