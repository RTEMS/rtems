/*  Shm_setclockvec
 *
 *  This routines installs the shared memory clock interrupt handler
 *  used when the driver is used in polling mode.
 *
 *  INPUT PARAMETERS:  NONE
 *
 *  OUTPUT PARAMETERS: NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>
#include <rtems/libio.h>

#include "shm_driver.h"

rtems_isr Shm_setclockvec()
{
    rtems_libio_ioctl_args_t args;
    args.iop = 0;
    args.command = rtems_build_name('N', 'E', 'W', ' ');
    args.buffer = (void *) Shm_Poll;

    (void) rtems_io_control(rtems_clock_major, rtems_clock_minor, &args);
}
