/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSApplicationConfiguration
 *
 * @brief Evaluate IO Library Configuration Options
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

#ifndef _RTEMS_CONFDEFS_LIBIO_H
#define _RTEMS_CONFDEFS_LIBIO_H

#ifndef __CONFIGURATION_TEMPLATE_h
#error "Do not include this file directly, use <rtems/confdefs.h> instead"
#endif

#ifdef CONFIGURE_INIT

#include <rtems/confdefs/bsp.h>
#include <rtems/libio.h>
#include <rtems/sysinit.h>

#ifdef CONFIGURE_FILESYSTEM_ALL
  #define CONFIGURE_FILESYSTEM_DOSFS
  #define CONFIGURE_FILESYSTEM_FTPFS
  #define CONFIGURE_FILESYSTEM_IMFS
  #define CONFIGURE_FILESYSTEM_JFFS2
  #define CONFIGURE_FILESYSTEM_NFS
  #define CONFIGURE_FILESYSTEM_RFS
  #define CONFIGURE_FILESYSTEM_TFTPFS
#endif

#ifdef CONFIGURE_USE_MINIIMFS_AS_BASE_FILESYSTEM
  #ifdef CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM
    #error "CONFIGURE_USE_MINIIMFS_AS_BASE_FILESYSTEM cannot be used together with CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM"
  #endif

  #define CONFIGURE_IMFS_DISABLE_CHMOD
  #define CONFIGURE_IMFS_DISABLE_CHOWN
  #define CONFIGURE_IMFS_DISABLE_LINK
  #define CONFIGURE_IMFS_DISABLE_READLINK
  #define CONFIGURE_IMFS_DISABLE_RENAME
  #define CONFIGURE_IMFS_DISABLE_SYMLINK
  #define CONFIGURE_IMFS_DISABLE_UNMOUNT
  #define CONFIGURE_IMFS_DISABLE_UTIME
#endif

#ifdef CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM
  #define CONFIGURE_IMFS_DISABLE_CHMOD
  #define CONFIGURE_IMFS_DISABLE_CHOWN
  #define CONFIGURE_IMFS_DISABLE_LINK
  #define CONFIGURE_IMFS_DISABLE_MKNOD_FILE
  #define CONFIGURE_IMFS_DISABLE_MOUNT
  #define CONFIGURE_IMFS_DISABLE_READDIR
  #define CONFIGURE_IMFS_DISABLE_READLINK
  #define CONFIGURE_IMFS_DISABLE_RENAME
  #define CONFIGURE_IMFS_DISABLE_RMNOD
  #define CONFIGURE_IMFS_DISABLE_SYMLINK
  #define CONFIGURE_IMFS_DISABLE_UNMOUNT
  #define CONFIGURE_IMFS_DISABLE_UTIME
#endif

#ifdef CONFIGURE_APPLICATION_DISABLE_FILESYSTEM
  #ifdef CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM
    #error "CONFIGURE_APPLICATION_DISABLE_FILESYSTEM cannot be used together with CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM"
  #endif

  #ifdef CONFIGURE_USE_MINIIMFS_AS_BASE_FILESYSTEM
    #error "CONFIGURE_APPLICATION_DISABLE_FILESYSTEM cannot be used together with CONFIGURE_USE_MINIIMFS_AS_BASE_FILESYSTEM"
  #endif

  #ifdef CONFIGURE_FILESYSTEM_DOSFS
    #error "CONFIGURE_APPLICATION_DISABLE_FILESYSTEM cannot be used together with CONFIGURE_FILESYSTEM_DOSFS"
  #endif

  #ifdef CONFIGURE_FILESYSTEM_FTPFS
    #error "CONFIGURE_APPLICATION_DISABLE_FILESYSTEM cannot be used together with CONFIGURE_FILESYSTEM_FTPFS"
  #endif

  #ifdef CONFIGURE_FILESYSTEM_IMFS
    #error "CONFIGURE_APPLICATION_DISABLE_FILESYSTEM cannot be used together with CONFIGURE_FILESYSTEM_IMFS"
  #endif

  #ifdef CONFIGURE_FILESYSTEM_JFFS2
    #error "CONFIGURE_APPLICATION_DISABLE_FILESYSTEM cannot be used together with CONFIGURE_FILESYSTEM_JFFS2"
  #endif

  #ifdef CONFIGURE_FILESYSTEM_NFS
    #error "CONFIGURE_APPLICATION_DISABLE_FILESYSTEM cannot be used together with CONFIGURE_FILESYSTEM_NFS"
  #endif

  #ifdef CONFIGURE_FILESYSTEM_RFS
    #error "CONFIGURE_APPLICATION_DISABLE_FILESYSTEM cannot be used together with CONFIGURE_FILESYSTEM_RFS"
  #endif

  #ifdef CONFIGURE_FILESYSTEM_TFTPFS
    #error "CONFIGURE_APPLICATION_DISABLE_FILESYSTEM cannot be used together with CONFIGURE_FILESYSTEM_TFTPFS"
  #endif
