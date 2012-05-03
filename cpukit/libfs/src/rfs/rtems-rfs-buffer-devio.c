/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems-rfs
 *
 * RTEMS File Systems Buffer Routines.
 *
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>

#include <rtems/rfs/rtems-rfs-buffer.h>
#include <rtems/rfs/rtems-rfs-file-system.h>

#if !RTEMS_RFS_USE_LIBBLOCK

/**
 * Show errors.
 */
#define RTEMS_RFS_BUFFER_ERRORS 1

int
rtems_rfs_buffer_deviceio_request (rtems_rfs_buffer_handle* handle,
                                   dev_t                    device,
                                   rtems_rfs_buffer_block   block,
                                   bool                     read)
{
}

int
rtems_rfs_buffer_deviceio_release (rtems_rfs_buffer_handle* handle,
                                   dev_t                    device)
{
}

int
rtems_rfs_buffer_deviceio_handle_open (rtems_rfs_buffer_handle* handle,
                                       dev_t                    device)
{
}

int
rtems_rfs_buffer_device_handle_close (rtems_rfs_buffer_handle* handle,
                                      dev_t                    device)
{
}

#endif
