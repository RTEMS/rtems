/*  Clock_init()
 *
 *  This routine initializes the Tick Timer 2 on the MVME147 board.
 *  The tick frequency is 1 millisecond.
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
 *  MVME147 port for TNI - Telecom Bretagne
 *  by Dominique LE CAMPION (Dominique.LECAMPION@enst-bretagne.fr)
 *  May 1996
 *
 *  $Id$
 */

#include <stdlib.h>

#include <bsp.h>
#include <rtems/libio.h>

#define MS_COUNT          65376    /* 1ms */
/* MS_COUNT = 0x10000 - 1e-3/6.25e-6 */
#define CLOCK_INT_LEVEL   6               /* T2's interrupt level */

rtems_unsigned32 Clock_isrs;                  /* ISRs until next tick */
volatile rtems_unsigned32 Clock_driver_ticks; /* ticks since initialization */
rtems_isr_entry  Old_ticker;

void Clock_exit( void );
 
/*
 * These are set by clock driver during its init
 */
 
rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;
 

/*
 *  ISR Handler
 */

rtems_isr Clock_isr(rtems_vector_number vector)
{
  Clock_driver_ticks += 1;
  pcc->timer2_int_control |= 0x80; /* Acknowledge interr. */

  if (Clock_isrs == 1) {
    rtems_clock_tick();
    Clock_isrs = BSP_Configuration.microseconds_per_tick / 1000;
  }
  else 
    Clock_isrs -= 1;
}

void Install_clock(rtems_isr_entry clock_isr )
{

  Clock_driver_ticks = 0;
  Clock_isrs = BSP_Configuration.microseconds_per_tick / 1000;

  Old_ticker = (rtems_isr_entry) set_vector( clock_isr, TIMER_2_VECTOR, 1 );

  pcc->timer2_int_control = 0x00; /* Disable T2 Interr. */
  pcc->timer2_preload = MS_COUNT;
  /* write preload value */
  pcc->timer2_control = 0x07; /* clear T2 overflow counter, enable counter */
  pcc->timer2_int_control = CLOCK_INT_LEVEL|0x08;
  /* Enable Timer 2 and set its int. level */
    
  atexit( Clock_exit );
}

void Clock_exit( void )
{
  pcc->timer2_int_control = 0x00; /* Disable T2 Interr. */
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
        Clock_isr(TIMER_2_VECTOR);
    }
    else if (args->command == rtems_build_name('N', 'E', 'W', ' '))
    {
      rtems_interrupt_disable( isrlevel );
       (void) set_vector( args->buffer, TIMER_2_VECTOR, 1 );
      rtems_interrupt_enable( isrlevel );
    }
 
done:
    return RTEMS_SUCCESSFUL;
}

