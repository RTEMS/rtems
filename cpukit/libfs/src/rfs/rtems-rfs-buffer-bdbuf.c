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
 * RTEMS File Systems Buffer Routines for the RTEMS libblock BD buffer cache.
 *
 */

#if HAVE_CONFIG_H
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
