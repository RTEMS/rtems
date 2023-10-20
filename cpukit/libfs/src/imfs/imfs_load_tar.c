/* SPDX-License-Identifier: BSD-2-Clause */

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

#include <rtems/imfs.h>
#include <rtems/untar.h>

#include <string.h>

int rtems_tarfs_load(
  const char *mountpoint,
  const void *tar_image,
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
  const uint8_t                        *image;

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
  image = tar_image;

  while ( ptr + 512 <= tar_size ) {
    int retval;

    retval = Untar_ProcessHeader( &ctx, (const char *) &image[ ptr ] );
    if ( retval != UNTAR_SUCCESSFUL ) {
      return -1;
    }

    ptr += 512;

    if ( ctx.linkflag == REGTYPE ) {
      retval = IMFS_make_linearfile(
        ctx.file_path,
        ctx.mode,
        &image[ ptr ],
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
