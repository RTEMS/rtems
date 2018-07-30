/**
 * @file
 *
 * @ingroup rtems_ramdisk
 *
 * @brief RAM disk block device implementation.
 */

/*
 * Copyright (c) 2009-2012 embedded brains GmbH.  All rights reserved.
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

#include <rtems/ramdisk.h>

rtems_status_code ramdisk_register(
  uint32_t media_block_size,
  rtems_blkdev_bnum media_block_count,
  bool trace,
  const char *disk
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  ramdisk *rd = NULL;

  rd = ramdisk_allocate(NULL, media_block_size, media_block_count, trace);
  if (rd == NULL) {
    return RTEMS_UNSATISFIED;
  }

  sc = rtems_blkdev_create(
    disk,
    media_block_size,
    media_block_count,
    ramdisk_ioctl,
    rd
  );
  if (sc != RTEMS_SUCCESSFUL) {
    ramdisk_free(rd);

    return RTEMS_UNSATISFIED;
  }

  return RTEMS_SUCCESSFUL;
}
