/*  null_initialize
 *
 *  This routine is the null device driver init routine.
 *
 *  Derived from rtems' stub driver.
 *
 *  Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 *
 *  Input parameters:
 *    major - device major number
 *    minor - device minor number
 *    pargp - pointer to parameter block
 *
 *  Output parameters:
 *    rval       - NULL_SUCCESSFUL
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
#include <sh/null.h>

rtems_unsigned32 NULL_major;

rtems_device_driver null_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  rtems_device_driver status ;
  
  status = rtems_io_register_name(
    "/dev/null",
    major,
    (rtems_device_minor_number) 0
    );

  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);
      
  NULL_major = major;
  
  return RTEMS_SUCCESSFUL;
}
