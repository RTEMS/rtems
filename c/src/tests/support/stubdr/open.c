/*  Stub_open
 *
 *  This routine is the terminal driver open routine.
 *
 *  Input parameters:
 *    major - device major number
 *    minor - device minor number
 *    pargb - pointer to open parameter block
 *
 *  Output parameters:
 *    rval       - STUB_SUCCESSFUL
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
#include "stubdrv.h"

rtems_device_driver Stub_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  return STUB_SUCCESSFUL;
}
