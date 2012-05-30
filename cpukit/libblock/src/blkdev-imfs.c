/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <rtems/blkdev.h>
#include <rtems/bdbuf.h>
#include <rtems/imfs.h>

typedef struct {
  rtems_disk_device dd;
  int fd;
} rtems_blkdev_imfs_context;

static ssize_t rtems_blkdev_imfs_read(
  rtems_libio_t *iop,
  void *buffer,
  size_t count
)
{
  int rv;
  rtems_blkdev_imfs_context *ctx = IMFS_generic_get_context_by_iop(iop);
  rtems_disk_device *dd = &ctx->dd;
  ssize_t remaining = (ssize_t) count;
  off_t offset = iop->offset;
  ssize_t block_size = (ssize_t) rtems_disk_get_block_size(dd);
  rtems_blkdev_bnum block = (rtems_blkdev_bnum) (offset / block_size);
  ssize_t block_offset = (ssize_t) (offset % block_size);
  char *dst = buffer;

  while (remaining > 0) {
    rtems_bdbuf_buffer *bd;
    rtems_status_code sc = rtems_bdbuf_read(dd, block, &bd);

    if (sc == RTEMS_SUCCESSFUL) {
      ssize_t copy = block_size - block_offset;

      if (copy > remaining) {
        copy = remaining;
      }

      memcpy(dst, (char *) bd->buffer + block_offset, (size_t) copy);

      sc = rtems_bdbuf_release(bd);
      if (sc == RTEMS_SUCCESSFUL) {
        block_offset = 0;
        remaining -= copy;
        dst += copy;
        ++block;
      } else {
        remaining = -1;
      }
    } else {
      remaining = -1;
    }
  }

  if (remaining >= 0) {
    iop->offset += count;
    rv = (ssize_t) count;
  } else {
    errno = EIO;
    rv = -1;
  }

  return rv;
}

static ssize_t rtems_blkdev_imfs_write(
  rtems_libio_t *iop,
  const void *buffer,
  size_t count
)
{
  int rv;
  rtems_blkdev_imfs_context *ctx = IMFS_generic_get_context_by_iop(iop);
  rtems_disk_device *dd = &ctx->dd;
  ssize_t remaining = (ssize_t) count;
  off_t offset = iop->offset;
  ssize_t block_size = (ssize_t) rtems_disk_get_block_size(dd);
  rtems_blkdev_bnum block = (rtems_blkdev_bnum) (offset / block_size);
  ssize_t block_offset = (ssize_t) (offset % block_size);
  const char *src = buffer;

  while (remaining > 0) {
    rtems_status_code sc;
    rtems_bdbuf_buffer *bd;

    if (block_offset == 0 && remaining >= block_size) {
       sc = rtems_bdbuf_get(dd, block, &bd);
    } else {
       sc = rtems_bdbuf_read(dd, block, &bd);
    }

    if (sc == RTEMS_SUCCESSFUL) {
      ssize_t copy = block_size - block_offset;

      if (copy > remaining) {
        copy = remaining;
      }

      memcpy((char *) bd->buffer + block_offset, src, (size_t) copy);

      sc = rtems_bdbuf_release_modified(bd);
      if (sc == RTEMS_SUCCESSFUL) {
        block_offset = 0;
        remaining -= copy;
        src += copy;
        ++block;
      } else {
        remaining = -1;
      }
    } else {
      remaining = -1;
    }
  }

  if (remaining >= 0) {
    iop->offset += count;
    rv = (ssize_t) count;
  } else {
    errno = EIO;
    rv = -1;
  }

  return rv;
}

static int rtems_blkdev_imfs_ioctl(
  rtems_libio_t *iop,
  uint32_t request,
  void *buffer
)
{
  int rv = 0;

  if (request != RTEMS_BLKIO_REQUEST) {
    rtems_blkdev_imfs_context *ctx = IMFS_generic_get_context_by_iop(iop);
    rtems_disk_device *dd = &ctx->dd;

    rv = (*dd->ioctl)(dd, request, buffer);
  } else {
    /*
     * It is not allowed to directly access the driver circumventing the cache.
     */
    errno = EINVAL;
    rv = -1;
  }

  return rv;
}

static int rtems_blkdev_imfs_fstat(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
)
{
  rtems_blkdev_imfs_context *ctx =
    IMFS_generic_get_context_by_location(loc);
  rtems_disk_device *dd = &ctx->dd;

  buf->st_rdev = rtems_disk_get_device_identifier(dd);
  buf->st_blksize = rtems_disk_get_block_size(dd);
  buf->st_blocks = rtems_disk_get_block_count(dd);

  return IMFS_stat(loc, buf);
}

static int rtems_blkdev_imfs_fsync_or_fdatasync(
  rtems_libio_t *iop
)
{
  int rv = 0;
  rtems_blkdev_imfs_context *ctx = IMFS_generic_get_context_by_iop(iop);
  rtems_disk_device *dd = &ctx->dd;
  rtems_status_code sc = rtems_bdbuf_syncdev(dd);

  if (sc != RTEMS_SUCCESSFUL) {
    errno = EIO;
    rv = -1;
  }

  return rv;
}

