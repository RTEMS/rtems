/*  Clock_init()
 *
 *  This routine initializes the timer on the VIC chip on the CVME961.
 *  The tick frequency is 1 millisecond.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <stdlib.h>

#include <rtems.h>
#include <bsp.h>
#include <clockdrv.h>

rtems_unsigned32 Clock_isrs;              /* ISRs until next tick */
i960_isr         Old_ticker;
volatile rtems_unsigned32 Clock_driver_ticks;
                                          /* ticks since initialization */

rtems_device_driver Clock_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp,
  rtems_id id,
  rtems_unsigned32 *rval )
{
  Install_clock( Clock_isr );
  atexit( Clock_exit );
}

void ReInstall_clock(
  rtems_isr_entry clock_isr
)
{
   (void) set_vector( clock_isr, 5, 1 );
}

void Install_clock(
  rtems_isr_entry clock_isr
)
{
  volatile unsigned char *victimer;

  Clock_driver_ticks = 0;
  Clock_isrs = BSP_Configuration.microseconds_per_tick / 1000;

  if ( BSP_Configuration.ticks_per_timeslice ) {
    Old_ticker = set_vector( clock_isr, 5, 1 );
    victimer = (volatile unsigned char *) 0xa00000c3;
    *victimer = 0x12;
    *victimer = 0x92;      /* 1000 HZ */
  }
}

void Clock_exit()
{
  unsigned char *victimer;

  if ( BSP_Configuration.ticks_per_timeslice ) {
    victimer = (unsigned char *) 0xa00000c3;
    *victimer = 0x12;
    i960_mask_intr( 5 );
    /* do not restore old vector */
  }
}
