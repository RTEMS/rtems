/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup FatFS
 *
 * @brief RTEMS FATFS directory operations
 */

/*
 * Copyright (C) 2025 Sepehr Ganji <sepehrganji79@gmail.com>
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

#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>

#include "ff.h"
#include "rtems-fatfs.h"

#define DIR POSIX_DIR
#include <dirent.h>
#undef DIR

ssize_t rtems_fatfs_dir_read( rtems_libio_t *iop, void *buffer, size_t count )
{
  rtems_fatfs_node_t *node = (rtems_fatfs_node_t *) iop->pathinfo.node_access;
  struct dirent      *dp   = (struct dirent *) buffer;
  FILINFO             fno;
  FRESULT             fr;
  size_t              bytes_transferred = 0;

  if ( node == NULL || !node->is_directory ) {
    rtems_set_errno_and_return_minus_one( ENOTDIR );
  }

  if ( count < sizeof( struct dirent ) ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  rtems_fatfs_lock( iop->pathinfo.mt_entry );

  memset( &fno, 0, sizeof( fno ) );

  fr = f_readdir( &node->handle.dir, &fno );
  if ( fr != FR_OK ) {
    rtems_fatfs_unlock( iop->pathinfo.mt_entry );
    errno = rtems_fatfs_fresult_to_errno( fr );
    return -1;
  }

  if ( fno.fname[ 0 ] == '\0' ) {
    rtems_fatfs_unlock( iop->pathinfo.mt_entry );
    return 0;
  }

  memset( dp, 0, sizeof( struct dirent ) );
  dp->d_ino    = 1;
  dp->d_off    = iop->offset;
  dp->d_reclen = sizeof( struct dirent );

  size_t name_len = strlen( fno.fname );
  if ( name_len >= sizeof( dp->d_name ) ) {
    name_len = sizeof( dp->d_name ) - 1;
  }
  memcpy( dp->d_name, fno.fname, name_len );
  dp->d_name[ name_len ] = '\0';
  dp->d_namlen           = name_len;

  if ( fno.fattrib & AM_DIR ) {
    dp->d_type = DT_DIR;
  } else {
    dp->d_type = DT_REG;
  }

  bytes_transferred  = sizeof( struct dirent );
  iop->offset       += sizeof( struct dirent );

  rtems_fatfs_unlock( iop->pathinfo.mt_entry );

  return (ssize_t) bytes_transferred;
}

int rtems_fatfs_dir_fstat(
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

int rtems_fatfs_opendir(
  rtems_libio_t *iop,
  const char    *path,
  int            oflag,
  mode_t         mode
)
{
  (void) path;
  (void) oflag;
  (void) mode;

  rtems_fatfs_node_t *node = (rtems_fatfs_node_t *) iop->pathinfo.node_access;
  rtems_fatfs_fs_info_t *fs_info = (rtems_fatfs_fs_info_t *)
                                     iop->pathinfo.mt_entry->fs_info;
  char    full_path[ 256 ];
  FRESULT fr;
  int     rc;

  if ( node == NULL || !node->is_directory ) {
    rtems_set_errno_and_return_minus_one( ENOTDIR );
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

  fr = f_opendir( &node->handle.dir, full_path );
  if ( fr != FR_OK ) {
    rtems_fatfs_unlock( iop->pathinfo.mt_entry );
    errno = rtems_fatfs_fresult_to_errno( fr );
    return -1;
  }

  node->is_open = true;
  rtems_fatfs_unlock( iop->pathinfo.mt_entry );

  return 0;
}

int rtems_fatfs_closedir( rtems_libio_t *iop )
{
  rtems_fatfs_node_t *node = (rtems_fatfs_node_t *) iop->pathinfo.node_access;
  FRESULT             fr;

  if ( node == NULL || !node->is_directory ) {
    rtems_set_errno_and_return_minus_one( ENOTDIR );
  }

  if ( !node->is_open ) {
    return 0;
  }

  rtems_fatfs_lock( iop->pathinfo.mt_entry );

  fr = f_closedir( &node->handle.dir );
  if ( fr != FR_OK ) {
    rtems_fatfs_unlock( iop->pathinfo.mt_entry );
    errno = rtems_fatfs_fresult_to_errno( fr );
    return -1;
  }

  node->is_open = false;
  rtems_fatfs_unlock( iop->pathinfo.mt_entry );

  return 0;
}

const rtems_filesystem_file_handlers_r rtems_fatfs_dir_handlers = {
  .open_h      = rtems_fatfs_opendir,
  .close_h     = rtems_fatfs_closedir,
  .read_h      = rtems_fatfs_dir_read,
  .write_h     = rtems_filesystem_default_write,
  .ioctl_h     = rtems_filesystem_default_ioctl,
  .lseek_h     = rtems_filesystem_default_lseek_directory,
  .fstat_h     = rtems_fatfs_dir_fstat,
  .ftruncate_h = rtems_filesystem_default_ftruncate_directory,
  .fsync_h     = rtems_fatfs_file_fsync,
  .fdatasync_h = rtems_fatfs_file_fsync,
  .fcntl_h     = rtems_filesystem_default_fcntl,
  .kqfilter_h  = rtems_filesystem_default_kqfilter,
  .mmap_h      = rtems_filesystem_default_mmap,
  .poll_h      = rtems_filesystem_default_poll,
  .readv_h     = rtems_filesystem_default_readv,
  .writev_h    = rtems_filesystem_default_writev
};
