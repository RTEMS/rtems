/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup IMFS
 *
 * @brief FIFO Support
 */

/*
 * Copyright (C) 2008 Wei Shen <jshen.yn@gmail.com>
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

#include <rtems/imfsimpl.h>

#include <sys/filio.h>

#define JNODE2PIPE(_jnode)  ( ((IMFS_fifo_t *)(_jnode))->pipe )

#define LIBIO2PIPE(_iop)  ( JNODE2PIPE((IMFS_jnode_t *)(_iop)->pathinfo.node_access) )

/* Set errno and return -1 if error, else return _err */
#define IMFS_FIFO_RETURN(_err) \
do {  \
  if (_err < 0)	\
    rtems_set_errno_and_return_minus_one(-_err); \
  return _err; \
} while (0)

static int IMFS_fifo_open(
  rtems_libio_t *iop,
  const char    *pathname,
  int            oflag,
  mode_t         mode
)
{
  (void) pathname;
  (void) oflag;
  (void) mode;

  IMFS_jnode_t *jnode = iop->pathinfo.node_access;

  int err = fifo_open(&JNODE2PIPE(jnode), iop);
  IMFS_FIFO_RETURN(err);
}

static int IMFS_fifo_close(
  rtems_libio_t *iop
)
{
  int err = 0;
  IMFS_jnode_t *jnode = iop->pathinfo.node_access;

  pipe_release(&JNODE2PIPE(jnode), iop);

  IMFS_FIFO_RETURN(err);
}

static ssize_t IMFS_fifo_read(
  rtems_libio_t *iop,
  void          *buffer,
  size_t         count
)
{
  IMFS_jnode_t *jnode = iop->pathinfo.node_access;

  int err = pipe_read(JNODE2PIPE(jnode), buffer, count, iop);
  if (err > 0)
    IMFS_update_atime(jnode);

  IMFS_FIFO_RETURN(err);
}

static ssize_t IMFS_fifo_write(
  rtems_libio_t *iop,
  const void    *buffer,
  size_t         count
)
{
  IMFS_jnode_t *jnode = iop->pathinfo.node_access;

  int err = pipe_write(JNODE2PIPE(jnode), buffer, count, iop);
  if (err > 0) {
    IMFS_mtime_ctime_update(jnode);
  }

  IMFS_FIFO_RETURN(err);
}

static int IMFS_fifo_ioctl(
  rtems_libio_t   *iop,
  ioctl_command_t  command,
  void            *buffer
)
{
  int err;

  if (command == FIONBIO) {
    if (buffer == NULL)
      err = -EFAULT;
    else {
      if (*(int *)buffer)
        rtems_libio_iop_flags_set( iop, LIBIO_FLAGS_NO_DELAY );
      else
        rtems_libio_iop_flags_clear( iop, LIBIO_FLAGS_NO_DELAY );
      return 0;
    }
  }
  else
    err = pipe_ioctl(LIBIO2PIPE(iop), command, buffer, iop);

  IMFS_FIFO_RETURN(err);
}

static const rtems_filesystem_file_handlers_r IMFS_fifo_handlers = {
  .open_h = IMFS_fifo_open,
  .close_h = IMFS_fifo_close,
  .read_h = IMFS_fifo_read,
  .write_h = IMFS_fifo_write,
  .ioctl_h = IMFS_fifo_ioctl,
  .lseek_h = rtems_filesystem_default_lseek,
  .fstat_h = IMFS_stat,
  .ftruncate_h = rtems_filesystem_default_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .kqfilter_h = rtems_filesystem_default_kqfilter,
  .mmap_h = rtems_filesystem_default_mmap,
  .poll_h = rtems_filesystem_default_poll,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev
};

const IMFS_mknod_control IMFS_mknod_control_fifo = {
  {
    .handlers = &IMFS_fifo_handlers,
    .node_initialize = IMFS_node_initialize_default,
    .node_remove = IMFS_node_remove_default,
    .node_destroy = IMFS_node_destroy_default
  },
  .node_size = sizeof( IMFS_fifo_t )
};
