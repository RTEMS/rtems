/*  Clock_init()
 *
 *  This routine initializes Timer 1 for an MC68302.
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

#include <stdlib.h>			/* for atexit() */

#include <bsp.h>
#include <rtems/libio.h>

#include "m68302.h"

#define CLOCK_VECTOR 137

#define TMR1_VAL (  RBIT_TMR_RST	/* software reset the timer */\
		  | RBIT_TMR_ICLK_MASTER16 /* master clock divided by 16 */\
		  | RBIT_TMR_FRR	/* restart timer after ref reached */\
		  | RBIT_TMR_ORI)	/* enable interrupt when ref reached */
#define TRR1_VAL 1000			/* 1000 ticks @ 16MHz/16
					 *    = 1 millisecond tick.
					 */

/*
 *  Clock_driver_ticks is a monotonically increasing counter of the
 *  number of clock ticks since the driver was initialized.
 */
volatile rtems_unsigned32 Clock_driver_ticks;

/*
 *  Clock_isrs is the number of clock ISRs until the next invocation of
 *  the RTEMS clock tick routine.  The clock tick device driver
 *  gets an interrupt once a millisecond and counts down until the
 *  length of time between the user configured microseconds per tick
 *  has passed.
 */
rtems_unsigned32 Clock_isrs;

void Clock_exit( void );
 
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

  m302.reg.isr  = RBIT_ISR_TIMER1;	/* clear in-service bit */
  m302.reg.ter1 = (RBIT_TER_REF | RBIT_TER_CAP); /* clear timer intr request */

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

  set_vector( clock_isr, CLOCK_VECTOR, 1 );

  m302.reg.trr1 = TRR1_VAL;		/* set timer reference register */
  m302.reg.tmr1 = TMR1_VAL;		/* set timer mode register & enable */
  /*
   * Enable TIMER1 interrupts only.
   */
  m302.reg.imr |= RBIT_IMR_TIMER1;	/* set 68302 int-mask to allow ints */

  atexit( Clock_exit );
}

void Clock_exit( void )
{
  /* TODO: figure out what to do here */
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
        Clock_isr( CLOCK_VECTOR);
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

