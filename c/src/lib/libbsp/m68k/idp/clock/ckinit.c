/*  Clock_init()
 *
 *
 *  This is modified by Doug McBride to get it to work for the MC68EC040
 *  IDP board.  The below comments are kept to show that some prior work
 *  was done in the area and the modifications performed was application
 *  specific for the IDP board to port it to.
 *
 *  This routine initializes the mc68230 on the MC68EC040 board.
 *  The tick frequency is 40 milliseconds.
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

extern rtems_configuration_table Configuration;
extern void led_putnum();
void Disable_clock();

#define CLOCK_VECTOR 0x4D

void Clock_exit( void );
 
/*
 * These are set by clock driver during its init
 */
 
rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;
 

/*
 *  ISR Handler
 *
 *
 * ((1ms * 6.5 MHz) / 2^5) = 203.125) where 6.5 MHz is the clock rate of the
 * MC68230, 2^5 is the prescaler factor, and 1ms is the common interrupt
 * interval for the Clock_isr routine.
 * Therefore, 203 (decimal) is the number to program into the CPRH-L registers
 * of the MC68230 for countdown.  However, I have found that 193 instead of
 * 203 provides greater accuracy -- why?  The crystal should be more accurate
 * than that
 */

rtems_isr Clock_isr(
  rtems_vector_number vector
)
{
  Clock_driver_ticks += 1;
  /* acknowledge interrupt
  	TSR = 1; */
  MC68230_WRITE (TSR, 1);

  if ( Clock_isrs == 1 ) {
    rtems_clock_tick();
	/* Cast to an integer so that 68EC040 IDP which doesn't have an FPU doesn't
	   have a heart attack -- if you use newlib1.6 or greater and get
	   libgcc.a for gcc with software floating point support, this is not
	   a problem */
    Clock_isrs = 
      (int)(BSP_Configuration.microseconds_per_tick / 1000);
  }
  else 
    Clock_isrs -= 1;
}

void Disable_clock()
{
	/* Disable timer */
	MC68230_WRITE (TCR, 0x00);
}

void Install_clock( clock_isr )
rtems_isr_entry clock_isr;
{
  Clock_driver_ticks = 0;
  Clock_isrs = (int)(Configuration.microseconds_per_tick / 1000);

/*    led_putnum('c'); * for debugging purposes */
    Old_ticker = (rtems_isr_entry) set_vector( clock_isr, CLOCK_VECTOR, 1 );

  /* Disable timer for initialization */
  MC68230_WRITE (TCR, 0x00);

  /* some PI/T initialization stuff here -- see comment in the ckisr.c
     file in this directory to understand why I use the values that I do */
  /* Set up the interrupt vector on the MC68230 chip:
  TIVR = CLOCK_VECTOR; */
  MC68230_WRITE (TIVR, CLOCK_VECTOR);

  /* Set CPRH through CPRL to 193 (not 203) decimal for countdown--see ckisr.c
  	CPRH = 0x00;
  	CPRM = 0x00;
  	CPRL = 0xC1; */
  MC68230_WRITE (CPRH, 0x00);
  MC68230_WRITE (CPRM, 0x00);
  MC68230_WRITE (CPRL, 0xC1);

  /* Enable timer and use it as an external periodic interrupt generator
  	TCR = 0xA1; */
/*    led_putnum('a'); * for debugging purposes */
  MC68230_WRITE (TCR, 0xA1);

  /*
   *  Schedule the clock cleanup routine to execute if the application exits.
   */
  atexit( Clock_exit );
}

/* The following was added for debugging purposes */
void Clock_exit( void )
{
  rtems_unsigned8 data;

  /* disable timer
  	data = TCR;
  	TCR = (data & 0xFE); */
  MC68230_READ (TCR, data);
  MC68230_WRITE (TCR, (data & 0xFE));

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

