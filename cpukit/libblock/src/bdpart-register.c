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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rtems.h>
#include <rtems/bdpart.h>

rtems_status_code rtems_bdpart_register(
  const char *disk_name,
  const rtems_bdpart_partition *pt,
  size_t count
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_status_code esc = RTEMS_SUCCESSFUL;
  rtems_device_major_number major = 0;
  rtems_device_minor_number minor = 0;
  rtems_blkdev_bnum disk_end = 0;
  dev_t disk = 0;
  dev_t logical_disk = 0;
  char *logical_disk_name = NULL;
  char *logical_disk_marker = NULL;
  size_t disk_name_size = strlen( disk_name);
  size_t i = 0;
  int fd = -1;
  rtems_disk_device *dd = NULL;

  /* Get disk data */
  sc = rtems_bdpart_get_disk_data( disk_name, &fd, &dd, &disk_end);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }
  disk = rtems_disk_get_device_identifier( dd);
  close( fd);

  /* Get the disk device identifier */
  rtems_filesystem_split_dev_t( disk, major, minor);

  /* Create logical disk name */
  logical_disk_name = malloc( disk_name_size + RTEMS_BDPART_NUMBER_SIZE);
  if (logical_disk_name == NULL) {
    return RTEMS_NO_MEMORY;
  }
  strncpy( logical_disk_name, disk_name, disk_name_size);
  logical_disk_marker = logical_disk_name + disk_name_size;

  /* Create a logical disk for each partition */
  for (i = 0; i < count; ++i) {
    const rtems_bdpart_partition *p = pt + i;
    int rv = 0;

    /* New minor number */
    ++minor;

    /* Create a new device identifier */
    logical_disk = rtems_filesystem_make_dev_t( major, minor);

    /* Set partition number for logical disk name */
    rv = snprintf( logical_disk_marker, RTEMS_BDPART_NUMBER_SIZE, "%zu", i + 1);
    if (rv >= RTEMS_BDPART_NUMBER_SIZE) {
      esc = RTEMS_INVALID_NAME;
      goto cleanup;
    }

    /* Create logical disk */
    sc = rtems_disk_create_log(
      logical_disk,
      disk,
      p->begin,
      p->end - p->begin,
      logical_disk_name
    );
    if (sc != RTEMS_SUCCESSFUL) {
      esc = sc;
      goto cleanup;
    }
  }

cleanup:

  free( logical_disk_name);

  return esc;
}

rtems_status_code rtems_bdpart_register_from_disk( const char *disk_name)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_bdpart_format format;
  rtems_bdpart_partition pt [RTEMS_BDPART_PARTITION_NUMBER_HINT];
  size_t count = RTEMS_BDPART_PARTITION_NUMBER_HINT;

  /* Read partitions */
  sc = rtems_bdpart_read( disk_name, &format, pt, &count);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  /* Register partitions */
  return rtems_bdpart_register( disk_name, pt, count);
}

rtems_status_code rtems_bdpart_unregister(
  const char *disk_name,
  const rtems_bdpart_partition *pt __attribute__((unused)),
  size_t count
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_device_major_number major = 0;
  rtems_device_minor_number minor = 0;
  rtems_blkdev_bnum disk_end = 0;
  dev_t disk = 0;
  dev_t logical_disk = 0;
  size_t i = 0;
  int fd = -1;
  rtems_disk_device *dd = NULL;

  /* Get disk data */
  sc = rtems_bdpart_get_disk_data( disk_name, &fd, &dd, &disk_end);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }
  disk = rtems_disk_get_device_identifier( dd);
  close( fd);

  /* Get the disk device identifier */
  rtems_filesystem_split_dev_t( disk, major, minor);

  /* Create a logical disk for each partition */
  for (i = 0; i < count; ++i) {
    /* New minor number */
    ++minor;

    /* Get the device identifier */
    logical_disk = rtems_filesystem_make_dev_t( major, minor);

    /* Delete logical disk */
    sc = rtems_disk_delete( logical_disk);
    if (sc != RTEMS_SUCCESSFUL) {
      return sc;
    }
  }

  return RTEMS_SUCCESSFUL;
}
