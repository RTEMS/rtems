/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_bdpart
 *
 * @brief Block Device Partition Management
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
    /* The string is NUL terminated by (A) ... */
    memcpy( logical_disk_name, disk_name, disk_name_size);
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

  /* ... (A) */
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
