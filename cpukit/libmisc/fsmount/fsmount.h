/**
 * @file
 *
 * File system mount functions.
 */

/*===============================================================*\
| Project: RTEMS fsmount                                          |
+-----------------------------------------------------------------+
| File: fsmount.h                                                 |
+-----------------------------------------------------------------+
|                    Copyright (c) 2003 IMD                       |
|      Ingenieurbuero fuer Microcomputertechnik Th. Doerfler      |
|               <Thomas.Doerfler@imd-systems.de>                  |
|                       all rights reserved                       |
+-----------------------------------------------------------------+
| this file contains the fsmount functions. These functions       |
| are used to mount a list of filesystems (and create their mount |
| points before)                                                  |
|                                                                 |
|  The license and distribution terms for this file may be        |
|  found in the file LICENSE in this distribution or at           |
|  http://www.rtems.com/license/LICENSE.                     |
|                                                                 |
+-----------------------------------------------------------------+
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 02.07.03  creation                                         doe  |
\*===============================================================*/

#ifndef _FSMOUNT_H
#define _FSMOUNT_H

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup rtems_fstab File System Mount Support
 *
 * @{
 */

/**
 * File system mount report and abort condition flags.
 *
 * The flags define, which conditions will cause a report during the mount
 * process (via printf()) or abort the mount process.
 *
 * @see rtems_fstab_entry and rtems_fsmount().
 */
typedef enum {
  /**
   * No conditions.
   */
  RTEMS_FSTAB_NONE = 0U,

  /**
   * Complete mount process was successful.
   */
  RTEMS_FSTAB_OK = 0x1U,

  /**
   * Mount point creation failed.
   */
  RTEMS_FSTAB_ERROR_MOUNT_POINT = 0x2U,

  /**
   * File system mount failed.
   */
  RTEMS_FSTAB_ERROR_MOUNT = 0x4U,

  /**
   * Something failed.
   */
  RTEMS_FSTAB_ERROR = RTEMS_FSTAB_ERROR_MOUNT_POINT | RTEMS_FSTAB_ERROR_MOUNT,

  /**
   * Any condition.
   */
  RTEMS_FSTAB_ANY = RTEMS_FSTAB_OK | RTEMS_FSTAB_ERROR
} rtems_fstab_conditions;

/**
 * File system table entry.
 */
typedef struct {
  /**
   * Source for the mount.
   */
  const char *source;

  /**
   * Target for the mount.
   */
  const char *target;

  /**
   * File system operations.
   */
  const char *type;

  /**
   * File system mount options.
   */
  rtems_filesystem_options_t options;

  /**
   * Report @ref rtems_fstab_conditions "condition flags".
   */
  uint16_t report_reasons;

  /**
   * Abort @ref rtems_fstab_conditions "condition flags".
   */
  uint16_t abort_reasons;
} rtems_fstab_entry;

/**
 * Mounts the file systems listed in the file system mount table @a fstab of
 * size @a size.
 *
 * Each file system will be mounted according to its table entry parameters.
 * In case of an abort condition the corresponding table index will be reported
 * in @a abort_index.  The pointer @a abort_index may be @c NULL.  The mount
 * point paths will be created with rtems_mkdir() and need not exist
 * beforehand.
 *
 * On success, zero is returned.  On error, -1 is returned, and @c errno is set
 * appropriately.
 *
 * @see rtems_bdpart_register_from_disk().
 *
 * The following example code tries to mount a FAT file system within a SD
 * Card.  Some cards do not have a partition table so at first it tries to find
 * a file system inside the hole disk.  If this is successful the mount process
 * will be aborted because the @ref RTEMS_FSTAB_OK condition is true.  If this
 * did not work it tries to mount the file system inside the first partition.
 * If this fails the mount process will not be aborted (this is already the
 * last entry), but the last error status will be returned.
 *
 * @code
 * #include <stdio.h>
 * #include <string.h>
 * #include <errno.h>
 *
 * #include <rtems.h>
 * #include <rtems/bdpart.h>
 * #include <rtems/error.h>
 * #include <rtems/fsmount.h>
 *
 * static const rtems_fstab_entry fstab [] = {
 *   {
 *     .source = "/dev/sd-card-a",
 *     .target = "/mnt",
 *     .type = "dosfs",
 *     .options = RTEMS_FILESYSTEM_READ_WRITE,
 *     .report_reasons = RTEMS_FSTAB_ANY,
 *     .abort_reasons = RTEMS_FSTAB_OK
 *   }, {
 *     .source = "/dev/sd-card-a1",
 *     .target = "/mnt",
 *     .type = "dosfs",
 *     .options = RTEMS_FILESYSTEM_READ_WRITE,
 *     .report_reasons = RTEMS_FSTAB_ANY,
 *     .abort_reasons = RTEMS_FSTAB_NONE
 *   }
 * };
 *
 * static void my_mount(void)
 * {
 *   rtems_status_code sc = RTEMS_SUCCESSFUL;
 *   int rv = 0;
 *   size_t abort_index = 0;
 *
 *   sc = rtems_bdpart_register_from_disk("/dev/sd-card-a");
 *   if (sc != RTEMS_SUCCESSFUL) {
 *     printf("read partition table failed: %s\n", rtems_status_text(sc));
 *   }
 *
 *   rv = rtems_fsmount(fstab, sizeof(fstab) / sizeof(fstab [0]), &abort_index);
 *   if (rv != 0) {
 *     printf("mount failed: %s\n", strerror(errno));
 *   }
 *   printf("mount aborted at %zu\n", abort_index);
 * }
 * @endcode
 */
int rtems_fsmount( const rtems_fstab_entry *fstab, size_t size, size_t *abort_index);

/** @} */

typedef rtems_fstab_entry fstab_t;

#define FSMOUNT_MNT_OK RTEMS_FSTAB_OK

#define FSMOUNT_MNTPNT_CRTERR RTEMS_FSTAB_ERROR_MOUNT_POINT

#define FSMOUNT_MNT_FAILED RTEMS_FSTAB_ERROR_MOUNT

#ifdef __cplusplus
}
#endif

#endif /* _FSMOUNT_H */
