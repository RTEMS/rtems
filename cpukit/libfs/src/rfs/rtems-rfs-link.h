/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
/**
 * @file
 *
 * @ingroup rtems-rfs
 *
 * RTEMS File System Link Support
 *
 * This file provides the link support functions.
 */

#if !defined (_RTEMS_RFS_LINK_H_)
#define _RTEMS_RFS_LINK_H_

#include <dirent.h>

#include <rtems/rfs/rtems-rfs-file-system.h>
#include <rtems/rfs/rtems-rfs-inode.h>

/**
 * Create a link.
 *
 * @param fs The file system.
 * @param name The name of the link.
 * @param length The length of the name.
 * @param parent The inode number of the parent directory.
 * @param target The inode of the target.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_link (rtems_rfs_file_system* fs,
                    const char*            name,
                    int                    length,
                    rtems_rfs_ino          parent,
                    rtems_rfs_ino          target);

/**
 * Unlink the node from the parent directory. A directory offset for the target
 * entry is required because links cause a number of inode numbers to appear in
 * a single directory so scanning does not work.
 *
 * @param fs The file system.
 * @param parent The inode number of the parent directory.
 * @param target The inode of the target.
 * @param doff Parent directory entry offset for the target entry.
 * @param dir If true unlinking of directory nodes is allowed.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_unlink (rtems_rfs_file_system* fs,
                      rtems_rfs_ino          parent,
                      rtems_rfs_ino          target,
                      uint32_t               doff,
                      bool                   dir);

/**
 * Symbolic link is an inode that has a path attached.
 *
 * @param fs The file system data.
 * @param name The name of the node.
 * @param length The length of the name of the node.
 * @param link The link path attached to the symlink inode.
 * @param link_length The length of the link path.
 * @param parent The parent inode number.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_symlink (rtems_rfs_file_system* fs,
                       const char*            name,
                       int                    length,
                       const char*            link,
                       int                    link_length,
                       uid_t                  uid,
                       gid_t                  gid,
                       rtems_rfs_ino          parent);

/**
 * Read a symbolic link into the provided buffer returning the link of link
 * name.
 *
 * @param fs The file system data.
 * @param link The link inode number to read.
 * @param path The buffer to write the link path into.
 * @param size The size of the buffer.
 * @param length Set to the length of the link path.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_symlink_read (rtems_rfs_file_system* fs,
                            rtems_rfs_ino          link,
                            char*                  path,
                            size_t                 size,
                            size_t*                length);

#endif
