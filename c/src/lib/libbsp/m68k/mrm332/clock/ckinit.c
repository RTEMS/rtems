/*  Clock_init()
 *
 *  This routine initailizes the periodic interrupt timer on
 *  the Motorola 68332.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
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

#include <stdlib.h>
#include <bsp.h>
#include <rtems/libio.h>
#include <mrm332.h>

#define CLOCK_VECTOR   MRM_PIV

rtems_unsigned32 Clock_isrs;        /* ISRs until next tick */
volatile rtems_unsigned32 Clock_driver_ticks;
                                    /* ticks since initialization */
rtems_isr_entry  Old_ticker;

void Clock_exit( void );
 
/*
 * These are set by clock driver during its init
 */
 
rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;
 
rtems_isr Clock_isr(rtems_vector_number vector)
{
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

  Old_ticker = (rtems_isr_entry) set_vector( clock_isr, CLOCK_VECTOR, 1 );

  /* enable 1mS interrupts */
  *PITR = (unsigned short int)( SAM(0x09,0,PITM) );/* load counter */
  *PICR = (unsigned short int)                     /* enable interrupt */
    ( SAM(ISRL_PIT,8,PIRQL) | SAM(CLOCK_VECTOR,0,PIV) );
  
  atexit( Clock_exit );
}

void Clock_exit( void )
{
  /* shutdown the periodic interrupt */
  *PICR = (unsigned short int)
    ( SAM(0,8,PIRQL) | SAM(CLOCK_VECTOR,0,PIV) );
  /*     ^^ zero disables interrupt */

  /* do not restore old vector */
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
 
