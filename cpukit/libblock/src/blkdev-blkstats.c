/**
 * @file
 *
 * @ingroup rtems_blkdev Block Device Management
 *
 * @brief Block Device Statistics Command
 */

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
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/blkdev.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

void rtems_blkstats(const rtems_printer* printer, const char *device, bool reset)
{
  int fd = open(device, O_RDONLY);

  if (fd >= 0) {
    struct stat st;
    int rv;

    rv = fstat(fd, &st);
    if (rv == 0) {
      if (S_ISBLK(st.st_mode)) {
        if (reset) {
          rv = rtems_disk_fd_reset_device_stats(fd);
          if (rv != 0) {
            rtems_printf(printer, "error: reset stats: %s\n", strerror(errno));
          }
        } else {
          uint32_t media_block_size = 0;
          uint32_t media_block_count = 0;
          uint32_t block_size = 0;
          rtems_blkdev_stats stats;

          rtems_disk_fd_get_media_block_size(fd, &media_block_size);
          rtems_disk_fd_get_block_count(fd, &media_block_count);
          rtems_disk_fd_get_block_size(fd, &block_size);

          rv = rtems_disk_fd_get_device_stats(fd, &stats);
          if (rv == 0) {
            rtems_blkdev_print_stats(
              &stats,
              media_block_size,
              media_block_count,
              block_size,
              printer
            );
          } else {
            rtems_printf(printer, "error: get stats: %s\n", strerror(errno));
          }
        }
      } else {
        rtems_printf(printer, "error: not a block device\n");
      }
    } else {
      rtems_printf(printer, "error: get file stats: %s\n", strerror(errno));
    }

    rv = close(fd);
    if (rv != 0) {
      rtems_printf(printer, "error: close device: %s\n", strerror(errno));
    }
  } else {
    rtems_printf(printer, "error: open device: %s\n", strerror(errno));
  }
}
