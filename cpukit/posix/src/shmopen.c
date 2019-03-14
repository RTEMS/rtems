/**
 * @file
 */

/*
 * Copyright (c) 2016 Gedare Bloom.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <rtems/libio_.h>
#include <rtems/seterr.h>

#include <rtems/posix/shmimpl.h>
#include <rtems/score/wkspace.h>
#include <rtems/sysinit.h>

static const rtems_filesystem_file_handlers_r shm_handlers;

static int shm_fstat(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
)
{
  POSIX_Shm_Control *shm = loc_to_shm( loc );

  if ( shm == NULL )
    rtems_set_errno_and_return_minus_one( EIO );

  /* mandatory for shm objects */
  buf->st_uid = shm->uid;
  buf->st_gid = shm->gid;
  buf->st_size = shm->shm_object.size;
  buf->st_mode = shm->mode;

  /* optional for shm objects */
  buf->st_atime = shm->atime;
  buf->st_mtime = shm->mtime;
  buf->st_ctime = shm->ctime;

  return 0;
}

/* read() is unspecified for shared memory objects */
static ssize_t shm_read( rtems_libio_t *iop, void *buffer, size_t count )
{
  ssize_t bytes_read;
  POSIX_Shm_Control *shm = iop_to_shm( iop );

  _Objects_Allocator_lock();
  bytes_read = (*shm->shm_object.ops->object_read)(
      &shm->shm_object,
      buffer,
      count
  );
  _POSIX_Shm_Update_atime( shm );

  _Objects_Allocator_unlock();
  return bytes_read;
}

static int shm_ftruncate( rtems_libio_t *iop, off_t length )
{
  int err;
  POSIX_Shm_Control *shm = iop_to_shm( iop );

  _Objects_Allocator_lock();

  err = (*shm->shm_object.ops->object_resize)( &shm->shm_object, length );

  if ( err != 0 ) {
    _Objects_Allocator_unlock();
    rtems_set_errno_and_return_minus_one( err );
  }

  _POSIX_Shm_Update_mtime_ctime( shm );

  _Objects_Allocator_unlock();
  return 0;
}

static int shm_close( rtems_libio_t *iop )
{
  POSIX_Shm_Control *shm = iop_to_shm( iop );
  int err;

  err = 0;

  POSIX_Shm_Attempt_delete(shm);
  iop->pathinfo.node_access = NULL;

  if ( err != 0 ) {
    rtems_set_errno_and_return_minus_one( err );
  }
  return 0;
}

static int shm_mmap(
  rtems_libio_t *iop,
  void** addr,
  size_t len,
  int prot,
  off_t off
)
{
  POSIX_Shm_Control *shm = iop_to_shm( iop );

  _Objects_Allocator_lock();

  *addr = (*shm->shm_object.ops->object_mmap)( &shm->shm_object, len, prot, off);
  if ( *addr != NULL ) {
    /* Keep a reference in the shared memory to prevent its removal. */
    ++shm->reference_count;

    /* Update atime */
    _POSIX_Shm_Update_atime(shm);
  } else {
    _Objects_Allocator_unlock();
    rtems_set_errno_and_return_minus_one( ENOMEM );
  }

  _Objects_Allocator_unlock();

  return 0;
}

static inline POSIX_Shm_Control *shm_allocate(
  const char *name_arg,
  size_t name_len,
  int oflag,
  mode_t mode,
  int *error
)
{
  POSIX_Shm_Control *shm;
  char *name;
  struct timeval tv;

  /* Reject any name without a leading slash. */
  if ( name_arg[0] != '/' ) {
    *error = EINVAL;
    return NULL;
  }

  /* Only create the object if requested. */
  if ( ( oflag & O_CREAT ) != O_CREAT ) {
    *error = ENOENT;
    return NULL;
  }

  name = _Workspace_String_duplicate( name_arg, name_len );
  if ( name == NULL ) {
    *error = ENOSPC;
    return NULL;
  }

  shm = _POSIX_Shm_Allocate_unprotected();
  if ( shm == NULL ) {
    _Workspace_Free( name );
    *error = ENFILE;
    return NULL;
  }

  gettimeofday( &tv, 0 );

  shm->reference_count = 1;
  shm->shm_object.handle = NULL;
  shm->shm_object.size = 0;
  shm->shm_object.ops = &_POSIX_Shm_Object_operations;
  shm->mode = mode & ~rtems_filesystem_umask;
  shm->oflag = oflag;
  shm->uid = geteuid();
  shm->gid = getegid();
  shm->atime = (time_t) tv.tv_sec;
  shm->mtime = (time_t) tv.tv_sec;
  shm->ctime = (time_t) tv.tv_sec;

  _Objects_Open_string( &_POSIX_Shm_Information, &shm->Object, name );

  return shm;
}

