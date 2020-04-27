/**
 * @file
 *
 * @ingroup IMFS
 *
 * @brief RTEMS Load Tarfs
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/imfs.h>
#include <rtems/untar.h>

#include <string.h>

int rtems_tarfs_load(
  const char *mountpoint,
  uint8_t    *tar_image,
  size_t      tar_size
)
{
  rtems_filesystem_eval_path_context_t  eval_ctx;
  int                                   eval_flags;
  rtems_filesystem_location_info_t     *loc;
  bool                                  is_imfs;
  char                                  buf[ 156 + UNTAR_FILE_NAME_SIZE ];
  size_t                                len;
  Untar_HeaderContext                   ctx;
  unsigned long                         ptr;

  len = strlen( mountpoint );
  if ( len >= sizeof( buf ) - UNTAR_FILE_NAME_SIZE - 2 ) {
    return -1;
  }

  eval_flags = RTEMS_FS_FOLLOW_LINK;
  loc = rtems_filesystem_eval_path_start( &eval_ctx, mountpoint, eval_flags );
  is_imfs = IMFS_is_imfs_instance( loc );
  rtems_filesystem_eval_path_cleanup( &eval_ctx );
  if ( !is_imfs ) {
    return -1;
  }

  ctx.printer = NULL;
  ctx.file_path = memcpy( buf, mountpoint, len );

  if ( len > 0 && ctx.file_path[ len - 1 ] != '/') {
    ctx.file_path[ len ] = '/';
    ctx.file_name = ctx.file_path + len + 1;
  } else {
    ctx.file_name = ctx.file_path + len;
  }

  ptr = 0;

  while ( ptr + 512 <= tar_size ) {
    int retval;

    retval = Untar_ProcessHeader( &ctx, (const char *) &tar_image[ ptr ] );
    if ( retval != UNTAR_SUCCESSFUL ) {
      return -1;
    }

    ptr += 512;

    if ( ctx.linkflag == REGTYPE ) {
      retval = IMFS_make_linearfile(
        ctx.file_path,
        ctx.mode,
        &tar_image[ ptr ],
        ctx.file_size
      );
      if ( retval != 0 ) {
        return -1;
      }

      ptr += 512 * ctx.nblocks;
    }
  }

  return 0;
}
