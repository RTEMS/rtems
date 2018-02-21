/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

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
_Console_simple_Node_control = IMFS_GENERIC_INITIALIZER(
  &_Console_simple_Handlers,
  IMFS_node_initialize_default,
  IMFS_node_destroy_default
);

void _Console_simple_Initialize( void )
{
  IMFS_make_generic_node(
    CONSOLE_DEVICE_NAME,
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &_Console_simple_Node_control,
    NULL
  );
}