static inline bool shm_access_ok( POSIX_Shm_Control *shm, int oflag )
{
  int flags;

  if ( (oflag & O_ACCMODE) == O_RDONLY ) {
    flags = RTEMS_FS_PERMS_READ;
  } else {
    flags = RTEMS_FS_PERMS_WRITE;
  }
  return rtems_filesystem_check_access( flags, shm->mode, shm->uid, shm->gid );
}

static inline int shm_check_oflag( int oflag )
{
  if ( ( oflag & O_ACCMODE ) != O_RDONLY && ( oflag & O_ACCMODE ) != O_RDWR ) {
    rtems_set_errno_and_return_minus_one( EACCES );
  }

  if ( ( oflag & ~( O_RDONLY | O_RDWR | O_CREAT | O_EXCL | O_TRUNC ) ) != 0 ) {
    rtems_set_errno_and_return_minus_one( EACCES );
  }

  if ( ( oflag & O_TRUNC ) != 0 && ( oflag & O_ACCMODE ) != O_RDWR ) {
    rtems_set_errno_and_return_minus_one( EACCES );
  }
  return 0;
}

int shm_open( const char *name, int oflag, mode_t mode )
{
  int err = 0;
  int fd;
  rtems_libio_t *iop;
  POSIX_Shm_Control *shm;
  size_t len;
  Objects_Get_by_name_error obj_err;
  unsigned int flags;

  if ( shm_check_oflag( oflag ) != 0 ) {
    return -1;
  }

  iop = rtems_libio_allocate();
  if ( iop == NULL ) {
    rtems_set_errno_and_return_minus_one( EMFILE );
  }

  _Objects_Allocator_lock();
  shm = _POSIX_Shm_Get_by_name( name, &len, &obj_err );

  if ( shm == NULL ) {
    switch ( obj_err ) {
      case OBJECTS_GET_BY_NAME_INVALID_NAME:
        err = EINVAL;
        break;

      case OBJECTS_GET_BY_NAME_NAME_TOO_LONG:
        err = ENAMETOOLONG;
        break;

      case OBJECTS_GET_BY_NAME_NO_OBJECT:
      default:
        shm = shm_allocate(name, len, oflag, mode, &err);
        break;
    }
  } else { /* shm exists */
    if ( ( oflag & ( O_EXCL | O_CREAT ) ) == ( O_EXCL | O_CREAT ) ) {
      /* Request to create failed. */
      err = EEXIST;
    } else if ( !shm_access_ok( shm, oflag ) ) {
      err = EACCES;
    } else {
      ++shm->reference_count;
    }
  }
  _Objects_Allocator_unlock();
  if ( err != 0 ) {
    rtems_libio_free( iop );
    rtems_set_errno_and_return_minus_one( err );
  }

  if ( oflag & O_TRUNC ) {
    err = shm_ftruncate( iop, 0 );
    (void) err; /* ignore truncate error */
  }

  fd = rtems_libio_iop_to_descriptor( iop );
  iop->data0 = fd;
  iop->data1 = shm;
  iop->pathinfo.node_access = shm;
  iop->pathinfo.handlers = &shm_handlers;
  iop->pathinfo.mt_entry = &rtems_filesystem_null_mt_entry;
  rtems_filesystem_location_add_to_mt_entry( &iop->pathinfo );

  flags = LIBIO_FLAGS_CLOSE_ON_EXEC;
  if ( (oflag & O_ACCMODE) == O_RDONLY ) {
    flags |= LIBIO_FLAGS_READ;
  } else {
    flags |= LIBIO_FLAGS_READ_WRITE;
  }

  rtems_libio_iop_flags_initialize( iop, flags );

  return fd;
}

static const rtems_filesystem_file_handlers_r shm_handlers = {
  .open_h = rtems_filesystem_default_open,
  .close_h = shm_close,
  .read_h = shm_read,
  .write_h = rtems_filesystem_default_write,
  .ioctl_h = rtems_filesystem_default_ioctl,
  .lseek_h = rtems_filesystem_default_lseek,
  .fstat_h = shm_fstat,
  .ftruncate_h = shm_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .kqfilter_h = rtems_filesystem_default_kqfilter,
  .mmap_h = shm_mmap,
  .poll_h = rtems_filesystem_default_poll,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev
};

static void _POSIX_Shm_Manager_initialization( void )
{
  _Objects_Initialize_information( &_POSIX_Shm_Information );
}

RTEMS_SYSINIT_ITEM(
  _POSIX_Shm_Manager_initialization,
  RTEMS_SYSINIT_POSIX_SHM,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
