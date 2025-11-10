/**
 * @file
 *
 * @ingroup FatFS
 *
 * @brief RTEMS FATFS file operations
 */

/*
 Copyright (C) 2025 Sepehr Ganji <sepehrganji79@gmail.com>
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>

#include "ff.h"
#include "rtems-fatfs.h"

ssize_t rtems_fatfs_file_read( rtems_libio_t *iop, void *buffer, size_t count )
{
  rtems_fatfs_node_t *node = (rtems_fatfs_node_t *) iop->pathinfo.node_access;
  FRESULT             fr;
  UINT                bytes_read = 0;

  if ( node == NULL || node->is_directory ) {
    rtems_set_errno_and_return_minus_one( EBADF );
  }

  rtems_fatfs_lock( iop->pathinfo.mt_entry );

  fr = f_lseek( &node->handle.file, (FSIZE_t) iop->offset );
  if ( fr != FR_OK ) {
    rtems_fatfs_unlock( iop->pathinfo.mt_entry );
    errno = rtems_fatfs_fresult_to_errno( fr );
    return -1;
  }

  fr = f_read( &node->handle.file, buffer, (UINT) count, &bytes_read );

  rtems_fatfs_unlock( iop->pathinfo.mt_entry );

  if ( fr != FR_OK ) {
    errno = rtems_fatfs_fresult_to_errno( fr );
    return -1;
  }

  if ( bytes_read > 0 ) {
    iop->offset += bytes_read;
  }

  return (ssize_t) bytes_read;
}

ssize_t rtems_fatfs_file_write(
  rtems_libio_t *iop,
  const void    *buffer,
  size_t         count
)
{
  rtems_fatfs_node_t *node = (rtems_fatfs_node_t *) iop->pathinfo.node_access;
  FRESULT             fr;
  UINT                bytes_written = 0;

  if ( node == NULL || node->is_directory ) {
    rtems_set_errno_and_return_minus_one( EBADF );
  }

  rtems_fatfs_lock( iop->pathinfo.mt_entry );

  if ( rtems_libio_iop_is_append( iop ) ) {
    fr = f_lseek( &node->handle.file, f_size( &node->handle.file ) );
    if ( fr != FR_OK ) {
      rtems_fatfs_unlock( iop->pathinfo.mt_entry );
      errno = rtems_fatfs_fresult_to_errno( fr );
      return -1;
    }
    iop->offset = f_tell( &node->handle.file );
  } else {
    FSIZE_t current_size = f_size( &node->handle.file );
    FSIZE_t target_pos   = (FSIZE_t) iop->offset;

    if ( target_pos > current_size ) {
      FSIZE_t gap_size = target_pos - current_size;
      char    zero_buf[ 512 ];
      UINT    bw;

      memset( zero_buf, 0, sizeof( zero_buf ) );

      fr = f_lseek( &node->handle.file, current_size );
      if ( fr != FR_OK ) {
        rtems_fatfs_unlock( iop->pathinfo.mt_entry );
        errno = rtems_fatfs_fresult_to_errno( fr );
        return -1;
      }

      while ( gap_size > 0 && fr == FR_OK ) {
        UINT to_write = gap_size > sizeof( zero_buf ) ? sizeof( zero_buf ) :
                                                        (UINT) gap_size;
        fr            = f_write( &node->handle.file, zero_buf, to_write, &bw );
        if ( fr == FR_OK && bw == to_write ) {
          gap_size -= to_write;
        } else {
          break;
        }
      }

      if ( fr != FR_OK ) {
        rtems_fatfs_unlock( iop->pathinfo.mt_entry );
        errno = rtems_fatfs_fresult_to_errno( fr );
        return -1;
      }

      if ( gap_size > 0 ) {
        rtems_fatfs_unlock( iop->pathinfo.mt_entry );
        errno = ENOSPC;
        return -1;
      }
    } else {
      fr = f_lseek( &node->handle.file, target_pos );
      if ( fr != FR_OK ) {
        rtems_fatfs_unlock( iop->pathinfo.mt_entry );
        errno = rtems_fatfs_fresult_to_errno( fr );
        return -1;
      }
    }
  }

  fr = f_write( &node->handle.file, buffer, (UINT) count, &bytes_written );

  rtems_fatfs_unlock( iop->pathinfo.mt_entry );

  if ( fr != FR_OK ) {
    errno = rtems_fatfs_fresult_to_errno( fr );
    return -1;
  }

  if ( bytes_written > 0 ) {
    iop->offset      += bytes_written;
    node->info.fsize  = f_size( &node->handle.file );

    time_t current_time    = time( NULL );
    node->posix_mtime      = current_time;
    node->posix_ctime      = current_time;
    node->posix_time_valid = true;
  } else if ( count > 0 && bytes_written == 0 ) {
    errno = ENOSPC;
    return -1;
  }

  return (ssize_t) bytes_written;
}

off_t rtems_fatfs_file_lseek( rtems_libio_t *iop, off_t offset, int whence )
{
  rtems_fatfs_node_t *node = (rtems_fatfs_node_t *) iop->pathinfo.node_access;
  FSIZE_t             new_pos;
  off_t               result;

  if ( node == NULL || node->is_directory ) {
    rtems_set_errno_and_return_minus_one( EBADF );
  }

  switch ( whence ) {
    case SEEK_SET:
      if ( offset < 0 ) {
        rtems_set_errno_and_return_minus_one( EINVAL );
      }
      new_pos = (FSIZE_t) offset;
      break;
    case SEEK_CUR:
      if ( offset > 0 && iop->offset > LLONG_MAX - offset ) {
        rtems_set_errno_and_return_minus_one( EOVERFLOW );
      }
      result = iop->offset + offset;
      if ( result < 0 ) {
        rtems_set_errno_and_return_minus_one( EINVAL );
      }
      new_pos = (FSIZE_t) result;
      break;
    case SEEK_END:
      if ( offset > 0 && (off_t) node->info.fsize > LLONG_MAX - offset ) {
        rtems_set_errno_and_return_minus_one( EOVERFLOW );
      }
      result = (off_t) node->info.fsize + offset;
      if ( result < 0 ) {
        rtems_set_errno_and_return_minus_one( EINVAL );
      }
      new_pos = (FSIZE_t) result;
      break;
    default:
      rtems_set_errno_and_return_minus_one( EINVAL );
  }

  iop->offset = (off_t) new_pos;
  return iop->offset;
}

int rtems_fatfs_file_fstat(
  const rtems_filesystem_location_info_t *loc,
  struct stat                            *buf
)
{
  rtems_fatfs_node_t *node = (rtems_fatfs_node_t *) loc->node_access;

  if ( node == NULL ) {
    rtems_set_errno_and_return_minus_one( EBADF );
  }

  rtems_fatfs_fs_info_t *fs_info = (rtems_fatfs_fs_info_t *)
                                     loc->mt_entry->fs_info;
  rtems_fatfs_node_to_stat( node, buf, fs_info );
  return 0;
}

int rtems_fatfs_file_ftruncate( rtems_libio_t *iop, off_t length )
{
  rtems_fatfs_node_t *node = (rtems_fatfs_node_t *) iop->pathinfo.node_access;
  FRESULT             fr;
  FSIZE_t             old_size;

  if ( node == NULL || node->is_directory ) {
    rtems_set_errno_and_return_minus_one( EBADF );
  }

  rtems_fatfs_lock( iop->pathinfo.mt_entry );

  old_size = f_size( &node->handle.file );

  if ( (FSIZE_t) length > old_size ) {
    FSIZE_t current_pos = f_tell( &node->handle.file );
    FSIZE_t gap_size    = (FSIZE_t) length - old_size;
    char    zero_buf[ 512 ];
    UINT    bw;

    memset( zero_buf, 0, sizeof( zero_buf ) );

    fr = f_lseek( &node->handle.file, old_size );
    if ( fr != FR_OK ) {
      rtems_fatfs_unlock( iop->pathinfo.mt_entry );
      errno = rtems_fatfs_fresult_to_errno( fr );
      return -1;
    }

    while ( gap_size > 0 && fr == FR_OK ) {
      UINT to_write = gap_size > sizeof( zero_buf ) ? sizeof( zero_buf ) :
                                                      (UINT) gap_size;
      fr            = f_write( &node->handle.file, zero_buf, to_write, &bw );
      if ( fr == FR_OK && bw == to_write ) {
        gap_size -= to_write;
      } else {
        break;
      }
    }

    if ( fr == FR_OK ) {
      if ( gap_size > 0 ) {
        rtems_fatfs_unlock( iop->pathinfo.mt_entry );
        errno = ENOSPC;
        return -1;
      }
      fr = f_lseek( &node->handle.file, current_pos );
    }
  } else {
    fr = f_lseek( &node->handle.file, (FSIZE_t) length );
    if ( fr == FR_OK ) {
      fr = f_truncate( &node->handle.file );
    }
  }

  rtems_fatfs_unlock( iop->pathinfo.mt_entry );

  if ( fr != FR_OK ) {
    errno = rtems_fatfs_fresult_to_errno( fr );
    return -1;
  }

  node->info.fsize = (FSIZE_t) length;

  return 0;
}

int rtems_fatfs_file_fsync( rtems_libio_t *iop )
{
  rtems_fatfs_node_t *node = (rtems_fatfs_node_t *) iop->pathinfo.node_access;
  FRESULT             fr;

  if ( node == NULL || node->is_directory || !node->is_open ) {
    return 0;
  }

  rtems_fatfs_lock( iop->pathinfo.mt_entry );
  fr = f_sync( &node->handle.file );
  rtems_fatfs_unlock( iop->pathinfo.mt_entry );

  if ( fr != FR_OK ) {
    errno = rtems_fatfs_fresult_to_errno( fr );
    return -1;
  }

  return 0;
}

int rtems_fatfs_openfile(
  rtems_libio_t *iop,
  const char    *path,
  int            oflag,
  mode_t         mode
)
{
  (void) path;
  (void) mode;

  rtems_fatfs_node_t *node = (rtems_fatfs_node_t *) iop->pathinfo.node_access;
  rtems_fatfs_fs_info_t *fs_info = (rtems_fatfs_fs_info_t *)
                                     iop->pathinfo.mt_entry->fs_info;
  char    full_path[ 256 ];
  FRESULT fr;
  int     rc;

  if ( node == NULL || node->is_directory ) {
    rtems_set_errno_and_return_minus_one( EISDIR );
  }

  if ( node->is_open ) {
    return 0;
  }

  rtems_fatfs_lock( iop->pathinfo.mt_entry );

  rc = rtems_fatfs_resolve_node_path(
    fs_info,
    node,
    full_path,
    sizeof( full_path ),
    iop->pathinfo.mt_entry
  );
  if ( rc != 0 ) {
    return rc;
  }

  BYTE fatfs_mode = FA_READ;

  if ( oflag & O_WRONLY ) {
    fatfs_mode = FA_WRITE;
  } else if ( oflag & O_RDWR ) {
    fatfs_mode = FA_READ | FA_WRITE;
  }

  if ( oflag & O_CREAT ) {
    if ( oflag & O_WRONLY ) {
      fatfs_mode = FA_WRITE | FA_OPEN_EXISTING;
    } else if ( oflag & O_RDWR ) {
      fatfs_mode = FA_READ | FA_WRITE | FA_OPEN_EXISTING;
    } else {
      fatfs_mode = FA_READ | FA_OPEN_EXISTING;
    }
  }

  fr = f_open( &node->handle.file, full_path, fatfs_mode );
  if ( fr != FR_OK ) {
    rtems_fatfs_unlock( iop->pathinfo.mt_entry );
    errno = rtems_fatfs_fresult_to_errno( fr );
    return -1;
  }

  if ( oflag & O_TRUNC ) {
    if ( f_size( &node->handle.file ) == 0 ) {
      UINT bw;
      char dummy = 0;
      fr         = f_write( &node->handle.file, &dummy, 1, &bw );
      if ( fr == FR_OK ) {
        fr = f_truncate( &node->handle.file );
      }
    } else {
      fr = f_truncate( &node->handle.file );
    }

    if ( fr != FR_OK ) {
      f_close( &node->handle.file );
      rtems_fatfs_unlock( iop->pathinfo.mt_entry );
      errno = rtems_fatfs_fresult_to_errno( fr );
      return -1;
    }

    f_sync( &node->handle.file );
  }

  node->is_open = true;
  rtems_fatfs_unlock( iop->pathinfo.mt_entry );

  return 0;
}

int rtems_fatfs_closefile( rtems_libio_t *iop )
{
  rtems_fatfs_node_t *node = (rtems_fatfs_node_t *) iop->pathinfo.node_access;
  FRESULT             fr;

  if ( node == NULL || node->is_directory ) {
    rtems_set_errno_and_return_minus_one( EISDIR );
  }

  if ( !node->is_open ) {
    return 0;
  }

  rtems_fatfs_lock( iop->pathinfo.mt_entry );

  fr = f_close( &node->handle.file );
  if ( fr != FR_OK ) {
    rtems_fatfs_unlock( iop->pathinfo.mt_entry );
    errno = rtems_fatfs_fresult_to_errno( fr );
    return -1;
  }

  node->is_open = false;
  rtems_fatfs_unlock( iop->pathinfo.mt_entry );

  return 0;
}

const rtems_filesystem_file_handlers_r rtems_fatfs_file_handlers = {
  .open_h      = rtems_fatfs_openfile,
  .close_h     = rtems_fatfs_closefile,
  .read_h      = rtems_fatfs_file_read,
  .write_h     = rtems_fatfs_file_write,
  .ioctl_h     = rtems_filesystem_default_ioctl,
  .lseek_h     = rtems_fatfs_file_lseek,
  .fstat_h     = rtems_fatfs_file_fstat,
  .ftruncate_h = rtems_fatfs_file_ftruncate,
  .fsync_h     = rtems_fatfs_file_fsync,
  .fdatasync_h = rtems_fatfs_file_fsync,
  .fcntl_h     = rtems_filesystem_default_fcntl,
  .kqfilter_h  = rtems_filesystem_default_kqfilter,
  .mmap_h      = rtems_filesystem_default_mmap,
  .poll_h      = rtems_filesystem_default_poll,
  .readv_h     = rtems_filesystem_default_readv,
  .writev_h    = rtems_filesystem_default_writev
};
