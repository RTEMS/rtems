/*-------------------------------------------------------------------------+
| ckinit.c v1.1 - PC386 BSP - 1997/08/07
+--------------------------------------------------------------------------+
| This file contains the PC386 clock package.
+--------------------------------------------------------------------------+
| (C) Copyright 1997 -
| - NavIST Group - Real-Time Distributed Systems and Industrial Automation
|
| http://pandora.ist.utl.pt
|
| Instituto Superior Tecnico * Lisboa * PORTUGAL
+--------------------------------------------------------------------------+
| Disclaimer:
|
| This file is provided "AS IS" without warranty of any kind, either
| expressed or implied.
+--------------------------------------------------------------------------+
| This code is based on:
|   ckinit.c,v 1.4 1995/12/19 20:07:13 joel Exp - go32 BSP
| With the following copyright notice:
| **************************************************************************
| * COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.                      *
| * On-Line Applications Research Corporation (OAR).                       *
| * All rights assigned to U.S. Government, 1994.                          *
| *                                                                        *
| * This material may be reproduced by or for the U.S. Government pursuant *
| * to the copyright license under the clause at DFARS 252.227-7013.  This *
| * notice must appear in all copies of this file and its derivatives.     *
| **************************************************************************
+--------------------------------------------------------------------------*/


#include <stdlib.h>

#include <bsp.h>
#include <irq.h>
#include <rtems/libio.h>

/*-------------------------------------------------------------------------+
| Constants
+--------------------------------------------------------------------------*/
#define CLOCK_IRQ  0x00  /* Clock IRQ. */

/*-------------------------------------------------------------------------+
| Macros
+--------------------------------------------------------------------------*/
#if 0
/* This was dropped in the last revision.  Its a nice thing to know. */
#define TICKS_PER_SECOND() \
          (1000000 / (Clock_isrs_per_tick * microseconds_per_isr))
#endif /* 0 */

/*-------------------------------------------------------------------------+
| Global Variables
+--------------------------------------------------------------------------*/

volatile rtems_unsigned32 Clock_driver_ticks;   /* Tick (interrupt) counter. */
         rtems_unsigned32 Clock_isrs_per_tick;  /* ISRs per tick.            */
         rtems_unsigned32 Clock_isrs;           /* ISRs until next tick.     */

/* The following variables are set by the clock driver during its init */

rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

/*-------------------------------------------------------------------------+
|         Function: clockIsr
|      Description: Interrupt Service Routine for clock (08h) interruption.
| Global Variables: Clock_driver_ticks, Clock_isrs.
|        Arguments: vector - standard RTEMS argument - see documentation.
|          Returns: standard return value - see documentation. 
+--------------------------------------------------------------------------*/
static rtems_isr
clockIsr(rtems_vector_number vector)
{
  /*-------------------------------------------------------------------------+
  | PLEASE NOTE: The following is directly transcribed from the go32 BSP for
  |              those who wish to use it with PENTIUM based machine. It needs
  |              to be correctly integrated with the rest of the code!!!
  +--------------------------------------------------------------------------*/

#if 0 && defined(pentium) /* more accurate clock for PENTIUMs (not supported) */
  {
    extern long long Last_RDTSC;
    __asm __volatile(".byte 0x0F, 0x31" : "=A" (Last_RDTSC));
  }
#endif /* 0 && pentium */

  Clock_driver_ticks++;

  if ( Clock_isrs == 1 )
  {
    rtems_clock_tick();
    Clock_isrs = Clock_isrs_per_tick;
  }
  else
    Clock_isrs--;

  PC386_ackIrq(vector - PC386_IRQ_VECTOR_BASE);
} /* clockIsr */


/*-------------------------------------------------------------------------+
|         Function: Clock_exit
|      Description: Clock cleanup routine at RTEMS exit. NOTE: This routine is
|                   not really necessary, since there will be a reset at exit.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void Clock_exit(void)
{
  if (BSP_Configuration.ticks_per_timeslice)
  {
    /* reset timer mode to standard (BIOS) value */
    outport_byte(TIMER_MODE, TIMER_SEL0 | TIMER_16BIT | TIMER_RATEGEN);
    outport_byte(TIMER_CNTR0, 0);
    outport_byte(TIMER_CNTR0, 0);
  }
} /* Clock_exit */


/*-------------------------------------------------------------------------+
|         Function: Install_clock
|      Description: Initialize and install clock interrupt handler.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void
Install_clock(rtems_isr_entry isr)
{
  rtems_unsigned32  microseconds_per_isr;

  rtems_status_code status;
 
#if 0
  /* Initialize clock from on-board real time clock.  This breaks the  */
  /* test code which assumes which assumes the application will do it. */
  {
    rtems_time_of_day now;

    /* External Prototypes */
    extern void init_rtc(void);                /* defined in 'rtc.c' */
    extern long rtc_read(rtems_time_of_day *); /* defined in 'rtc.c' */

    init_rtc();
    if (rtc_read(&now) >= 0)
      clock_set(&now);
  }
