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
#include <rtems/bdbuf.h>

rtems_status_code rtems_disk_init_phys(
  rtems_disk_device *dd,
  uint32_t block_size,
  rtems_blkdev_bnum block_count,
  rtems_block_device_ioctl handler,
  void *driver_data
)
{
  rtems_status_code sc;

  dd = memset(dd, 0, sizeof(*dd));

  dd->phys_dev = dd;
  dd->size = block_count;
  dd->media_block_size = block_size;
  dd->ioctl = handler;
  dd->driver_data = driver_data;
  dd->read_ahead.trigger = RTEMS_DISK_READ_AHEAD_NO_TRIGGER;

  if (block_count > 0) {
    if ((*handler)(dd, RTEMS_BLKIO_CAPABILITIES, &dd->capabilities) != 0) {
      dd->capabilities = 0;
    }

    sc = rtems_bdbuf_set_block_size(dd, block_size);
  } else {
    sc = RTEMS_INVALID_NUMBER;
  }

  return sc;
}

rtems_status_code rtems_disk_init_log(
  rtems_disk_device *dd,
  rtems_disk_device *phys_dd,
  rtems_blkdev_bnum block_begin,
  rtems_blkdev_bnum block_count
)
{
  rtems_status_code sc;

  dd = memset(dd, 0, sizeof(*dd));

  dd->phys_dev = phys_dd;
  dd->start = block_begin;
  dd->size = block_count;
  dd->media_block_size = phys_dd->media_block_size;
  dd->ioctl = phys_dd->ioctl;
  dd->driver_data = phys_dd->driver_data;
  dd->read_ahead.trigger = RTEMS_DISK_READ_AHEAD_NO_TRIGGER;

  if (phys_dd->phys_dev == phys_dd) {
    rtems_blkdev_bnum phys_block_count = phys_dd->size;

    if (
      block_begin < phys_block_count
        && block_count > 0
        && block_count <= phys_block_count - block_begin
    ) {
      sc = rtems_bdbuf_set_block_size(dd, phys_dd->media_block_size);
    } else {
      sc = RTEMS_INVALID_NUMBER;
    }
  } else {
    sc = RTEMS_INVALID_ID;
  }

  return sc;
}
