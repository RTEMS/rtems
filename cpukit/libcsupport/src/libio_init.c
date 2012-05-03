/*
 *  This file contains the support infrastructure used to manage the
 *  table of integer style file descriptors used by the low level
 *  POSIX system calls like open(), read, fstat(), etc.
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/libio_.h>               /* libio_.h pulls in rtems */
#include <rtems.h>
#include <rtems/assoc.h>                /* assoc.h not included by rtems.h */

#include <stdio.h>                      /* O_RDONLY, et.al. */
#include <fcntl.h>                      /* O_RDONLY, et.al. */
#include <errno.h>

#include <errno.h>
#include <string.h>                     /* strcmp */
#include <unistd.h>
#include <stdlib.h>                     /* calloc() */

#include <rtems/libio.h>                /* libio.h not pulled in by rtems */

/*
 *  File descriptor Table Information
 */

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
    rtems_status_code rc;
    uint32_t i;
    rtems_libio_t *iop;

    if (rtems_libio_number_iops > 0)
    {
        rtems_libio_iops = (rtems_libio_t *) calloc(rtems_libio_number_iops,
                                                    sizeof(rtems_libio_t));
        if (rtems_libio_iops == NULL)
            rtems_fatal_error_occurred(RTEMS_NO_MEMORY);

        iop = rtems_libio_iop_freelist = rtems_libio_iops;
        for (i = 0 ; (i + 1) < rtems_libio_number_iops ; i++, iop++)
          iop->data1 = iop + 1;
        iop->data1 = NULL;
    }

  /*
   *  Create the binary semaphore used to provide mutual exclusion
   *  on the IOP Table.
   */

  rc = rtems_semaphore_create(
    RTEMS_LIBIO_SEM,
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY | RTEMS_PRIORITY,
    RTEMS_NO_PRIORITY,
    &rtems_libio_semaphore
  );
  if ( rc != RTEMS_SUCCESSFUL )
    rtems_fatal_error_occurred( rc );

  /*
   *  Initialize the base file system infrastructure.
   */

  if (rtems_fs_init_helper)
     (* rtems_fs_init_helper)();
}
