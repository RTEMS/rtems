/*  clock.c
 *
 *  This routine initializes the PIT on the MPC8xx.
 *  The tick frequency is specified by the bsp.
 *
 *  Author: Jay Monkman (jmonkman@frasca.com)
 *  Copyright (C) 1998 by Frasca International, Inc.
 *
 *  Derived from c/src/lib/libcpu/ppc/ppc403/clock/clock.c:
 *
 *  Author: Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/lib/libcpu/hppa1_1/clock/clock.c:
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>
#include <clockdrv.h>
#include <rtems/libio.h>

#include <stdlib.h>                     /* for atexit() */
#include <mpc8260.h>
#include <bsp/irq.h>

volatile rtems_unsigned32 Clock_driver_ticks;
extern int BSP_get_clock_irq_level();
extern int BSP_connect_clock_handler(rtems_isr_entry);
extern int BSP_disconnect_clock_handler();

void Clock_exit( void );

rtems_unsigned32 decrementer_value;

volatile int ClockInitialised = 0;


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
  int clicks;

  if( ClockInitialised ) {
    PPC_Get_decrementer( clicks );
    do {
      clicks += decrementer_value;
      PPC_Set_decrementer( clicks );

      Clock_driver_ticks++;
      rtems_clock_tick();
    } while( clicks < 100 );
  }
#if 0
  m8260.piscr |= M8260_PISCR_PS;
  Clock_driver_ticks++;
  rtems_clock_tick();
#endif


}

void clockOn(void* unused)
{

  decrementer_value = rtems_configuration_get_microseconds_per_tick() *
                      rtems_cpu_configuration_get_clicks_per_usec() - 1;

  PPC_Set_decrementer( decrementer_value );
  Clock_driver_ticks = 0;

  ClockInitialised = 1;


#if 0
  unsigned desiredLevel;
  rtems_unsigned32 pit_value;
  
  pit_value = (rtems_configuration_get_microseconds_per_tick() *
               rtems_cpu_configuration_get_clicks_per_usec()) - 1 ;
  
  if (pit_value > 0xffff) {           /* pit is only 16 bits long */
    rtems_fatal_error_occurred(-1);
  }
  m8260.sccr &= ~(1<<24);
  m8260.pitc = pit_value;

  desiredLevel = BSP_get_clock_irq_level();
  /* set PIT irq level, enable PIT, PIT interrupts */
  /*  and clear int. status */
  m8260.piscr = /*M8260_PISCR_PIRQ(desiredLevel) |*/
    M8260_PISCR_PTE | M8260_PISCR_PS | M8260_PISCR_PIE;
#endif
}
/*
 * Called via atexit()
 * Remove the clock interrupt handler by setting handler to NULL
 */
void
clockOff(void* unused)
{
#if 0
  /* disable PIT and PIT interrupts */
  m8260.piscr &= ~(M8260_PISCR_PTE | M8260_PISCR_PIE);
#endif
  ClockInitialised = 0;
}

int clockIsOn(void* unused)
{
  return ClockInitialised;
#if 0
  if (m8260.piscr & M8260_PISCR_PIE) return 1;
  return 0;
#endif

}

/*
 * Called via atexit()
 * Remove the clock interrupt handler by setting handler to NULL
 */
void
Clock_exit(void)
{
  (void) BSP_disconnect_clock_handler ();
}

void Install_clock(rtems_isr_entry clock_isr)
{
  Clock_driver_ticks = 0;


  decrementer_value = rtems_configuration_get_microseconds_per_tick() *
                      rtems_cpu_configuration_get_clicks_per_usec() - 1;

  PPC_Set_decrementer( decrementer_value );

  BSP_connect_clock_handler (clock_isr);


  ClockInitialised = 1;

  atexit(Clock_exit);

}

void
ReInstall_clock(rtems_isr_entry new_clock_isr)
{
  BSP_connect_clock_handler (new_clock_isr);
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
  
  if (args->command == rtems_build_name('I', 'S', 'R', ' ')) {
    Clock_isr(BSP_PERIODIC_TIMER);
  }
  else if (args->command == rtems_build_name('N', 'E', 'W', ' ')) {
    ReInstall_clock(args->buffer);
  }
  
 done:
  return RTEMS_SUCCESSFUL;
}

