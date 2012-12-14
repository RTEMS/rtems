/**
 *  @file rtems/dosfs.h
 *
 *  Application interface to MSDOS filesystem.
 */

/*
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_DOSFS_H
#define _RTEMS_DOSFS_H

#include <rtems.h>
#include <rtems/libio.h>

#ifdef __cplusplus
extern "C" {
#endif

int rtems_dosfs_initialize(rtems_filesystem_mount_table_entry_t *mt_entry,
                           const void                           *data);

/**
 * @defgroup rtems_msdos_format DOSFS Support
 *
 * @ingroup FileSystemTypesAndMount
 *
 * @{
 */

#define MSDOS_FMT_INFO_LEVEL_NONE   (0)
#define MSDOS_FMT_INFO_LEVEL_INFO   (1)
#define MSDOS_FMT_INFO_LEVEL_DETAIL (2)
#define MSDOS_FMT_INFO_LEVEL_DEBUG  (3)

/**
 * @brief FAT file system format request parameters.
 */
typedef struct {
  /**
   * @brief OEM name string or NULL.
   */
  const char *OEMName;

  /**
   * @brief Volume label string or NULL.
   */
  const char *VolLabel;

  /**
   * @brief Sectors per cluster hint.
   *
   * The format procedure may choose another value.  Use 0 as default value.
   */
  uint32_t sectors_per_cluster;

  /**
   * @brief Number of FATs hint.
   *
   * Use 0 as default value.
   */
  uint32_t fat_num;

  /**
   * @brief Minimum files in root directory for FAT12 and FAT16.
   *
   * The format procedure may choose a greater value.  Use 0 as default value.
   */
  uint32_t files_per_root_dir;

  /**
   * @brief Media code.
   *
   * Use 0 as default value.  The default media code is 0xf8.
   */
  uint8_t media;

  /**
   * @brief Quick format.
   *
   * If set to true, then do not clear data sectors to zero.
   */
  bool quick_format;

  /**
   * @brief Do not align FAT, data cluster, and root directory to a cluster
   * boundary.
   */
  bool skip_alignment;

  /**
   * @brief Synchronize device after write operations.
   */
  bool sync_device;

  /**
   * @brief The amount of info to output.
   */
  int info_level;
} msdos_format_request_param_t;

/**
 * @brief Formats a block device with a FAT file system.
 *
 * @param[in] devname The block device path.
 * @param[in] rqdata The FAT file system format request data.  Use NULL for
 * default parameters.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occurred.  The @c errno indicates the error.
 */
int msdos_format (
  const char *devname,
  const msdos_format_request_param_t *rqdata
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
