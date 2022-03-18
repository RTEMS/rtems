/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_rfs
 *
 * @brief RTEMS File Systems Buffer Routines
 */

/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
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
