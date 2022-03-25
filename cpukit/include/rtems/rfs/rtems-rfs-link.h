/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief RTEMS File System Link Support
 *
 * @ingroup rtems_rfs
 *
 * RTEMS File System Link Support
 *
 * This file provides the link support functions.
 */

/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
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


#if !defined (_RTEMS_RFS_LINK_H_)
#define _RTEMS_RFS_LINK_H_

#include <dirent.h>

#include <rtems/rfs/rtems-rfs-file-system.h>
#include <rtems/rfs/rtems-rfs-inode.h>

/**
 * Directory unlink modes.
 */
typedef enum rtems_rfs_unlink_dir_e
{
  rtems_rfs_unlink_dir_denied,   /**< Not allowed to unlink a directory. */
  rtems_rfs_unlink_dir_if_empty, /**< Unlink if the directory is empty. */
  rtems_rfs_unlink_dir_allowed   /**< Unlinking of directories is allowed. */
} rtems_rfs_unlink_dir;

/**
 * Create a link. Do not link directories unless renaming or you will create
 * loops in the file system.
 *
 * @param[in] fs is the file system.
 * @param[in] name is a pointer to the name of the link.
 * @param[in] length is the length of the name.
 * @param[in] parent is the inode number of the parent directory.
 * @param[in] target is the inode of the target.
 * @param[in] link_dir If true directories can be linked. Useful when
 *                renaming.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
 */
int rtems_rfs_link (rtems_rfs_file_system* fs,
                    const char*            name,
                    int                    length,
                    rtems_rfs_ino          parent,
                    rtems_rfs_ino          target,
                    bool                   link_dir);

/**
 * Unlink the node from the parent directory. A directory offset for the
 * target entry is required because links cause a number of inode numbers to
 * appear in a single directory so scanning does not work.
 *
 * @param[in] fs is the file system.
 * @param[in] parent is the inode number of the parent directory.
 * @param[in] target is the inode of the target.
 * @param[in] doff is the parent directory entry offset for the target entry.
 * @param[in] dir_mode is the directory unlink mode.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
 */
int rtems_rfs_unlink (rtems_rfs_file_system* fs,
                      rtems_rfs_ino          parent,
                      rtems_rfs_ino          target,
                      uint32_t               doff,
                      rtems_rfs_unlink_dir   dir_mode);

/**
 * Symbolic link is an inode that has a path attached.
 *
 * @param[in] fs is the file system data.
 * @param[in] name is a pointer to the name of the node.
 * @param[in] length is the length of the name of the node.
 * @param[in] link is a pointer to the link path attached to the
 *             symlink inode.
 * @param[in] link_length is the length of the link path.
 * @param[in] parent is the parent inode number.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
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
 * @param[in] fs is the file system data.
 * @param[in] link is the link inode number to read.
 * @param[in] path is a pointer to the buffer to write the link path into.
 * @param[in] size is the size of the buffer.
 * @param[out] length will contain the length of the link path.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
 */
int rtems_rfs_symlink_read (rtems_rfs_file_system* fs,
                            rtems_rfs_ino          link,
                            char*                  path,
                            size_t                 size,
                            size_t*                length);

#endif
