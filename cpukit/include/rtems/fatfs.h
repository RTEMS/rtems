/**
 * @file
 *
 * @ingroup FatFS
 *
 * @brief RTEMS FatFS Filesystem Public API
 */

/*
 Copyright (C) 2025 Sepehr Ganji <sepehrganji79@gmail.com>
*/

#ifndef _RTEMS_FATFS_H
#define _RTEMS_FATFS_H

#include <rtems.h>
#include <rtems/libio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup FATFS
 *
 * @{
 */

/**
 * @brief RTEMS FatFS filesystem mount options.
 */
typedef struct {
  /** Currently no options defined */
  int reserved;
} rtems_fatfs_mount_options;

/**
 * @brief Initialize FatFS filesystem.
 *
 * This function is the mount handler for the FatFS filesystem. It is called
 * by the mount() system call when the filesystem type is
 * RTEMS_FILESYSTEM_TYPE_FATFS.
 *
 * @param[in] mt_entry The mount table entry.
 * @param[in] data Mount options (rtems_fatfs_mount_options or NULL).
 *
 * @return 0 on success, -1 on error with errno set.
 */
int rtems_fatfs_initialize(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_FATFS_H */
