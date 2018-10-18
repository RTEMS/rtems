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
 *  Copyright (c) 2012 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
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

  rtems_filesystem_eval_path_start( &ctx, path, eval_flags );

  if ( rtems_filesystem_eval_path_has_token( &ctx ) ) {
    create_regular_file( &ctx, mode );
  }

#ifdef O_DIRECTORY
  open_dir = ( oflag & O_DIRECTORY ) == O_DIRECTORY;
#else
  open_dir = false;
#endif

  if ( write_access || open_dir ) {
    const rtems_filesystem_location_info_t *currentloc =
      rtems_filesystem_eval_path_get_currentloc( &ctx );
    mode_t type = rtems_filesystem_location_type( currentloc );

    if ( write_access && S_ISDIR( type ) ) {
      rtems_filesystem_eval_path_error( &ctx, EISDIR );
    }

    if ( open_dir && !S_ISDIR( type ) ) {
      rtems_filesystem_eval_path_error( &ctx, ENOTDIR );
    }
  }

  rtems_filesystem_eval_path_extract_currentloc( &ctx, &iop->pathinfo );
  rtems_filesystem_eval_path_cleanup( &ctx );

  _Atomic_Store_uint(
    &iop->flags,
    rtems_libio_fcntl_flags( oflag ),
    ATOMIC_ORDER_RELAXED
  );

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
