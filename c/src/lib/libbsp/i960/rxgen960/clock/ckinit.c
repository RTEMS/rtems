/*  Clock_init()
 *
 *  This routine initializes the i960RP onboard timer 
 *  The tick frequency is 1 millisecond; assuming 33MHz core
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1997.
 *  RAMiX Inc 1999
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <stdlib.h>

#include <bsp.h>
#include <rtems/libio.h>

#define CLOCK_VECTOR 0x92

rtems_unsigned32 Clock_isrs;              /* ISRs until next tick */
rtems_unsigned32 Reload_Clock_isrs;

i960_isr_entry   Old_ticker;
volatile rtems_unsigned32 Clock_driver_ticks;
                                          /* ticks since initialization */
unsigned int clock_isr_global[16];  /* place to store global regs */

void Clock_exit( void );
 
/*
 * These are set by clock driver during its init
 */
 
rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

/* this is later in the file to avoid it being inlined */
rtems_isr Clock_isr( rtems_vector_number vector );

void Install_clock(
  rtems_isr_entry clock_isr
)
{
  volatile unsigned int *tmr0 = (unsigned int *) TMR0_ADDR;
  volatile unsigned int *trr0 = (unsigned int *) TRR0_ADDR;
  volatile unsigned int *tcr0 = (unsigned int *) TCR0_ADDR;
  volatile unsigned int *icon = (unsigned int *) ICON_ADDR;
  volatile unsigned int *imap2 = (unsigned int *) IMAP2_ADDR;
  volatile unsigned int *ipnd = (unsigned int *) IPND_ADDR;
  volatile unsigned int *imsk = (unsigned int *) IMSK_ADDR;
  void clockHandler();


  Clock_driver_ticks = 0;
  Reload_Clock_isrs = BSP_Configuration.microseconds_per_tick / 1000;
  Clock_isrs = Reload_Clock_isrs;

    #define BUS_CLOCK_1 0
    #define TMR_WRITE_CNTL 8
    #define TMR_AUTO_RELOAD 4
    #define TMR_ENABLE 2
    #define TMR_TERM_CNT_STAT 1

    Old_ticker = set_vector( (((unsigned int) clock_isr) | 0x2), CLOCK_VECTOR, 1 );

    /* initialize the i960RP timer 0 here */
    
    /* set the timer countdown (Assume 33MHz operation) */
    *trr0 = 30 * BSP_Configuration.microseconds_per_tick ;
    *tcr0 = 30 * BSP_Configuration.microseconds_per_tick ;
/*
kkprintf("Load the timers with %x\n", 30 * BSP_Configuration.microseconds_per_tick / Reload_Clock_isrs);
*/
  
    *tmr0 = BUS_CLOCK_1 | TMR_AUTO_RELOAD | TMR_ENABLE;
/* Unmask the interrupts */
	*ipnd &= ~(1 << 12);
	*imsk |= 1 << 12;

}

void Clock_exit()
{
  volatile unsigned int *tmr0 = (unsigned int *) TMR0_ADDR;

  /* shut down the timer */
  *tmr0 = *tmr0 & ~TMR_ENABLE;

  i960_mask_intr( 12 );
  /* do not restore old vector */
}

rtems_device_driver Clock_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  Install_clock( Clock_isr );
 
  atexit( Clock_exit );

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
       (void) set_tmr_vector( args->buffer, CLOCK_VECTOR, 0 );
      rtems_interrupt_enable( isrlevel );
    }
 
done:
    return RTEMS_SUCCESSFUL;
}

rtems_isr Clock_isr(
  rtems_vector_number vector
)
{
  /* enable_tracing(); */
#ifdef NOTMB
  if ( Clock_isrs == 1 ) {
    rtems_clock_tick();
    Clock_isrs = Reload_Clock_isrs;
  }
  else
  {
    Clock_isrs -= 1;
  }
#else
*(int *)(0xfed00000) += 1;
    rtems_clock_tick();
#endif

  i960_clear_intr( 12 );
}

