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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include <rtems/imfs.h>

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
