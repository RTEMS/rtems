/*  Shm_setclockvec
 *
 *  This routines installs the shared memory clock interrupt handler
 *  used when the driver is used in polling mode.
 *
 *  INPUT PARAMETERS:  NONE
 *
 *  OUTPUT PARAMETERS: NONE
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
#include <rtems/libio.h>

#include "shm.h"

rtems_isr Shm_setclockvec()
{
    rtems_libio_ioctl_args_t args;
    args.iop = 0;
    args.command = rtems_build_name('N', 'E', 'W', ' ');
    args.buffer = (void *) Shm_Poll;

    (void) rtems_io_control(rtems_clock_major, rtems_clock_minor, &args);
}
