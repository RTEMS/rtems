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
#include <rtems/libio.h>

rtems_status_code rtems_bdpart_mount(
  const char *disk_name,
  const rtems_bdpart_partition *pt __attribute__((unused)),
  size_t count,
  const char *mount_base
)
{
  rtems_status_code esc = RTEMS_SUCCESSFUL;
  const char *disk_file_name = strrchr( disk_name, '/');
  char *logical_disk_name = NULL;
  char *logical_disk_marker = NULL;
  char *mount_point = NULL;
  char *mount_marker = NULL;
  size_t disk_file_name_size = 0;
  size_t disk_name_size = strlen( disk_name);
  size_t mount_base_size = strlen( mount_base);
  size_t i = 0;

  /* Create logical disk name base */
  logical_disk_name = malloc( disk_name_size + RTEMS_BDPART_NUMBER_SIZE);
  if (logical_disk_name == NULL) {
    return RTEMS_NO_MEMORY;
  }
  strncpy( logical_disk_name, disk_name, disk_name_size);

  /* Get disk file name */
  if (disk_file_name != NULL) {
    disk_file_name += 1;
    disk_file_name_size = strlen( disk_file_name);
  } else {
    disk_file_name = disk_name;
    disk_file_name_size = disk_name_size;
  }

  /* Create mount point base */
  mount_point = malloc( mount_base_size + 1 + disk_file_name_size + RTEMS_BDPART_NUMBER_SIZE);
  if (mount_point == NULL) {
    esc = RTEMS_NO_MEMORY;
    goto cleanup;
  }
  strncpy( mount_point, mount_base, mount_base_size);
  mount_point [mount_base_size] = '/';
  strncpy( mount_point + mount_base_size + 1, disk_file_name, disk_file_name_size);

  /* Markers */
  logical_disk_marker = logical_disk_name + disk_name_size;
  mount_marker = mount_point + mount_base_size + 1 + disk_file_name_size;

  /* Mount supported file systems for each partition */
  for (i = 0; i < count; ++i) {
    /* Create logical disk name */
    int rv = snprintf( logical_disk_marker, RTEMS_BDPART_NUMBER_SIZE, "%zu", i + 1);
    if (rv >= RTEMS_BDPART_NUMBER_SIZE) {
      esc = RTEMS_INVALID_NAME;
      goto cleanup;
    }

    /* Create mount point */
    strncpy( mount_marker, logical_disk_marker, RTEMS_BDPART_NUMBER_SIZE);
    rv = rtems_mkdir( mount_point, S_IRWXU | S_IRWXG | S_IRWXO);
    if (rv != 0) {
      esc = RTEMS_IO_ERROR;
      goto cleanup;
    }

    /* Mount */
    rv = mount(
      logical_disk_name,
      mount_point,
      "msdos",
      0,
      NULL
    );
    if (rv != 0) {
      rmdir( mount_point);
    }
  }

cleanup:

  free( logical_disk_name);
  free( mount_point);

  return esc;
}

rtems_status_code rtems_bdpart_unmount(
  const char *disk_name,
  const rtems_bdpart_partition *pt __attribute__((unused)),
  size_t count,
  const char *mount_base
)
{
  rtems_status_code esc = RTEMS_SUCCESSFUL;
  const char *disk_file_name = strrchr( disk_name, '/');
  char *mount_point = NULL;
  char *mount_marker = NULL;
  size_t disk_file_name_size = 0;
  size_t disk_name_size = strlen( disk_name);
  size_t mount_base_size = strlen( mount_base);
  size_t i = 0;

  /* Get disk file name */
  if (disk_file_name != NULL) {
    disk_file_name += 1;
    disk_file_name_size = strlen( disk_file_name);
  } else {
    disk_file_name = disk_name;
    disk_file_name_size = disk_name_size;
  }

  /* Create mount point base */
  mount_point = malloc( mount_base_size + 1 + disk_file_name_size + RTEMS_BDPART_NUMBER_SIZE);
  if (mount_point == NULL) {
    esc = RTEMS_NO_MEMORY;
    goto cleanup;
  }
  strncpy( mount_point, mount_base, mount_base_size);
  mount_point [mount_base_size] = '/';
  strncpy( mount_point + mount_base_size + 1, disk_file_name, disk_file_name_size);

  /* Marker */
  mount_marker = mount_point + mount_base_size + 1 + disk_file_name_size;

  /* Mount supported file systems for each partition */
  for (i = 0; i < count; ++i) {
    /* Create mount point */
    int rv = snprintf( mount_marker, RTEMS_BDPART_NUMBER_SIZE, "%zu", i + 1);
    if (rv >= RTEMS_BDPART_NUMBER_SIZE) {
      esc = RTEMS_INVALID_NAME;
      goto cleanup;
    }

    /* Unmount */
    rv = unmount( mount_point);
    if (rv == 0) {
      /* Remove mount point */
      rv = rmdir( mount_point);
      if (rv != 0) {
        esc = RTEMS_IO_ERROR;
        goto cleanup;
      }
    }
  }

cleanup:

  free( mount_point);

  return esc;
}
