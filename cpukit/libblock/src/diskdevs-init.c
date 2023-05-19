/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_disk Block Device Disk Management
 *
 * @brief Block Device Disk Management Initialize
 */

/*
 * Copyright (c) 2012 embedded brains GmbH & Co. KG
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

#include <rtems/blkdev.h>
#include <rtems/bdbuf.h>

#include <string.h>

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

    sc = rtems_bdbuf_set_block_size(dd, block_size, false);
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
      sc = rtems_bdbuf_set_block_size(dd, phys_dd->media_block_size, false);
    } else {
      sc = RTEMS_INVALID_NUMBER;
    }
  } else {
    sc = RTEMS_INVALID_ID;
  }

  return sc;
}
