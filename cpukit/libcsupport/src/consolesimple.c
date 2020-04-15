/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2017, 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

#include <rtems/console.h>
#include <rtems/bspIo.h>
#include <rtems/imfs.h>

#include "consolesimple.h"

static ssize_t _Console_simple_Write(
  rtems_libio_t *iop,
  const void    *buffer,
  size_t         count
)
{
  const char *buf;
  ssize_t     i;
  ssize_t     n;

  buf = buffer;
  n = (ssize_t) count;

  for ( i = 0; i < n; ++i ) {
    rtems_putc( buf[ i ] );
  }

  return n;
}

static const rtems_filesystem_file_handlers_r _Console_simple_Handlers = {
  .open_h = rtems_filesystem_default_open,
  .close_h = rtems_filesystem_default_close,
  .read_h = _Console_simple_Read,
  .write_h = _Console_simple_Write,
  .ioctl_h = rtems_filesystem_default_ioctl,
  .lseek_h = rtems_filesystem_default_lseek,
  .fstat_h = IMFS_stat,
  .ftruncate_h = rtems_filesystem_default_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev,
  .mmap_h = rtems_filesystem_default_mmap
};

static const IMFS_node_control
_Console_simple_Node_control = IMFS_NODE_CONTROL_INITIALIZER(
  &_Console_simple_Handlers,
  IMFS_node_initialize_default,
  IMFS_do_nothing_destroy
);

static const char _Console_simple_Name[] = "console";

static IMFS_jnode_t _Console_simple_Node = IMFS_NODE_INITIALIZER(
  &_Console_simple_Node_control,
  _Console_simple_Name,
  sizeof( _Console_simple_Name ) - 1,
  S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO
);

void _Console_simple_Initialize( void )
{
  IMFS_add_node( "/dev", &_Console_simple_Node, NULL );
}
