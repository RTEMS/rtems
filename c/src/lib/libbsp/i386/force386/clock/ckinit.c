/*  Clock_initialize
 *
 *  This routine initializes the Motorola MFP 68901 on the
 *  FORCE CPU386 board.  The tick frequency is 1 millisecond.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <bsp.h>

#include <rtems/libio.h>

#include <stdlib.h>

#define CLOCK_VECTOR  0x38

rtems_unsigned32 Clock_isrs;              /* ISRs until next tick */

volatile rtems_unsigned32 Clock_driver_ticks;

rtems_isr_entry  Old_ticker;

void Clock_exit( void );
 
/*
 * These are set by clock driver during its init
 */
 
rtems_device_major_number rtems_clock_major = ~0;
rtems_device_major_number rtems_clock_minor = 0;

/*
 *  This is the ISR handler.
 */

rtems_isr Clock_isr(
  rtems_vector_number vector
)
{
  /* enable_tracing(); */
  Clock_driver_ticks += 1;
  if ( Clock_isrs == 1 ) {
    rtems_clock_tick();
    Clock_isrs = BSP_Configuration.microseconds_per_tick / 1000;
  }
  else
    Clock_isrs -= 1;
}

void Install_clock(
  rtems_isr_entry clock_isr
)
{
  Clock_driver_ticks = 0;
  Clock_isrs = BSP_Configuration.microseconds_per_tick / 1000;

  if ( BSP_Configuration.ticks_per_timeslice ) {
    Old_ticker = ( rtems_isr_entry ) set_vector( clock_isr, CLOCK_VECTOR, 1 );
    outport_byte( TBCR, 0x14 );  /* reset it, delay mode, 50X */
    outport_byte( TBDR, 0x50 );  /* 1 millisecond */
    outport_byte( IERA, 0x41 );  /* enable interrupt for B */
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
    outport_byte( TBCR, 0x00 );  /* initial value */
    outport_byte( IERA, 0x40 );  /* disable interrupt */
    /* ??? Is "do not restore old vector" causing problems? */
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
