/*  Clock_init()
 *
 *  This routine initializes the Z80386 1 on the MVME136 board.
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
 *  $Id$
 */

#include <stdlib.h>
 
#include <bsp.h>
#include <rtems/libio.h>

rtems_unsigned32 Clock_isrs;        /* ISRs until next tick */
volatile rtems_unsigned32 Clock_driver_ticks;
                                    /* ticks since initialization */
rtems_isr_entry  Old_ticker;

void Clock_exit( void );

#define CLOCK_VECTOR  TIMER_VECTOR
 
/*
 * These are set by clock driver during its init
 */
 
rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

/*
 *  ISR Handler
 */

rtems_isr Clock_isr(
  rtems_vector_number vector
)
{
  Clock_driver_ticks += 1;

  Z8x36_WRITE( TIMER, CT1_CMD_STATUS, 0xE2 );
  Z8x36_WRITE( TIMER, CT1_CMD_STATUS, 0x22 );
  Z8x36_WRITE( TIMER, CT1_CMD_STATUS, 0xC6 );

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
  rtems_unsigned8 data;

  Clock_driver_ticks = 0;
  Clock_isrs = BSP_Configuration.microseconds_per_tick / 1000;

  Old_ticker = (rtems_isr_entry) set_vector( clock_isr, CLOCK_VECTOR, 1 );

  Z8x36_WRITE( TIMER, MASTER_CFG, 0xd4 );
  Z8x36_READ ( TIMER, MASTER_INTR, data );
  Z8x36_WRITE( TIMER, MASTER_INTR, (data & 0x7E) );
  Z8x36_WRITE( TIMER, CT1_TIME_CONST_MSB, 0x04 );
  Z8x36_WRITE( TIMER, CT1_TIME_CONST_LSB, 0xCE );
  Z8x36_WRITE( TIMER, CT1_MODE_SPEC, 0x83 );
  Z8x36_WRITE( TIMER, CNT_TMR_VECTOR, CLOCK_VECTOR );
  Z8x36_WRITE( TIMER, CT1_CMD_STATUS, 0x20 );
  Z8x36_READ ( TIMER, MASTER_INTR, data );
  Z8x36_WRITE( TIMER, MASTER_INTR, (data & 0xDA) | 0x80 );

  /*
   * ACC_IC54 - interrupt 5 will be vectored and mapped to level 6
   */

  data = (*(rtems_unsigned8 *)0x0D00000B);
  (*(rtems_unsigned8 *)0x0D00000B) = (data & 0x7F) | 0x60;

  Z8x36_WRITE( TIMER, CT1_CMD_STATUS, 0xC6 );

  atexit( Clock_exit );
}

void Clock_exit( void )
{
  rtems_unsigned8 data;

  Z8x36_READ ( TIMER, MASTER_INTR, data );
  Z8x36_WRITE( TIMER, MASTER_INTR, (data & 0x01) );
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

