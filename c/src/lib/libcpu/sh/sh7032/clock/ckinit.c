/*
 *  This file contains the clock driver the Hitachi SH 703X
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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>

#include <stdlib.h>

#include <rtems/libio.h>
#include <rtems/score/sh_io.h>
#include <rtems/score/sh.h>
#include <rtems/score/ispsh7032.h>
#include <rtems/score/iosh7032.h>

extern uint32_t bsp_clicks_per_second;

#ifndef CLOCKPRIO
#define CLOCKPRIO 10
#endif

#define I_CLK_PHI_1	0
#define I_CLK_PHI_2	1
#define I_CLK_PHI_4	2
#define I_CLK_PHI_8	3

/*
 * Set I_CLK_PHI to one of the I_CLK_PHI_X values from above to choose
 * a PHI/X clock rate.
 */

#define I_CLK_PHI 	I_CLK_PHI_4
#define CLOCK_SCALE	(1<<I_CLK_PHI)

#define ITU0_STARTMASK 	0xfe
#define ITU0_SYNCMASK 	0xfe
#define ITU0_MODEMASK 	0xfe
#define ITU0_TCRMASK 	(0x20 | I_CLK_PHI)
#define ITU_STAT_MASK 	0xf8
#define ITU0_IRQMASK 	0xfe
#define ITU0_TIERMASK 	0x01
#define IPRC_ITU0_MASK 	0xff0f
#define ITU0_TIORVAL 	0x08

/*
 * clicks_per_tick := clicks_per_sec * usec_per_tick
 *
 * This is a very expensive function ;-)
 *
 * Below are two variants:
 * 1. A variant applying integer arithmetics, only.
 * 2. A variant applying floating point arithmetics
 *
 * The floating point variant pulls in the fmath routines when linking,
 * resulting in slightly larger executables for applications that do not
 * apply fmath otherwise. However, the imath variant is significantly slower
 * than the fmath variant and more complex.
 *
 * Assuming that most applications will not use fmath, but are critical
 * in memory size constraints, we apply the integer variant.
 *
 * To the sake of simplicity, we might abandon one of both variants in
 * future.
 */
static unsigned int sh_clicks_per_tick(
  unsigned int clicks_per_sec,
  unsigned int usec_per_tick
)
{
#if 1
  unsigned int clicks_per_tick = 0 ;

  unsigned int b = clicks_per_sec ;
  unsigned int c = 1000000 ;
  unsigned int d = 1 ;
  unsigned int a = ( ( b / c ) * usec_per_tick ) / d;

  clicks_per_tick += a ;

  while ( ( b %= c ) > 0 )
  {
    c /= 10 ;
    d *= 10 ;
    a = ( ( b / c ) * usec_per_tick ) / d ;
    clicks_per_tick += a ;
  }
  return clicks_per_tick ;
#else
  double fclicks_per_tick =
    ((double) clicks_per_sec * (double) usec_per_tick) / 1000000.0 ;
  return (uint32_t) fclicks_per_tick ;
#endif
}

/*
 *  The interrupt vector number associated with the clock tick device
 *  driver.
 */

#define CLOCK_VECTOR IMIA0_ISP_V

/*
 *  Clock_driver_ticks is a monotonically increasing counter of the
 *  number of clock ticks since the driver was initialized.
 */

volatile uint32_t   Clock_driver_ticks;

static void Clock_exit( void );
static rtems_isr Clock_isr( rtems_vector_number vector );

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
  temp = read8( ITU_TSR0) & ITU_STAT_MASK;
  write8( temp, ITU_TSR0);

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
  uint32_t   microseconds_per_tick;
  uint32_t   cclicks_per_tick;
  uint16_t   Clock_limit;

  /*
   *  Initialize the clock tick device driver variables
   */

  Clock_driver_ticks = 0;

  if ( rtems_configuration_get_microseconds_per_tick() != 0 )
    microseconds_per_tick = rtems_configuration_get_microseconds_per_tick() ;
  else
    microseconds_per_tick = 10000 ; /* 10000 us */

  /* clock clicks per tick */
  cclicks_per_tick = sh_clicks_per_tick(
     bsp_clicks_per_second / CLOCK_SCALE, microseconds_per_tick );

  Clock_isrs_const = cclicks_per_tick >> 16 ;
  if ( ( cclicks_per_tick | 0xffff ) > 0 )
    Clock_isrs_const++ ;
  Clock_limit = cclicks_per_tick / Clock_isrs_const ;
  Clock_isrs = Clock_isrs_const;

  rtems_interrupt_catch( Clock_isr, CLOCK_VECTOR, &Old_ticker );
  /*
   *  Hardware specific initialize goes here
   */

  /* stop Timer 0 */
  temp8 = read8( ITU_TSTR) & ITU0_STARTMASK;
  write8( temp8, ITU_TSTR);

  /* set initial counter value to 0 */
  write16( 0, ITU_TCNT0);

  /* Timer 0 runs independent */
  temp8 = read8( ITU_TSNC) & ITU0_SYNCMASK;
  write8( temp8, ITU_TSNC);

  /* Timer 0 normal mode */
  temp8 = read8( ITU_TMDR) & ITU0_MODEMASK;
  write8( temp8, ITU_TMDR);

  /* TCNT is cleared by GRA ; internal clock /4 */
  write8( ITU0_TCRMASK , ITU_TCR0);

  /* use GRA without I/O - pins  */
  write8( ITU0_TIORVAL, ITU_TIOR0);

  /* reset flags of the status register */
  temp8 = read8( ITU_TSR0) & ITU_STAT_MASK;
  write8( temp8, ITU_TSR0);

  /* Irq if is equal GRA */
  temp8 = read8( ITU_TIER0) | ITU0_TIERMASK;
  write8( temp8, ITU_TIER0);

  /* set interrupt priority */
  if( sh_set_irq_priority( CLOCK_VECTOR, CLOCKPRIO ) != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred( RTEMS_NOT_CONFIGURED);

  /* set counter limits */
  write16( Clock_limit, ITU_GRA0);

  /* start counter */
  temp8 = read8( ITU_TSTR) |~ITU0_STARTMASK;
  write8( temp8, ITU_TSTR);

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
 *   temp16 = read16( ITU_TIER0) & IPRC_ITU0_IRQMASK;
 *   write16( temp16, ITU_TIER0);
 */

  /* stop counter */
  temp8 = read8( ITU_TSTR) & ITU0_STARTMASK;
  write8( temp8, ITU_TSTR);

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
