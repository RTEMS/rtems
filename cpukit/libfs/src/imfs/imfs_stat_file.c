/**
 * @file
 *
 * @ingroup IMFS
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/imfs.h>

int imfs_memfile_bytes_per_block;

int IMFS_stat_file(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
)
{
  const IMFS_file_t *file = loc->node_access;

  buf->st_size = file->File.size;
  buf->st_blksize = imfs_memfile_bytes_per_block;

  return IMFS_stat( loc, buf );
}
