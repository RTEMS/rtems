/**
 * @file
 *
 * @brief File Operations Table for MSDOS FileSystem
 * @ingroup libfs
 */

/*
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/libio.h>
#include "msdos.h"

const rtems_filesystem_file_handlers_r msdos_file_handlers = {
  .open_h = rtems_filesystem_default_open,
  .close_h = rtems_filesystem_default_close,
  .read_h = msdos_file_read,
  .write_h = msdos_file_write,
  .ioctl_h = rtems_filesystem_default_ioctl,
  .lseek_h = rtems_filesystem_default_lseek_file,
  .fstat_h = msdos_file_stat,
  .ftruncate_h = msdos_file_ftruncate,
  .fsync_h = msdos_file_sync,
  .fdatasync_h = msdos_sync,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .kqfilter_h = rtems_filesystem_default_kqfilter,
  .mmap_h = rtems_filesystem_default_mmap,
  .poll_h = rtems_filesystem_default_poll,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev
};
