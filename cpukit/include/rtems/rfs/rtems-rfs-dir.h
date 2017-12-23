/**
 * @file
 *
 * @brief RTEMS File System Directory Support
 *
 * @ingroup rtems_rfs
 *
 * RTEMS File System Directory Support
 *
 * This file provides the directory support functions.
 */

/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
 * @param[in] _e is a pointer to the directory entry.
 *
 * @retval hash The uint32_t hash of the entry.
 */
#define rtems_rfs_dir_entry_hash(_e) \
  rtems_rfs_read_u32 (_e + RTEMS_RFS_DIR_ENTRY_HASH)

/**
 * Set the hash of the entry.
 *
 * @param[in] _e is a pointer to the directory entry.
 *
 * @param[in] _h is the hash of the entry.
 */
#define rtems_rfs_dir_set_entry_hash(_e, _h) \
  rtems_rfs_write_u32 (_e + RTEMS_RFS_DIR_ENTRY_HASH, _h)

/**
 * Return the ino of the entry.
 *
 * @param[in] _e is a pointer to the directory entry.
 *
 * @retval ino The ino of the entry.
 */
#define rtems_rfs_dir_entry_ino(_e) \
  rtems_rfs_read_u32 (_e + RTEMS_RFS_DIR_ENTRY_INO)

/**
 * Set the ino of the entry.
 *
 * @param[in] _e is a pointer to the directory entry.
 *
 * @param[in] _i is the ino of the entry.
 */
#define rtems_rfs_dir_set_entry_ino(_e, _i) \
  rtems_rfs_write_u32 (_e + RTEMS_RFS_DIR_ENTRY_INO, _i)

/**
 * Return the length of the entry.
 *
 * @param[in] _e Pointer to the directory entry.
 *
 * @retval length The length of the entry.
 */
#define rtems_rfs_dir_entry_length(_e) \
  rtems_rfs_read_u16 (_e + RTEMS_RFS_DIR_ENTRY_LEN)

/**
 * Set the length of the entry.
 *
 * @param[in] _e is a pointer to the directory entry.
 * @param[in] _l is the length.
 */
#define rtems_rfs_dir_set_entry_length(_e, _l) \
  rtems_rfs_write_u16 (_e + RTEMS_RFS_DIR_ENTRY_LEN, _l)

/**
 * Look up a directory entry in the directory pointed to by the inode. The look
 * up is local to this directory. No need to decend.
 *
 * @param[in] fs is the file system.
 * @param[in] inode is a pointer to the inode of the directory to search.
 * @param[in] name is a pointer to the name to look up. The name may not be
 *             nul terminated.
 * @param[in] length is the length of the name.
 * @param[out] ino will be filled in with the inode number
 *              if there is no error.
 * @param[in] offset is the offset in the directory for the entry.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
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
 * @param[in] fs is the file system data.
 * @param[in] dir is a pointer to the directory inode the
 *             entry is to be added too.
 * @param[in] name is a pointer to the name of the entry to be added.
 * @param[in] length is the length of the name excluding a terminating 0.
 * @param[in] ino is the ino of the entry.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
 */
int rtems_rfs_dir_add_entry (rtems_rfs_file_system*  fs,
                             rtems_rfs_inode_handle* dir,
                             const char*             name,
                             size_t                  length,
                             rtems_rfs_ino           ino);

/**
 * Del an entry from the directory using an inode number as a key.
 *
 * @param[in] fs is the file system data.
 * @param[in] dir is a pointer to the directory inode the
 * entry is to be deleted from.
 * @param[in] ino is the ino of the entry.
 * @param[in] offset is the offset in the directory of the entry
 *               to delete. If 0  search from the start for the ino.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
 */
int rtems_rfs_dir_del_entry (rtems_rfs_file_system*  fs,
                             rtems_rfs_inode_handle* dir,
                             rtems_rfs_ino           ino,
                             uint32_t                offset);

/**
 * Read the directory entry from offset into the directory entry buffer and
 * return the length of space this entry uses in the directory table.
 *
 * @param[in] fs is the file system data.
 * @param[in] dir is a pointer to the direct inode handler.
 * @param[in] offset is the offset in the directory to read from.
 * @param[in] dirent is a ointer to the dirent structure the entry
 *              is written into.
 * @param[out] length will contain the length this entry
 *               takes in the directory.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
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
 * @param[in] fs is the file system data
 * @param[in] dir is a pointer to the directory inode to check.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
 */
int rtems_rfs_dir_empty (rtems_rfs_file_system*  fs,
                         rtems_rfs_inode_handle* dir);

#endif
