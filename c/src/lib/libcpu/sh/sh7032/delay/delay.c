/*  
 *  This routine is a simple spin delay
 *
 *  Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1999, Ralf Corsepius, Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
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


#include <rtems.h>

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 *
 *  Since we don't have a real time clock, this is a very rough
 *  approximation, assuming that each cycle of the delay loop takes
 *  approx. 4 machine cycles.
 *
 *  e.g.: clicks_per_second = 20MHz
 *        => 5e-8 secs per instruction
 *        => 4 * 5e-8 secs per delay loop
 */

void CPU_delay( unsigned32 microseconds )
{ 
  register unsigned32 clicks_per_usec = 
    rtems_cpu_configuration_get_clicks_per_second() / 1000000 ;
  register unsigned32 _delay = 
    (microseconds) * (clicks_per_usec);
  asm volatile (
"0:	add  #-4,%0\n
	nop\n
	cmp/pl %0\n
	bt 0b
	nop"
     :: "r" (_delay) );
}
