/**
 * @file
 *
 * @brief File Transfer Protocol file system (FTP client).
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * (c) Copyright 2002
 * Thomas Doerfler
 * IMD Ingenieurbuero fuer Microcomputertechnik
 * Herbststr. 8
 * 82178 Puchheim, Germany
 * <Thomas.Doerfler@imd-systems.de>
 *
 * Modified by Sebastian Huber <sebastian.huber@embedded-brains.de>.
 *
 * This code has been created after closly inspecting "tftpdriver.c" from Eric
 * Norum.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_FTPFS_H
#define _RTEMS_FTPFS_H

#include <sys/time.h>
#include <sys/ioctl.h>

#include <rtems/libio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup rtems_ftpfs File Transfer Protocol File System
 *
 * @brief The FTP file system (FTP client) can be used to transfer files from
 * or to remote hosts.
 *
 * You can mount the FTP file system with a call to mount() or
 * mount_and_make_target_path() with the @ref RTEMS_FILESYSTEM_TYPE_FTPFS file
 * system type.
 *
 * You have to add @ref CONFIGURE_FILESYSTEM_FTPFS to your application
 * configuration.
 *
 * You can open files either read-only or write-only.  A seek is not allowed.
 * A close terminates the control and data connections.
 *
 * To open a file @c file.txt in the directory @c dir (relative to home
 * directory of the server) on a server named @c host using the user name
 * @c user and the password @c pw you must specify the following path:
 * <tt>/FTP/user:pw@@host/dir/file.txt</tt>.
 *
 * If the server is the default server specified in BOOTP, it can be ommitted:
 * <tt>/FTP/user:pw/dir/file.txt</tt>.
 *
 * The user name will be used for the password if it is ommitted:
 * <tt>/FTP/user@@host/dir/file.txt</tt>.
 *
 * For the data transfer passive (= default) and active (= fallback) mode are
 * supported.
 */
/**@{**/

/**
 * @brief Well-known port number for FTP control connection.
 */
#define RTEMS_FTPFS_CTRL_PORT 21

/**
 * @brief Default mount point for FTP file system.
 */
#define RTEMS_FTPFS_MOUNT_POINT_DEFAULT "/FTP"

/**
 * @brief FTP file system IO control requests.
 */
typedef enum {
  RTEMS_FTPFS_IOCTL_GET_VERBOSE = _IOR( 'd', 1, bool *),
  RTEMS_FTPFS_IOCTL_SET_VERBOSE = _IOW( 'd', 1, bool *),
  RTEMS_FTPFS_IOCTL_GET_TIMEOUT = _IOR( 'd', 2, struct timeval *),
  RTEMS_FTPFS_IOCTL_SET_TIMEOUT = _IOW( 'd', 2, struct timeval *)
} rtems_ftpfs_ioctl_numbers;

/**
 * @brief Returns in @a verbose if the verbose mode is enabled or disabled for
 * the file system at @a mount_point.
 *
 * If @a mount_point is @c NULL the default mount point
 * @ref RTEMS_FTPFS_MOUNT_POINT_DEFAULT will be used.
 */
rtems_status_code rtems_ftpfs_get_verbose( const char *mount_point, bool *verbose);

/**
 * @brief Enables or disables the verbose mode if @a verbose is @c true or
 * @c false respectively for the file system at @a mount_point.
 *
 * In the enabled verbose mode the commands and replies of the FTP control
 * connections will be printed to standard error.
 *
 * If @a mount_point is @c NULL the default mount point
 * @ref RTEMS_FTPFS_MOUNT_POINT_DEFAULT will be used.
 */
rtems_status_code rtems_ftpfs_set_verbose( const char *mount_point, bool verbose);

/**
 * @brief Returns the current timeout value in @a timeout for the file system
 * at @a mount_point.
 *
 * If @a mount_point is @c NULL the default mount point
 * @ref RTEMS_FTPFS_MOUNT_POINT_DEFAULT will be used.
 */
rtems_status_code rtems_ftpfs_get_timeout(
  const char *mount_point,
  struct timeval *timeout
);

/**
 * @brief Sets the timeout value to @a timeout for the file system at
 * @a mount_point.
 *
 * The timeout value will be used during connection establishment of active
 * data connections.  It will be also used for send and receive operations on
 * data and control connections.
 *
 * If @a mount_point is @c NULL the default mount point
 * @ref RTEMS_FTPFS_MOUNT_POINT_DEFAULT will be used.
 */
rtems_status_code rtems_ftpfs_set_timeout(
  const char *mount_point,
  const struct timeval *timeout
);

/** @} */

/**
 * @brief Do not call directly, use mount().
 */
int rtems_ftpfs_initialize(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void *data
);

#ifdef __cplusplus
}
#endif

#endif
