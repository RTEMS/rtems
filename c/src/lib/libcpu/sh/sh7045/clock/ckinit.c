/*
 *  This file contains the clock driver the Hitachi SH 704X
 *
 *  Authors: Ralf Corsepius (corsepiu@faw.uni-ulm.de) and
 *           Bernd Becker (becker@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *      Modified to reflect registers of sh7045 processor:
 *      John M. Mills (jmills@tga.com)
 *      TGA Technologies, Inc.
 *      100 Pinnacle Way, Suite 140
 *      Norcross, GA 30071 U.S.A.
 *      August, 1999
 *
 *	This modified file may be copied and distributed in accordance
 *	the above-referenced license. It is provided for critique and
 *	developmental purposes without any warranty nor representation
 *	by the authors or by TGA Technologies.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>

#include <stdlib.h>

#include <rtems/libio.h>
#include <rtems/score/sh_io.h>
#include <rtems/score/sh.h>
#include <rtems/score/ispsh7045.h>
#include <rtems/score/iosh7045.h>

extern uint32_t bsp_clicks_per_second;

#define _MTU_COUNTER0_MICROSECOND (Clock_MHZ/16)

#ifndef CLOCKPRIO
#define CLOCKPRIO 10
#endif

#define MTU0_STARTMASK 	0xfe
#define MTU0_SYNCMASK 	0xfe
#define MTU0_MODEMASK 	0xc0
#define MTU0_TCRMASK 	0x22 /* bit 7 also used, vs 703x */
#define MTU0_STAT_MASK 	0xc0
#define MTU0_IRQMASK 	0xfe
#define MTU0_TIERMASK 	0x01
#define IPRC_MTU0_MASK 	0xff0f
#define MTU0_TIORVAL 	0x08

/*
 *  The interrupt vector number associated with the clock tick device
 *  driver.
 */

#define CLOCK_VECTOR MTUA0_ISP_V

/*
 *  Clock_driver_ticks is a monotonically increasing counter of the
 *  number of clock ticks since the driver was initialized.
 */

volatile uint32_t   Clock_driver_ticks;

static void Clock_exit( void );
static rtems_isr Clock_isr( rtems_vector_number vector );
static uint32_t   Clock_MHZ ;

/*
 *  Clock_isrs is the number of clock ISRs until the next invocation of
 *  the RTEMS clock tick routine.  The clock tick device driver
 *  gets an interrupt once a millisecond and counts down until the
 *  length of time between the user configured microseconds per tick
 *  has passed.
 */

uint32_t   Clock_isrs;              /* ISRs until next tick */
static uint32_t   Clock_isrs_const;        /* only calculated once */

/*
 * These are set by clock driver during its init
 */

rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

/*
 *  The previous ISR on this clock tick interrupt vector.
 */

rtems_isr_entry  Old_ticker;

/*
 *  Isr Handler
 */

rtems_isr Clock_isr(
  rtems_vector_number vector
)
{
  /*
   * bump the number of clock driver ticks since initialization
   *

   * determine if it is time to announce the passing of tick as configured
   * to RTEMS through the rtems_clock_tick directive
   *
   * perform any timer dependent tasks
   */
  uint8_t   temp;

  /* reset the flags of the status register */
  temp = read8( MTU_TSR0) & MTU0_STAT_MASK;
  write8( temp, MTU_TSR0);

  Clock_driver_ticks++ ;

  if( Clock_isrs == 1)
    {
      rtems_clock_tick();
      Clock_isrs = Clock_isrs_const;
    }
  else
    {
      Clock_isrs-- ;
    }
}

/*
 *  Install_clock
 *
 *  Install a clock tick handler and reprograms the chip.  This
 *  is used to initially establish the clock tick.
 */

void Install_clock(
  rtems_isr_entry clock_isr
)
{
  uint8_t   temp8 = 0;
  uint32_t   factor = 1000000;


  /*
   *  Initialize the clock tick device driver variables
   */

  Clock_driver_ticks = 0;
  Clock_isrs_const = rtems_configuration_get_microseconds_per_tick() / 10000;
  Clock_isrs = Clock_isrs_const;

  factor /= rtems_configuration_get_microseconds_per_tick(); /* minimalization of integer division error */
  Clock_MHZ = bsp_clicks_per_second / factor ;

  rtems_interrupt_catch( Clock_isr, CLOCK_VECTOR, &Old_ticker );

  /*
   *  Hardware specific initialize goes here
   */

  /* stop Timer 0 */
  temp8 = read8( MTU_TSTR) & MTU0_STARTMASK;
  write8( temp8, MTU_TSTR);

  /* set initial counter value to 0 */
  write16( 0, MTU_TCNT0);

  /* Timer 0 runs independent */
  temp8 = read8( MTU_TSYR) & MTU0_SYNCMASK;
  write8( temp8, MTU_TSYR);

  /* Timer 0 normal mode */
  temp8 = read8( MTU_TMDR0) & MTU0_MODEMASK;
  write8( temp8, MTU_TMDR0);

  /* TCNT is cleared by GRA ; internal clock /16 */
  write8( MTU0_TCRMASK , MTU_TCR0);

  /* use GRA without I/O - pins  */
  write8( MTU0_TIORVAL, MTU_TIORL0);

  /* reset flags of the status register */
  temp8 = read8( MTU_TSR0) & MTU0_STAT_MASK;
  write8( temp8, MTU_TSR0);

  /* Irq if is equal GRA */
  temp8 = read8( MTU_TIER0) | MTU0_TIERMASK;
  write8( temp8, MTU_TIER0);

  /* set interrupt priority */
  if( sh_set_irq_priority( CLOCK_VECTOR, CLOCKPRIO ) != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred( RTEMS_NOT_CONFIGURED);

  /* set counter limits */
  write16( _MTU_COUNTER0_MICROSECOND, MTU_GR0A);

  /* start counter */
  temp8 = read8( MTU_TSTR) |~MTU0_STARTMASK;
  write8( temp8, MTU_TSTR);

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
  uint8_t   temp8 = 0;

  /* turn off the timer interrupts */
  /* set interrupt priority to 0 */
  if( sh_set_irq_priority( CLOCK_VECTOR, 0 ) != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred( RTEMS_UNSATISFIED);

/*
 *   temp16 = read16( MTU_TIER0) & IPRC_MTU0_IRQMASK;
 *   write16( temp16, MTU_TIER0);
 */

  /* stop counter */
  temp8 = read8( MTU_TSTR) & MTU0_STARTMASK;
  write8( temp8, MTU_TSTR);

  /* old vector shall not be installed */
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
  Install_clock( Clock_isr );

  /*
   * make major/minor avail to others such as shared memory driver
   */

  rtems_clock_major = major;
  rtems_clock_minor = minor;

  return RTEMS_SUCCESSFUL;
}
