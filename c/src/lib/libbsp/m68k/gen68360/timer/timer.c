/*
 * Timer_init()
 *
 * Use TIMER 1 and TIMER 2 for Timing Test Suite
 *
 * The hardware on the MC68360 makes these routines very simple.
 *
 * Based on the `gen68302' board support package, and covered by the
 * original distribution terms.
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 *
 *  $Id$
 */

/*
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  NOTE: It is important that the timer start/stop overhead be 
 *        determined when porting or modifying this code.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 */

#include <rtems.h>
#include <bsp.h>
#include "mc68360.h"

void
Timer_initialize (void)
{
	/*
	 * Reset timers 1 and 2
	 */
	m360.tgcr &= ~0x00FF;
	m360.tcn1 = 0;
	m360.tcn2 = 0;
	m360.ter1 = 0xFFFF;
	m360.ter2 = 0xFFFF;

	/*
	 * Cascade timers 1 and 2
	 */
	m360.tgcr |= 0x0080;

	/*
	 * Configure timers 1 and 2 to a single 32-bit, 1 MHz timer.
	 * HARDWARE:
	 *	Change the `25' to match your processor clock
	 */
	m360.tmr2 = ((25-1) << 8) | 0x2;
	m360.tmr1 = 0;

	/*
	 * Start the timers
	 */
	m360.tgcr |=  0x0011;
}

/*
 * Return timer value in microsecond units
 */
int
Read_timer (void)
{
	return *(rtems_unsigned32 *)&m360.tcn1;
}

/*
 * Empty function call used in loops to measure basic cost of looping
 * in Timing Test Suite.
 */
rtems_status_code
Empty_function (void)
{
	return RTEMS_SUCCESSFUL;
}

void
Set_find_average_overhead(rtems_boolean find_flag)
{
}
