/*
 *  Reserve resources for console driver.
 *
 *  This is in a separate file to minimize the amount of baggage
 *  pulled in when not using the console device driver.
 *
 *  Author:
 *    W. Eric Norum
 *    Saskatchewan Accelerator Laboratory
 *    University of Saskatchewan
 *    Saskatoon, Saskatchewan, CANADA
 *    eric@skatter.usask.ca
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <termios.h>
#include <bsp.h>
#include <rtems/libio.h>
#include "m68360.h"

/*
 * Reserve resources consumed by this driver
 */
void console_reserve_resources(
  rtems_configuration_table *configuration
)
{
	rtems_termios_reserve_resources (configuration, 1);
}
