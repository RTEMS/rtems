/*  clock.c
 *
 *  This routine initializes the PIT on the MPC821.
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
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <clockdrv.h>
#include <rtems/libio.h>

#include <stdlib.h>                     /* for atexit() */
#include <mpc821.h>

volatile rtems_unsigned32 Clock_driver_ticks;
extern volatile m821_t m821;

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
  m821.piscr |= M821_PISCR_PS;
  Clock_driver_ticks++;
  rtems_clock_tick();
}

void Install_clock(rtems_isr_entry clock_isr)
{
  rtems_isr_entry previous_isr;
  rtems_unsigned32 pit_value;
  
  Clock_driver_ticks = 0;
  
  pit_value = rtems_configuration_get_microseconds_per_tick() /
               rtems_cpu_configuration_get_clicks_per_usec();
  if (pit_value == 0) {
    pit_value = 0xffff;
  } else {
    pit_value--;
  }
  
  if (pit_value > 0xffff) {           /* pit is only 16 bits long */
    rtems_fatal_error_occurred(-1);
  }  

  /*
   * initialize the interval here
   * First tick is set to right amount of time in the future
   * Future ticks will be incremented over last value set
   * in order to provide consistent clicks in the face of
   * interrupt overhead
   */
  
  rtems_interrupt_catch(clock_isr, PPC_IRQ_LVL0, &previous_isr);
    
  m821.sccr &= ~(1<<24);
  m821.pitc = pit_value;
    
  /* set PIT irq level, enable PIT, PIT interrupts */
  /*  and clear int. status */
  m821.piscr = M821_PISCR_PIRQ(0) |
    M821_PISCR_PTE | M821_PISCR_PS | M821_PISCR_PIE; 
    
  m821.simask |= M821_SIMASK_LVM0;
  atexit(Clock_exit);
}

void
ReInstall_clock(rtems_isr_entry new_clock_isr)
{
  rtems_isr_entry previous_isr;
  rtems_unsigned32 isrlevel = 0;
  
  rtems_interrupt_disable(isrlevel);
  
  rtems_interrupt_catch(new_clock_isr, PPC_IRQ_LVL0, &previous_isr);
  
  rtems_interrupt_enable(isrlevel);
}


/*
 * Called via atexit()
 * Remove the clock interrupt handler by setting handler to NULL
 */
void
Clock_exit(void)
{
  /* disable PIT and PIT interrupts */
  m821.piscr &= ~(M821_PISCR_PTE | M821_PISCR_PIE); 
  
  (void) set_vector(0, PPC_IRQ_LVL0, 1);
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
    Clock_isr(PPC_IRQ_LVL0);
  }
  else if (args->command == rtems_build_name('N', 'E', 'W', ' ')) {
    ReInstall_clock(args->buffer);
  }
  
 done:
  return RTEMS_SUCCESSFUL;
}

