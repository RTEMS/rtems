/*  Stub_write
 *
 *  This routine is the terminal driver write routine.
 *
 *  Input parameters:
 *    major - device major number
 *    minor - device minor number
 *    pargp - pointer to write parameter block
 *
 *  Output parameters:
 *    rval       - STUB_SUCCESSFUL
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems.h>
#include "stubdrv.h"

rtems_device_driver Stub_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  return STUB_SUCCESSFUL;
}
