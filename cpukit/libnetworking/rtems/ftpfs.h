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
 * http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#ifndef _RTEMS_FTPFS_H
#define _RTEMS_FTPFS_H

#include <rtems/libio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup rtems_ftpfs File Transfer Protocol File System
 *
 * The FTP file system (FTP client) can be used to transfer files from or to
 * remote hosts.
 *
 * You can mount the FTP file system with a call to rtems_ftpfs_mount().
 * Alternatively you can use mount() with the @ref rtems_ftpfs_ops operations
 * table.
 *
 * You can open files either read-only or write-only.  A seek is not allowed.
 * A close terminates the control and data connections.
 *
 * To open a file @c file.txt in the directory @c dir (relative to home
 * directory of the server) on a server named @c host using the user name
 * @c user and the password @c pw you must specify the following path:
 *
 * @c /FTP/user:pw@host/dir/file.txt
 *
 * If the server is the default server specified in BOOTP, it can be ommitted:
 *
 * @c /FTP/user:pw/dir/file.txt
 *
 * The user name will be used for the password if it is ommitted:
 *
 * @c /FTP/user@host/dir/file.txt
 *
 * For the data transfer passive (= default) and active (= fallback) mode are
 * supported.
 *
 * @{
 */

/**
 * @brief Well-known port number for FTP control connection.
 */
#define RTEMS_FTPFS_CTRL_PORT 21

/**
 * @brief Default mount point for FTP file system.
 */
#define RTEMS_FTPFS_MOUNT_POINT_DEFAULT "/FTP"

/**
 * @brief FTP file system operations table.
 */
extern const rtems_filesystem_operations_table rtems_ftpfs_ops;

/**
 * @brief Creates the mount point @a mount_point and mounts the FTP file
 * system.
 *
 * If @a mount_point is @c NULL the default mount point
 * @ref RTEMS_FTPFS_MOUNT_POINT_DEFAULT will be used.
 *
 * It is mounted with read and write access.
 *
 * @note The parent directories of the mount point have to exist.
 */
rtems_status_code rtems_ftpfs_mount( const char *mount_point);

/**
 * @brief Enables or disables the verbose mode if @a verbose is @c true or
 * @c false respectively.
 *
 * In the enabled verbose mode the commands and replies of the FTP control
 * connections will be printed to standard error.
 */
rtems_status_code rtems_ftpfs_set_verbose( bool verbose);

/**
 * @brief Returns in @a verbose if the verbose mode is enabled or disabled.
 */
rtems_status_code rtems_ftpfs_get_verbose( bool *verbose);

/** @} */

/**
 * @brief Creates the default mount point @ref RTEMS_FTPFS_MOUNT_POINT_DEFAULT
 * and mounts the FTP file system.
 *
 * It is mounted with read and write access.
 *
 * On success, zero is returned.  On error, -1 is returned.
 *
 * @deprecated Use rtems_ftpfs_mount() instead.
 */
int rtems_bsdnet_initialize_ftp_filesystem( void);

#ifdef __cplusplus
}
#endif

#endif
