/*  ckinit.c
 *
 *  This file provides a template for the clock device driver initialization.
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

#include <stdlib.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <bsp.h>
static void (*old_handler)(unsigned int,unsigned int,unsigned int,unsigned int);

void Clock_exit( void );
void Clock_isr( rtems_vector_number vector,unsigned int pc,
		unsigned int address, unsigned int sr);


/*
 *  The interrupt vector number associated with the clock tick device
 *  driver.
 */

#define CLOCK_VECTOR    8

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

rtems_unsigned32 Clock_isrs;              /* ISRs until next tick */

/*
 * These are set by clock driver during its init
 */
 
rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

void Clock_exit( void );

/* Write this to restart the Timer. Sets mode = 01,
   interrupts enabled, interrupt not pending, report
   at 200,000 (1 msec on 200 MHz CPU) */
static const unsigned int TTMR_RESET = 0x60030D40;

/*
 *  Isr Handler
 */

void Clock_isr(unsigned32 vector,
	       unsigned32 pc,
	       unsigned32 ear,
	       unsigned32 sr)
{
  register int pending;
  register int value = 0x60002710;

  /* Was it us? */
  asm volatile ("l.mfspr %0,r0,0x4802 \n\t"  /* Read the PIC status */
		"l.andi  %0,%0,0x8    \n\t" : "=r" (pending));

  if(pending)
    {
      rtems_clock_tick();
      asm ("l.mtspr r0,%0,0x5000 \n\t" :: "r" (value));
    }

  if(old_handler)
    (*old_handler)(vector,pc,ear,sr);
}
 
/*
 *  Install_clock
 *
 *  Install a clock tick handler and reprograms the chip.  This
 *  is used to initially establish the clock tick.
 */

void Install_clock()
{
  unsigned32 tmp,sr,ttmr,ttcr;
  extern unsigned32 Or1k_Interrupt_Vectors[16];

  ttmr = TTMR_RESET;    /* Reset value */
  ttcr = 0;             /* Start at 0 */

  /*
   *  Initialize the clock tick device driver variables
   */

  /* Make sure the Timer (interrupt 3) is enabled and
     reports a high prority interrupt */

  asm volatile ("l.mfspr %0,r0,0x4800  \n\t"  /* Get the PIC mask */
		"l.ori   %0,%0,0x8     \n\t"  /* Enable int 3 */
		"l.mtspr r0,%0,0x4800  \n\t"  /* Write back mask */
		"l.mfspr %0,r0,0x4801  \n\t"  /* Get priority mask */
		"l.ori   %0,%0,0x8     \n\t"  /* Set us to high */
		"l.mtspr r0,%0,0x4801  \n\t"  /* Write back to PICPR */
		: "=r" (tmp));

  /* Generate a 1 kHz interrupt */
  asm volatile ("l.mfspr %0,r0,0x11 \n\t"    /* Get the current setting */
		"l.addi  %1,r0,-5   \n\t"
		"l.and   %1,%1,%0   \n\t"    /* Turn off interrupts */
		"l.mtspr r0,%1,0x11 \n\t"    /* Set it in SR */
		"l.mtspr r0,%2,0x5000\n\t"   /* Set TTMR */
		"l.mtspr r0,%3,0x5100\n\t"   /* Set TTCR */
		: "=&r" (sr), "=&r" (tmp) : "r" (ttmr), "r" (ttcr));

  old_handler = (void(*)(unsigned int,unsigned int,unsigned int,unsigned int))
    Or1k_Interrupt_Vectors[8];
  Or1k_Interrupt_Vectors[8] = (unsigned32)Clock_isr;

  asm volatile ("l.mtspr r0,%0,0x11\n\t":: "r" (sr));
		 
  Clock_driver_ticks = 0;

  /*
   *  Schedule the clock cleanup routine to execute if the application exits.
   */

  atexit( Clock_exit );
}

/*
 *  Clean up before the application exits
 */

void Clock_exit( void )
{
  register int temp1;
  register int temp2;

  /* In the case of a fatal error, we should shut down
     all the interrupts so we don't get woken up again. */
  /* First, turn off the clock in the PIC */

  asm volatile ("l.mfspr %0,r0,0x4800  \n\t"  /* Get the PIC mask */
		"l.addi  %1,%1,-9      \n\t"  /* Create a mask to disable */
		"l.and   %0,%1,%0      \n\t"  /* Mask us out */
		"l.mtspr r0,%0,0x4800  \n\t"  /* Write back mask */
		"l.mfspr %0,r0,0x4801  \n\t"  /* Get priority mask */
		"l.and   %0,%1,%0      \n\t"  /* Set us to low */
		"l.mtspr r0,%0,0x4801  \n\t"  /* Write back to PICPR */
		: "=r" (temp1), "=r" (temp2));

  /* Now turn off the clock at the timer */
  asm volatile ("l.mtspr r0,r0,0x5000\n\t"   /* Clear TTMR */
		"l.mtspr r0,r0,0x5100\n\t"); /* Clear TTCR */
}

/*
 *  Clock_initialize
 *
 *  Device driver entry point for clock tick driver initialization.
 */

rtems_device_driver Clock_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  Install_clock();
 
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
        Clock_isr(CLOCK_VECTOR,0,0,0);
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
