/*
 *  Stub Base libio initialization
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/libio_.h>               /* libio_.h pulls in rtems */
#include <rtems.h>
#include <rtems/assoc.h>                /* assoc.h not included by rtems.h */

#include <stdio.h>                      /* O_RDONLY, et.al. */
#include <fcntl.h>                      /* O_RDONLY, et.al. */
#include <assert.h>
#include <errno.h>

#include <errno.h>
#include <string.h>                     /* strcmp */
#include <unistd.h>
#include <stdlib.h>                     /* calloc() */

#include <rtems/libio.h>                /* libio.h not pulled in by rtems */

/*
 *  File descriptor Table Information
 */

extern uint32_t    rtems_libio_number_iops;
rtems_id           rtems_libio_semaphore;
rtems_libio_t     *rtems_libio_iops;
rtems_libio_t     *rtems_libio_iop_freelist;

/*
 *  rtems_libio_init
 *
 *  Called by BSP startup code to initialize the libio subsystem.
 */
void rtems_libio_init( void )
{
}