#endif

#include <rtems/imfs.h>

#ifdef CONFIGURE_FILESYSTEM_DOSFS
#include <rtems/dosfs.h>
#endif

#ifdef CONFIGURE_FILESYSTEM_FTPFS
#include <rtems/ftpfs.h>
#endif

#ifdef CONFIGURE_FILESYSTEM_JFFS2
#include <rtems/jffs2.h>
#endif

#ifdef CONFIGURE_FILESYSTEM_NFS
#include <librtemsNfs.h>
#endif

#ifdef CONFIGURE_FILESYSTEM_RFS
#include <rtems/rtems-rfs.h>
#endif

#ifdef CONFIGURE_FILESYSTEM_TFTPFS
#include <rtems/tftp.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

#ifndef CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK
  #define CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK \
    IMFS_MEMFILE_DEFAULT_BYTES_PER_BLOCK
#endif

#if CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK != 16 && \
  CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK != 32 && \
  CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK != 64 && \
  CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK != 128 && \
  CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK != 256 && \
  CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK != 512
  #error "CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK must be a power of two between 16 and 512"
#endif

const int imfs_memfile_bytes_per_block = CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK;

static IMFS_fs_info_t IMFS_root_fs_info;

static const rtems_filesystem_operations_table IMFS_root_ops = {
  rtems_filesystem_default_lock,
  rtems_filesystem_default_unlock,
  #ifdef CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM
    IMFS_eval_path_devfs,
  #else
    IMFS_eval_path,
  #endif
  #ifdef CONFIGURE_IMFS_DISABLE_LINK
    rtems_filesystem_default_link,
  #else
    IMFS_link,
  #endif
  rtems_filesystem_default_are_nodes_equal,
  #ifdef CONFIGURE_IMFS_DISABLE_MKNOD
    rtems_filesystem_default_mknod,
  #else
    IMFS_mknod,
  #endif
  #ifdef CONFIGURE_IMFS_DISABLE_RMNOD
    rtems_filesystem_default_rmnod,
  #else
    IMFS_rmnod,
  #endif
  #ifdef CONFIGURE_IMFS_DISABLE_CHMOD
    rtems_filesystem_default_fchmod,
  #else
    IMFS_fchmod,
  #endif
  #ifdef CONFIGURE_IMFS_DISABLE_CHOWN
    rtems_filesystem_default_chown,
  #else
    IMFS_chown,
  #endif
  IMFS_node_clone,
  IMFS_node_free,
  #ifdef CONFIGURE_IMFS_DISABLE_MOUNT
    rtems_filesystem_default_mount,
  #else
    IMFS_mount,
  #endif
  #ifdef CONFIGURE_IMFS_DISABLE_UNMOUNT
    rtems_filesystem_default_unmount,
  #else
    IMFS_unmount,
  #endif
  rtems_filesystem_default_fsunmount,
  #ifdef CONFIGURE_IMFS_DISABLE_UTIME
    rtems_filesystem_default_utime,
  #else
    IMFS_utime,
  #endif
  #ifdef CONFIGURE_IMFS_DISABLE_SYMLINK
    rtems_filesystem_default_symlink,
  #else
    IMFS_symlink,
  #endif
  #ifdef CONFIGURE_IMFS_DISABLE_READLINK
    rtems_filesystem_default_readlink,
  #else
    IMFS_readlink,
  #endif
  #ifdef CONFIGURE_IMFS_DISABLE_RENAME
    rtems_filesystem_default_rename,
  #else
    IMFS_rename,
  #endif
  rtems_filesystem_default_statvfs
};

