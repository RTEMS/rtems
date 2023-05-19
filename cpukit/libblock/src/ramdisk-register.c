/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_ramdisk
 *
 * @brief RAM disk block device implementation.
 */

/*
 * Copyright (C) 2009, 2012 embedded brains GmbH & Co. KG
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
