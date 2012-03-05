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
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <stdlib.h>

#include <rtems.h>
#include <rtems/ramdisk.h>

ramdisk *ramdisk_allocate(
  void *area_begin,
  uint32_t block_size,
  rtems_blkdev_bnum block_count,
  bool trace
)
{
  struct ramdisk *rd = malloc(sizeof(struct ramdisk));

  if (rd == NULL) {
    return NULL;
  }

  if (area_begin == NULL) {
    area_begin = calloc(block_count, block_size);
    if (area_begin == NULL) {
      free(rd);

      return NULL;
    }
    rd->malloced = true;
  } else {
    rd->malloced = false;
  }
  rd->block_size = block_size;
  rd->block_num = block_count;
  rd->area = area_begin;
  rd->trace = trace;
  rd->initialized = true;

  return rd;
}

void ramdisk_free(ramdisk *rd)
{
  if (rd != NULL) {
    if (rd->malloced) {
      free(rd->area);
    }
    free(rd);
  }
}
