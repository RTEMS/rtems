/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Open a File
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH & Co. KG
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

#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>

#include <rtems/libio_.h>

static void create_regular_file(
  rtems_filesystem_eval_path_context_t *ctx,
  mode_t mode
)
{
  int rv = 0;
  const rtems_filesystem_location_info_t *currentloc =
    rtems_filesystem_eval_path_get_currentloc( ctx );
  const char *token = rtems_filesystem_eval_path_get_token( ctx );
  size_t tokenlen = rtems_filesystem_eval_path_get_tokenlen( ctx );

  rv = rtems_filesystem_mknod(
    currentloc,
    token,
    tokenlen,
    S_IFREG | mode,
    0
  );

  if ( rv == 0 ) {
    /* The mode only applies to future accesses of the newly created file */
    rtems_filesystem_eval_path_set_flags( ctx, 0 );

    rtems_filesystem_eval_path_set_path( ctx, token, tokenlen );
    rtems_filesystem_eval_path_continue( ctx );
  } else {
    rtems_filesystem_eval_path_error( ctx, 0 );
  }
}

static int do_open(
  rtems_libio_t *iop,
  const char *path,
  int oflag,
  mode_t mode
)
{
  int rv = 0;
  int fd = rtems_libio_iop_to_descriptor( iop );
  int rwflag = oflag + 1;
  bool read_access = (rwflag & _FREAD) == _FREAD;
  bool write_access = (rwflag & _FWRITE) == _FWRITE;
  bool make = (oflag & O_CREAT) == O_CREAT;
  bool exclusive = (oflag & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL);
  bool truncate = (oflag & O_TRUNC) == O_TRUNC;
  bool open_dir;
#ifdef O_NOFOLLOW
  int follow = (oflag & O_NOFOLLOW) == O_NOFOLLOW ? 0 : RTEMS_FS_FOLLOW_LINK;
#else
  int follow = RTEMS_FS_FOLLOW_LINK;
#endif
  int eval_flags = follow
    | (read_access ? RTEMS_FS_PERMS_READ : 0)
    | (write_access ? RTEMS_FS_PERMS_WRITE : 0)
    | (make ? RTEMS_FS_MAKE : 0)
    | (exclusive ?  RTEMS_FS_EXCLUSIVE : 0);
  rtems_filesystem_eval_path_context_t ctx;
  const rtems_filesystem_location_info_t *currentloc;
  bool create_reg_file;

  rtems_filesystem_eval_path_start( &ctx, path, eval_flags );

  currentloc = rtems_filesystem_eval_path_get_currentloc( &ctx );
  create_reg_file = !currentloc->mt_entry->no_regular_file_mknod;

  if ( create_reg_file && rtems_filesystem_eval_path_has_token( &ctx ) ) {
    create_regular_file( &ctx, mode );
  }

#ifdef O_DIRECTORY
  open_dir = ( oflag & O_DIRECTORY ) == O_DIRECTORY;
#else
  open_dir = false;
#endif

  if ( write_access || open_dir ) {
    mode_t type = rtems_filesystem_location_type( currentloc );

    if ( create_reg_file && write_access && S_ISDIR( type ) ) {
      rtems_filesystem_eval_path_error( &ctx, EISDIR );
    }

    if ( open_dir && !S_ISDIR( type ) ) {
      rtems_filesystem_eval_path_error( &ctx, ENOTDIR );
    }
  }

  rtems_filesystem_eval_path_extract_currentloc( &ctx, &iop->pathinfo );
  rtems_filesystem_eval_path_cleanup( &ctx );

  rtems_libio_iop_flags_set( iop, rtems_libio_fcntl_flags( oflag ) );

  rv = (*iop->pathinfo.handlers->open_h)( iop, path, oflag, mode );

  if ( rv == 0 ) {
    rtems_libio_iop_flags_set( iop, LIBIO_FLAGS_OPEN );

    if ( truncate ) {
      rv = ftruncate( fd, 0 );
      if ( rv != 0 ) {
        (*iop->pathinfo.handlers->close_h)( iop );
      }
    }

    if ( rv == 0 ) {
      rv = fd;
    } else {
      rv = -1;
    }
  }

  if ( rv < 0 ) {
    rtems_libio_free( iop );
  }

  return rv;
}

/**
*  POSIX 1003.1 5.3.1 - Open a File
*/
int open( const char *path, int oflag, ... )
{
  int rv = 0;
  va_list ap;
  mode_t mode = 0;
  rtems_libio_t *iop = NULL;

  va_start( ap, oflag );

  mode = va_arg( ap, mode_t );

  iop = rtems_libio_allocate();
  if ( iop != NULL ) {
    rv = do_open( iop, path, oflag, mode );
  } else {
    errno = ENFILE;
    rv = -1;
  }

  va_end( ap );

  return rv;
}



#if defined(RTEMS_NEWLIB) && !defined(HAVE__OPEN_R)

#include <reent.h>

/**
 *  This is the Newlib dependent reentrant version of open().
 */
int _open_r(
  struct _reent *ptr RTEMS_UNUSED,
  const char    *buf,
  int            oflag,
  int            mode
)
{
  return open( buf, oflag, mode );
}
#endif
