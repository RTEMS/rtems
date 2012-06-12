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

#include <rtems/blkdev.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

void rtems_blkstats(FILE *output, const char *device, bool reset)
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
            fprintf(output, "error: reset stats: %s\n", strerror(errno));
          }
        } else {
          rtems_blkdev_stats stats;

          rv = rtems_disk_fd_get_device_stats(fd, &stats);
          if (rv == 0) {
            rtems_blkdev_print_stats(
              &stats,
              (rtems_printk_plugin_t) fprintf,
              output
            );
          } else {
            fprintf(output, "error: get stats: %s\n", strerror(errno));
          }
        }
      } else {
        fprintf(output, "error: not a block device\n");
      }
    } else {
      fprintf(output, "error: get file stats: %s\n", strerror(errno));
    }

    rv = close(fd);
    if (rv != 0) {
      fprintf(output, "error: close device: %s\n", strerror(errno));
    }
  } else {
    fprintf(output, "error: open device: %s\n", strerror(errno));
  }
}