static const IMFS_mknod_controls IMFS_root_mknod_controls = {
  #ifdef CONFIGURE_IMFS_DISABLE_READDIR
    &IMFS_mknod_control_dir_minimal,
  #else
    &IMFS_mknod_control_dir_default,
  #endif
  #ifdef CONFIGURE_IMFS_DISABLE_MKNOD_DEVICE
    &IMFS_mknod_control_enosys,
  #else
    &IMFS_mknod_control_device,
  #endif
  #ifdef CONFIGURE_IMFS_DISABLE_MKNOD_FILE
    &IMFS_mknod_control_enosys,
  #else
    &IMFS_mknod_control_memfile,
  #endif
  #ifdef CONFIGURE_IMFS_ENABLE_MKFIFO
    &IMFS_mknod_control_fifo
  #else
    &IMFS_mknod_control_enosys
  #endif
};

static const IMFS_mount_data IMFS_root_mount_data = {
  &IMFS_root_fs_info,
  &IMFS_root_ops,
  &IMFS_root_mknod_controls
};

const rtems_filesystem_table_t rtems_filesystem_table[] = {
  { "/", IMFS_initialize_support },
  #ifdef CONFIGURE_FILESYSTEM_DOSFS
    { RTEMS_FILESYSTEM_TYPE_DOSFS, rtems_dosfs_initialize },
  #endif
  #ifdef CONFIGURE_FILESYSTEM_FTPFS
    { RTEMS_FILESYSTEM_TYPE_FTPFS, rtems_ftpfs_initialize },
  #endif
  #ifdef CONFIGURE_FILESYSTEM_IMFS
    { RTEMS_FILESYSTEM_TYPE_IMFS, IMFS_initialize },
  #endif
  #ifdef CONFIGURE_FILESYSTEM_JFFS2
    { RTEMS_FILESYSTEM_TYPE_JFFS2, rtems_jffs2_initialize },
  #endif
  #ifdef CONFIGURE_FILESYSTEM_NFS
    { RTEMS_FILESYSTEM_TYPE_NFS, rtems_nfs_initialize },
  #endif
  #ifdef CONFIGURE_FILESYSTEM_RFS
    { RTEMS_FILESYSTEM_TYPE_RFS, rtems_rfs_rtems_initialise },
  #endif
  #ifdef CONFIGURE_FILESYSTEM_TFTPFS
    { RTEMS_FILESYSTEM_TYPE_TFTPFS, rtems_tftpfs_initialize },
  #endif
  { NULL, NULL }
};

const rtems_filesystem_mount_configuration
rtems_filesystem_root_configuration = {
  NULL,
  NULL,
  "/",
  RTEMS_FILESYSTEM_READ_WRITE,
  &IMFS_root_mount_data
};

RTEMS_SYSINIT_ITEM(
  rtems_filesystem_initialize,
  RTEMS_SYSINIT_ROOT_FILESYSTEM,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

#endif /* !CONFIGURE_APPLICATION_DISABLE_FILESYSTEM */

#ifndef CONFIGURE_MAXIMUM_FILE_DESCRIPTORS
  #define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 3
#endif

rtems_libio_t rtems_libio_iops[ CONFIGURE_MAXIMUM_FILE_DESCRIPTORS ];

const uint32_t rtems_libio_number_iops = RTEMS_ARRAY_SIZE( rtems_libio_iops );

#ifdef __cplusplus
}
#endif

#endif /* CONFIGURE_INIT */

#endif /* _RTEMS_CONFDEFS_LIBIO_H */
