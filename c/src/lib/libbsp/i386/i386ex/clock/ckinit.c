/*  Clock_initialize
 *
 *  This routine initializes the Timer/Counter on the Intel
 *  386ex evaluation board.
 *
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
#define TMR0	  0xF040
#define TMR1	  0xF041
#define TMR2	  0xF042
#define TMRCON	  0xF043
#define TMRCFG    0xF834

#include <bsp.h>
#include <irq.h>

#include <rtems/libio.h>

#include <stdlib.h>

rtems_unsigned32 Clock_isrs;              /* ISRs until next tick */
static rtems_unsigned32 Clock_initial_isr_value;

volatile rtems_unsigned32 Clock_driver_ticks;

void Clock_exit( void );

/*
 * These are set by clock driver during its init
 */
 
rtems_device_major_number rtems_clock_major = ~0;
rtems_device_major_number rtems_clock_minor = 0;

/*
 *  This is the ISR handler.
 */

void Clock_isr()
{
  /* enable_tracing(); */
  Clock_driver_ticks += 1;
  if ( Clock_isrs == 1 ) {
    rtems_clock_tick();
    Clock_isrs = Clock_initial_isr_value; /* BSP_Configuration.microseconds_per_tick / 1000;*/
  }
  else
    Clock_isrs -= 1;
}

void ClockOff(const rtems_irq_connect_data* unused)
{
  outport_byte	( TMRCFG , 0x80 ); /* disable the counter timer */
}

void ClockOn(const rtems_irq_connect_data* unused)
{
  outport_byte    ( TMRCFG , 0x00 ); /* enable the counter timer */
}

int ClockIsOn(const rtems_irq_connect_data* unused)
{
  return ((i8259s_cache & 0x1) == 0);
}

static rtems_irq_connect_data clockIrqData = {BSP_PERIODIC_TIMER,
					      Clock_isr,
					      ClockOn,
					      ClockOff,
					      ClockIsOn};


rtems_device_driver Clock_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  unsigned timer_counter_init_value;
  unsigned char clock_lsb, clock_msb;
  
  Clock_driver_ticks = 0;

  Clock_isrs = 
    Clock_initial_isr_value = 
    BSP_Configuration.microseconds_per_tick / 1000; /* ticks per clock_isr */
  
  /*
   * configure the counter timer ( should be based on microsecs/tick )
   * NB. The divisor(Clock_isrs) resolves the  is the same number that appears in confdefs.h
   * when setting the microseconds_per_tick value.
   */
  ClockOff      ( &clockIrqData );
  
  timer_counter_init_value  =  BSP_Configuration.microseconds_per_tick / Clock_isrs;
  clock_lsb = (unsigned char)timer_counter_init_value;
  clock_msb = timer_counter_init_value >> 8;
  
  outport_byte  ( TMRCON , 0x34 ); 
  outport_byte	( TMR0   , clock_lsb );       /* load LSB first */
  outport_byte  ( TMR0   , clock_msb );  /* then MSB       */
 
  if (!BSP_install_rtems_irq_handler (&clockIrqData)) {
    printk("Unable to initialize system clock\n");
    rtems_fatal_error_occurred(1);
  }

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
        Clock_isr();
    }
    else if (args->command == rtems_build_name('N', 'E', 'W', ' '))
    {
      if (!BSP_install_rtems_irq_handler (&clockIrqData)) {
	printk("Error installing clock interrupt handler!\n");
	rtems_fatal_error_occurred(1);
      }
#ifdef DEBUG
      else
	printk("Clock installed AGAIN\n");
#endif
    }
 
done:
    return RTEMS_SUCCESSFUL;
}

void Clock_exit()
{
  ClockOff(&clockIrqData);  
  BSP_remove_rtems_irq_handler (&clockIrqData);
}
