/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup IMFS
 */

/*
 *  COPYRIGHT (c) 1989-1999.
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

#include <string.h>

#include <rtems/imfsimpl.h>

static ssize_t IMFS_linfile_read(
  rtems_libio_t *iop,
  void          *buffer,
  size_t         count
)
{
  IMFS_file_t *file = IMFS_iop_to_file( iop );
  off_t start = iop->offset;
  size_t size = file->File.size;
  const unsigned char *data = file->Linearfile.direct;

  if (count > size - start)
    count = size - start;

  IMFS_update_atime( &file->Node );
  iop->offset = start + count;
  memcpy(buffer, &data[start], count);

  return (ssize_t) count;
}

static int IMFS_linfile_open(
  rtems_libio_t *iop,
  const char    *pathname,
  int            oflag,
  mode_t         mode
)
{
  IMFS_file_t *file;

  file = iop->pathinfo.node_access;

  /*
   * Perform 'copy on write' for linear files
   */
  if (rtems_libio_iop_is_writeable(iop)) {
    uint32_t count = file->File.size;
    const unsigned char *buffer = file->Linearfile.direct;

    file->Node.control            = &IMFS_mknod_control_memfile.node_control;
    file->File.size               = 0;
    file->Memfile.indirect        = 0;
    file->Memfile.doubly_indirect = 0;
    file->Memfile.triply_indirect = 0;

    IMFS_Set_handlers( &iop->pathinfo );

    if ((count != 0)
     && (IMFS_memfile_write(&file->Memfile, 0, buffer, count) == -1))
        return -1;
  }

  return 0;
}

static const rtems_filesystem_file_handlers_r IMFS_linfile_handlers = {
  .open_h = IMFS_linfile_open,
  .close_h = rtems_filesystem_default_close,
  .read_h = IMFS_linfile_read,
  .write_h = rtems_filesystem_default_write,
  .ioctl_h = rtems_filesystem_default_ioctl,
  .lseek_h = rtems_filesystem_default_lseek_file,
  .fstat_h = IMFS_stat_file,
  .ftruncate_h = rtems_filesystem_default_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync_success,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync_success,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .kqfilter_h = rtems_filesystem_default_kqfilter,
  .mmap_h = rtems_filesystem_default_mmap,
  .poll_h = rtems_filesystem_default_poll,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev
};

static IMFS_jnode_t *IMFS_node_initialize_linfile(
  IMFS_jnode_t *node,
  void         *arg
)
{
  IMFS_linearfile_t *linfile;
  IMFS_linearfile_context *ctx;

  linfile = (IMFS_linearfile_t *) node;
  ctx = arg;
  linfile->File.size = ctx->size;
  linfile->direct = RTEMS_DECONST( void *, ctx->data );

  return node;
}

const IMFS_node_control IMFS_node_control_linfile = {
  .handlers = &IMFS_linfile_handlers,
  .node_initialize = IMFS_node_initialize_linfile,
  .node_remove = IMFS_node_remove_default,
  .node_destroy = IMFS_node_destroy_default
};
