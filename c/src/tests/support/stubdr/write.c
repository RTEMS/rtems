/*  Stub_write
 *
 *  This routine is the terminal driver write routine.
 *
 *  Input parameters:
 *    major - device major number
 *    minor - device minor number
 *    pargp - pointer to write parameter block
 *    tid   - calling task's id
 *    rval  - address of the return value
 *
 *  Output parameters:
 *    rval       - STUB_SUCCESSFUL
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

#include <rtems.h>
#include "stubdrv.h"

rtems_device_driver Stub_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp,
  rtems_id tid,
  rtems_unsigned32 *rval
)
{
  *rval = STUB_SUCCESSFUL;
}