#endif /* 0 */

  /* Start by assuming hardware counter is large enough, then  scale it until
     it actually fits. */

  Clock_driver_ticks  = 0;
  Clock_isrs_per_tick = 1;

  if (BSP_Configuration.microseconds_per_tick == 0)
    microseconds_per_isr = 10000; /* default 10 ms */
  else
    microseconds_per_isr = BSP_Configuration.microseconds_per_tick;
  while (US_TO_TICK(microseconds_per_isr) > 65535)
  {
    Clock_isrs_per_tick  *= 10;
    microseconds_per_isr /= 10;
  }

  Clock_isrs = Clock_isrs_per_tick; /* Initialize Clock_isrs */

  if (BSP_Configuration.ticks_per_timeslice)
  {
    /* 105/88 approximates TIMER_TICK * 1e-6 */
    rtems_unsigned32 count = US_TO_TICK(microseconds_per_isr);

    status = PC386_installRtemsIrqHandler(CLOCK_IRQ, isr);

    if (status != RTEMS_SUCCESSFUL)
    {
      printk("Error installing clock interrupt handler!\n");
      rtems_fatal_error_occurred(status);
    }

    outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_16BIT|TIMER_RATEGEN);
    outport_byte(TIMER_CNTR0, count >> 0 & 0xff);
    outport_byte(TIMER_CNTR0, count >> 8 & 0xff);
  } 

  atexit(Clock_exit);
} /* Install_clock */


/*-------------------------------------------------------------------------+
| Clock device driver INITIALIZE entry point.
+--------------------------------------------------------------------------+
| Initilizes the clock driver.
+--------------------------------------------------------------------------*/
rtems_device_driver
Clock_initialize(rtems_device_major_number major,
                 rtems_device_minor_number minor,
                 void                      *pargp)
{
  Install_clock(clockIsr); /* Install the interrupt handler */
 
  /* make major/minor avail to others such as shared memory driver */
 
  rtems_clock_major = major;
  rtems_clock_minor = minor;
 
  return RTEMS_SUCCESSFUL;
} /* Clock_initialize */


/*-------------------------------------------------------------------------+
| Console device driver CONTROL entry point
+--------------------------------------------------------------------------*/
rtems_device_driver
Clock_control(rtems_device_major_number major,
              rtems_device_minor_number minor,
	      void                      *pargp)
{
  if (pargp != NULL)
  {
    rtems_libio_ioctl_args_t *args = pargp;
        
    /*-------------------------------------------------------------------------+
    | This is hokey, but until we get a defined interface to do this, it will
    | just be this simple...
    +-------------------------------------------------------------------------*/
 
    if      (args->command == rtems_build_name('I', 'S', 'R', ' '))
      clockIsr(PC386_IRQ_VECTOR_BASE + CLOCK_IRQ);
    else if (args->command == rtems_build_name('N', 'E', 'W', ' '))
    {
      rtems_status_code status;
      
      status = PC386_installRtemsIrqHandler(CLOCK_IRQ, clockIsr);

      if (status != RTEMS_SUCCESSFUL)
      {
	printk("Error installing clock interrupt handler!\n");
	rtems_fatal_error_occurred(status);
      }
    }
  }

  return RTEMS_SUCCESSFUL;
} /* Clock_control */


/*-------------------------------------------------------------------------+
| PLEASE NOTE: The following is directly transcribed from the go32 BSP for
|              those who wish to use it with PENTIUM based machine. It needs
|              to be correctly integrated with the rest of the code!!!
+--------------------------------------------------------------------------*/


#if 0 && defined(pentium)

/* This can be used to get extremely accurate timing on a pentium. */
/* It isn't supported. [bryce]                                     */

#define HZ 90.0

volatile long long Last_RDTSC;

#define RDTSC()\
  ({ long long _now; __asm __volatile (".byte 0x0F,0x31":"=A"(_now)); _now; })

long long Kernel_Time_ns( void )
{
  extern rtems_unsigned32 _TOD_Ticks_per_second;

  unsigned  isrs_per_second = Clock_isrs_per_tick * _TOD_Ticks_per_second;
  long long now;
  int       flags;

  disable_intr(flags);
  now = 1e9 * Clock_driver_ticks / isrs_per_second +
        (RDTSC() - Last_RDTSC) * (1000.0/HZ);
  enable_intr(flags);
  return now;
} /* Kernel_Time_ns */

#endif /* 0 && pentium */