static const rtems_filesystem_file_handlers_r rtems_blkdev_imfs_node = {
  .open_h = rtems_filesystem_default_open,
  .close_h = rtems_filesystem_default_close,
  .read_h = rtems_blkdev_imfs_read,
  .write_h = rtems_blkdev_imfs_write,
  .ioctl_h = rtems_blkdev_imfs_ioctl,
  .lseek_h = rtems_filesystem_default_lseek_file,
  .fstat_h = rtems_blkdev_imfs_fstat,
  .ftruncate_h = rtems_filesystem_default_ftruncate,
  .fsync_h = rtems_blkdev_imfs_fsync_or_fdatasync,
  .fdatasync_h = rtems_blkdev_imfs_fsync_or_fdatasync,
  .fcntl_h = rtems_filesystem_default_fcntl
};

static IMFS_jnode_t *rtems_blkdev_imfs_initialize(
  IMFS_jnode_t *node,
  const IMFS_types_union *info
)
{
  rtems_blkdev_imfs_context *ctx;
  rtems_disk_device *dd;

  node = IMFS_node_initialize_generic(node, info);

  ctx = IMFS_generic_get_context_by_node(node);
  dd = &ctx->dd;
  dd->dev = IMFS_generic_get_device_identifier_by_node(node);

  return node;
}

static IMFS_jnode_t *rtems_blkdev_imfs_destroy(IMFS_jnode_t *node)
{
  rtems_blkdev_imfs_context *ctx = IMFS_generic_get_context_by_node(node);
  rtems_disk_device *dd = &ctx->dd;

  rtems_bdbuf_syncdev(dd);
  rtems_bdbuf_purge_dev(dd);

  if (ctx->fd >= 0) {
    close(ctx->fd);
  } else {
    (*dd->ioctl)(dd, RTEMS_BLKIO_DELETED, NULL);
  }

  free(ctx);

  return node;
}

static const IMFS_node_control rtems_blkdev_imfs_control = {
  .imfs_type = IMFS_GENERIC,
  .handlers = &rtems_blkdev_imfs_node,
  .node_initialize = rtems_blkdev_imfs_initialize,
  .node_remove = IMFS_node_remove_default,
  .node_destroy = rtems_blkdev_imfs_destroy
};

rtems_status_code rtems_blkdev_create(
  const char *device,
  uint32_t block_size,
  rtems_blkdev_bnum block_count,
  rtems_block_device_ioctl handler,
  void *driver_data
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_blkdev_imfs_context *ctx = malloc(sizeof(*ctx));

  if (ctx != NULL) {
    sc = rtems_disk_init_phys(
      &ctx->dd,
      block_size,
      block_count,
      handler,
      driver_data
    );

    ctx->fd = -1;

    if (sc == RTEMS_SUCCESSFUL) {
      int rv = IMFS_make_generic_node(
        device,
        S_IFBLK | S_IRWXU | S_IRWXG | S_IRWXO,
        &rtems_blkdev_imfs_control,
        ctx
      );

      if (rv != 0) {
        free(ctx);
        sc = RTEMS_UNSATISFIED;
      }
    } else {
      free(ctx);
    }
  } else {
    sc = RTEMS_NO_MEMORY;
  }

  return sc;
}

rtems_status_code rtems_blkdev_create_partition(
  const char *partition,
  const char *device,
  rtems_blkdev_bnum block_begin,
  rtems_blkdev_bnum block_count
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  int fd = open(device, O_RDWR);

  if (fd >= 0) {
    int rv;
    struct stat st;

    rv = fstat(fd, &st);
    if (rv == 0 && S_ISBLK(st.st_mode)) {
      rtems_disk_device *phys_dd;

      rv = rtems_disk_fd_get_disk_device(fd, &phys_dd);
      if (rv == 0) {
        rtems_blkdev_imfs_context *ctx = malloc(sizeof(*ctx));

        if (ctx != NULL) {
          sc = rtems_disk_init_log(
            &ctx->dd,
            phys_dd,
            block_begin,
            block_count
          );

          if (sc == RTEMS_SUCCESSFUL) {
            ctx->fd = fd;

            rv = IMFS_make_generic_node(
              partition,
              S_IFBLK | S_IRWXU | S_IRWXG | S_IRWXO,
              &rtems_blkdev_imfs_control,
              ctx
            );

            if (rv != 0) {
              free(ctx);
              sc = RTEMS_UNSATISFIED;
            }
          } else {
            free(ctx);
          }
        } else {
          sc = RTEMS_NO_MEMORY;
        }
      } else {
        sc = RTEMS_NOT_IMPLEMENTED;
      }
    } else {
      sc = RTEMS_INVALID_NODE;
    }

    if (sc != RTEMS_SUCCESSFUL) {
      close(fd);
    }
  } else {
    sc = RTEMS_INVALID_ID;
  }

  return sc;
}
