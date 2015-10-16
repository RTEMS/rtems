/**
 * @file
 *
 * @brief Block Device Partition Management
 * @ingroup rtems_bdpart
 */

/*
 * Copyright (c) 2009, 2012 embedded brains GmbH.  All rights reserved.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/bdpart.h>

static char *create_logical_disk_name( const char *disk_name, char **marker)
{
  size_t disk_name_size = strlen( disk_name);
  char *logical_disk_name = malloc( disk_name_size + RTEMS_BDPART_NUMBER_SIZE);

  if (logical_disk_name != NULL) {
    strncpy( logical_disk_name, disk_name, disk_name_size);
    *marker = logical_disk_name + disk_name_size;
  }

  return logical_disk_name;
}

static rtems_status_code update_logical_disk_name(
  char *logical_disk_marker,
  size_t i
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  int rv = 0;

  rv = snprintf( logical_disk_marker, RTEMS_BDPART_NUMBER_SIZE, "%zu", i + 1);
  if (rv >= RTEMS_BDPART_NUMBER_SIZE) {
    sc = RTEMS_INVALID_NAME;
  }

  return sc;
}

rtems_status_code rtems_bdpart_register(
  const char *disk_name,
  const rtems_bdpart_partition *pt,
  size_t count
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_status_code esc = RTEMS_SUCCESSFUL;
  rtems_blkdev_bnum disk_end = 0;
  char *logical_disk_name = NULL;
  char *logical_disk_marker = NULL;
  size_t i = 0;
  int fd = -1;
  rtems_disk_device *dd = NULL;

  /* Get disk data */
  sc = rtems_bdpart_get_disk_data( disk_name, &fd, &dd, &disk_end);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  /* Create logical disk name */
  logical_disk_name = create_logical_disk_name(
    disk_name,
    &logical_disk_marker
  );
  if (logical_disk_name == NULL) {
    esc = sc;
    goto cleanup;
  }

  /* Create a logical disk for each partition */
  for (i = 0; i < count; ++i) {
    const rtems_bdpart_partition *p = pt + i;

    /* Set partition number for logical disk name */
    sc = update_logical_disk_name( logical_disk_marker, i);
    if (sc != RTEMS_SUCCESSFUL) {
      esc = sc;
      goto cleanup;
    }

    /* Create logical disk */
    sc = rtems_blkdev_create_partition(
      logical_disk_name,
      disk_name,
      p->begin,
      p->end - p->begin
    );
    if (sc != RTEMS_SUCCESSFUL) {
      esc = sc;
      goto cleanup;
    }
  }

cleanup:

  free( logical_disk_name);
  close( fd);

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
  const rtems_bdpart_partition *pt RTEMS_UNUSED,
  size_t count
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_status_code esc = RTEMS_SUCCESSFUL;
  rtems_blkdev_bnum disk_end = 0;
  char *logical_disk_name = NULL;
  char *logical_disk_marker = NULL;
  size_t i = 0;
  int fd = -1;
  rtems_disk_device *dd = NULL;

  /* Get disk data */
  sc = rtems_bdpart_get_disk_data( disk_name, &fd, &dd, &disk_end);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  /* Create logical disk name */
  logical_disk_name = create_logical_disk_name(
    disk_name,
    &logical_disk_marker
  );
  if (logical_disk_name == NULL) {
    esc = sc;
    goto cleanup;
  }

  /* Delete the logical disk for each partition */
  for (i = 0; i < count; ++i) {
    int rv = 0;

    /* Set partition number for logical disk name */
    sc = update_logical_disk_name( logical_disk_marker, i);
    if (sc != RTEMS_SUCCESSFUL) {
      esc = sc;
      goto cleanup;
    }

    /* Delete logical disk */
    rv = unlink( logical_disk_name);
    if (rv != 0) {
      esc = sc;
      goto cleanup;
    }
  }

cleanup:

  free( logical_disk_name);
  close( fd);

  return esc;
}
