/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems-rfs
 *
 * RTEMS File System Directory Support
 *
 * This file provides the directory support functions.
 */

#if !defined (_RTEMS_RFS_DIR_H_)
#define _RTEMS_RFS_DIR_H_

#include <dirent.h>

#include <rtems/libio_.h>

#include <rtems/rfs/rtems-rfs-data.h>
#include <rtems/rfs/rtems-rfs-file-system.h>
#include <rtems/rfs/rtems-rfs-inode.h>

/**
 * Define the offsets of the fields of a directory entry.
 */
#define RTEMS_RFS_DIR_ENTRY_INO  (0) /**< The ino offset in a directory
                                      * entry. */
#define RTEMS_RFS_DIR_ENTRY_HASH (4) /**< The hash offset in a directory
                                      * entry. The hash is 32bits. We need at
                                      * least 16bits and given the length and
                                      * ino field are 4 the extra 2 bytes is
                                      * not a big overhead.*/
#define RTEMS_RFS_DIR_ENTRY_LEN  (8) /**< The length offset in a directory
                                      * entry. */

/**
 * The length of the directory entry header.
 */
#define RTEMS_RFS_DIR_ENTRY_SIZE (4 + 4 + 2)

/**
 * The length when the remainder of the directory block is empty.
 */
#define RTEMS_RFS_DIR_ENTRY_EMPTY (0xffff)

/**
 * Return the hash of the entry.
 *
 * @param _e Pointer to the directory entry.
 * @return uint32_t The hash.
 */
#define rtems_rfs_dir_entry_hash(_e) \
  rtems_rfs_read_u32 (_e + RTEMS_RFS_DIR_ENTRY_HASH)

/**
 * Set the hash of the entry.
 *
 * @param _e Pointer to the directory entry.
 * @param _h The hash.
 */
#define rtems_rfs_dir_set_entry_hash(_e, _h) \
  rtems_rfs_write_u32 (_e + RTEMS_RFS_DIR_ENTRY_HASH, _h)

/**
 * Return the ino of the entry.
 *
 * @param _e Pointer to the directory entry.
 * @return uint32_t The ino.
 */
#define rtems_rfs_dir_entry_ino(_e) \
  rtems_rfs_read_u32 (_e + RTEMS_RFS_DIR_ENTRY_INO)

/**
 * Set the ino of the entry.
 *
 * @param _e Pointer to the directory entry.
 * @param _i The ino.
 */
#define rtems_rfs_dir_set_entry_ino(_e, _i) \
  rtems_rfs_write_u32 (_e + RTEMS_RFS_DIR_ENTRY_INO, _i)

/**
 * Return the length of the entry.
 *
 * @param _e Pointer to the directory entry.
 * @return uint16_t The length.
 */
#define rtems_rfs_dir_entry_length(_e) \
  rtems_rfs_read_u16 (_e + RTEMS_RFS_DIR_ENTRY_LEN)

/**
 * Set the length of the entry.
 *
 * @param _e Pointer to the directory entry.
 * @param _l The length.
 */
#define rtems_rfs_dir_set_entry_length(_e, _l) \
  rtems_rfs_write_u16 (_e + RTEMS_RFS_DIR_ENTRY_LEN, _l)

/**
 * Look up a directory entry in the directory pointed to by the inode. The look
 * up is local to this directory. No need to decend.
 *
 * @param fs The file system.
 * @param inode The inode of the directory to search.
 * @param name The name to look up. The name may not be nul terminated.
 * @param length The length of the name.
 * @param ino The return inode number if there is no error.
 * @param offset The offset in the directory for the entry.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_dir_lookup_ino (rtems_rfs_file_system*  fs,
                              rtems_rfs_inode_handle* inode,
                              const char*             name,
                              int                     length,
                              rtems_rfs_ino*          ino,
                              uint32_t*               offset);

/**
 * Add an entry to the directory returing the inode number allocated to the
 * entry.
 *
 * @param fs The file system data.
 * @param dir Pointer to the directory inode the entry is to be added too.
 * @param name The name of the entry to be added.
 * @param length The length of the name excluding a terminating 0.
 * @param ino The ino of the entry.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_dir_add_entry (rtems_rfs_file_system*  fs,
                             rtems_rfs_inode_handle* dir,
                             const char*             name,
                             size_t                  length,
                             rtems_rfs_ino           ino);

/**
 * Del an entry from the directory using an inode number as a key.
 *
 * @param fs The file system data.
 * @param dir Pointer to the directory inode the entry is to be deleted from.
 * @param ino The ino of the entry.
 * @param offset The offset in the directory of the entry to delete. If 0
 *               search from the start for the ino.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_dir_del_entry (rtems_rfs_file_system*  fs,
                             rtems_rfs_inode_handle* dir,
                             rtems_rfs_ino           ino,
                             uint32_t                offset);

/**
 * Read the directory entry from offset into the directory entry buffer and
 * return the length of space this entry uses in the directory table.
 *
 * @param fs The file system data.
 * @param dir The direct inode handler.
 * @param offset The offset in the directory to read from.
 * @param dirent Pointer to the dirent structure the entry is written into.
 * @param length Set the length this entry takes in the directory.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_dir_read (rtems_rfs_file_system*  fs,
                        rtems_rfs_inode_handle* dir,
                        rtems_rfs_pos_rel       offset,
                        struct dirent*          dirent,
                        size_t*                 length);

/**
 * Check if the directory is empty. The current and parent directory entries
 * are ignored.
 *
 * @param fs The file system data
 * @param dir The directory inode to check.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_dir_empty (rtems_rfs_file_system*  fs,
                         rtems_rfs_inode_handle* dir);

#endif
