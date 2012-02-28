/**
 * @file
 *
 * @ingroup rtems_bdpart
 *
 * Block device partition management.
 */

/*
 * Copyright (c) 2009, 2010
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/bdpart.h>

rtems_status_code rtems_bdpart_create(
  const char *disk_name,
  const rtems_bdpart_format *format,
  rtems_bdpart_partition *pt,
  const unsigned *dist,
  size_t count
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  bool dos_compatibility = format != NULL
    && format->type == RTEMS_BDPART_FORMAT_MBR
    && format->mbr.dos_compatibility;
  rtems_blkdev_bnum disk_end = 0;
  rtems_blkdev_bnum pos = 0;
  rtems_blkdev_bnum dist_sum = 0;
  rtems_blkdev_bnum record_space =
    dos_compatibility ? RTEMS_BDPART_MBR_CYLINDER_SIZE : 1;
  rtems_blkdev_bnum overhead = 0;
  rtems_blkdev_bnum free_space = 0;
  size_t i = 0;

  /* Check if we have something to do */
  if (count == 0) {
    /* Nothing to do */
    return RTEMS_SUCCESSFUL;
  }

  /* Check parameter */
  if (format == NULL || pt == NULL || dist == NULL) {
    return RTEMS_INVALID_ADDRESS;
  }

  /* Get disk data */
  sc = rtems_bdpart_get_disk_data( disk_name, NULL, NULL, &disk_end);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  /* Get distribution sum and check for overflow */
  for (i = 0; i < count; ++i) {
    unsigned prev_sum = dist_sum;

    dist_sum += dist [i];

    if (dist_sum < prev_sum) {
      return RTEMS_INVALID_NUMBER;
    }

    if (dist [i] == 0) {
      return RTEMS_INVALID_NUMBER;
    }
  }

  /* Check format */
  if (format->type != RTEMS_BDPART_FORMAT_MBR) {
    return RTEMS_NOT_IMPLEMENTED;
  }

  /* Align end of disk on cylinder boundary if necessary */
  if (dos_compatibility) {
    disk_end -= (disk_end % record_space);
  }

  /*
   * We need at least space for the MBR and the compatibility space for the
   * first primary partition.
   */
  overhead += record_space;

  /*
   * In case we need an extended partition and logical partitions we have to
   * account for the space of each EBR.
   */
  if (count > 4) {
    overhead += (count - 3) * record_space;
  }

  /*
   * Account space to align every partition on cylinder boundaries if
   * necessary.
   */
  if (dos_compatibility) {
    overhead += (count - 1) * record_space;
  }

  /* Check disk space */
  if ((overhead + count) > disk_end) {
    return RTEMS_IO_ERROR;
  }

  /* Begin of first primary partition */
  pos = record_space;

  /* Space for partitions */
  free_space = disk_end - overhead;

  for (i = 0; i < count; ++i) {
    rtems_bdpart_partition *p = pt + i;

    /* Partition size */
    rtems_blkdev_bnum s = free_space * dist [i];
    if (s < free_space || s < dist [i]) {
      /* TODO: Calculate without overflow */
      return RTEMS_INVALID_NUMBER;
    }
    s /= dist_sum;

    /* Ensure that the partition is not empty */
    if (s == 0) {
      s = 1;
    }

    /* Align partition upwards */
    s += record_space - (s % record_space);

    /* Reserve space for the EBR if necessary */
    if (count > 4 && i > 2) {
      pos += record_space;
    }

    /* Partition begin and end */
    p->begin = pos;
    pos += s;
    p->end = pos;
  }

  /* Expand the last partition to the disk end */
  pt [count - 1].end = disk_end;

  return RTEMS_SUCCESSFUL;
}
