/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_rfs
 *
 * @brief RTEMS File Systems Buffer Routines
 *
 * RTEMS File Systems Buffer Routines for the RTEMS libblock BD buffer cache.
 *
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

#include <inttypes.h>
#include <errno.h>

#include <rtems/rfs/rtems-rfs-buffer.h>
#include <rtems/rfs/rtems-rfs-file-system.h>

#if RTEMS_RFS_USE_LIBBLOCK

/**
 * Show errors.
 */
#define RTEMS_RFS_BUFFER_ERRORS 0

int
rtems_rfs_buffer_bdbuf_request (rtems_rfs_file_system*   fs,
                                rtems_rfs_buffer_block   block,
                                bool                     read,
                                rtems_rfs_buffer**       buffer)
{
  rtems_status_code sc;
  int               rc = 0;

  if (read)
    sc = rtems_bdbuf_read (rtems_rfs_fs_device (fs), block, buffer);
  else
    sc = rtems_bdbuf_get (rtems_rfs_fs_device (fs), block, buffer);

  if (sc != RTEMS_SUCCESSFUL)
  {
#if RTEMS_RFS_BUFFER_ERRORS
    printf ("rtems-rfs: buffer-bdbuf-request: block=%lu: bdbuf-%s: %d: %s\n",
            block, read ? "read" : "get", sc, rtems_status_text (sc));
#endif
    rc = EIO;
  }

  return rc;
}

int
rtems_rfs_buffer_bdbuf_release (rtems_rfs_buffer* buffer,
                                bool              modified)
{
  rtems_status_code sc;
  int               rc = 0;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_RELEASE))
    printf ("rtems-rfs: bdbuf-release: block=%" PRIuPTR " bdbuf=%" PRIu32 " %s\n",
            ((intptr_t) buffer->user),
            buffer->block, modified ? "(modified)" : "");

  if (modified)
    sc = rtems_bdbuf_release_modified (buffer);
  else
    sc = rtems_bdbuf_release (buffer);

  if (sc != RTEMS_SUCCESSFUL)
  {
#if RTEMS_RFS_BUFFER_ERRORS
    printf ("rtems-rfs: buffer-release: bdbuf-%s: %s(%d)\n",
            modified ? "modified" : "not-modified",
            rtems_status_text (sc), sc);
#endif
    rc = EIO;
  }

  return rc;
}

#endif
