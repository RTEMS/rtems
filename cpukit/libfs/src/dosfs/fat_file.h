/*
 *  fat_file.h
 *
 *  Constants/data structures/prototypes for operations on "fat-file"
 *
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#ifndef __DOSFS_FAT_FILE_H__
#define __DOSFS_FAT_FILE_H__

#include <rtems.h>
#include <rtems/libio_.h>

#include <time.h>

#include "fat.h"

#ifdef __cplusplus
extern "C" {
#endif

/* "fat-file" representation
 *
 * the idea is: fat-file is nothing but a cluster chain, any open fat-file is
 * represented in system by fat-file descriptor and has well-known
 * file interface:
 *
 * fat_file_open()
 * fat_file_close()
 * fat_file_read()
 * fat_file_write()
 *
 * Such interface hides the architecture of fat-file and represents it like
 * linear file
 */

typedef rtems_filesystem_node_types_t fat_file_type_t;

#define FAT_DIRECTORY     RTEMS_FILESYSTEM_DIRECTORY
#define FAT_FILE          RTEMS_FILESYSTEM_MEMORY_FILE

typedef struct fat_file_map_s
{
    uint32_t   file_cln;
    uint32_t   disk_cln;
    uint32_t   last_cln;
} fat_file_map_t;
/*
 * descriptor of a fat-file
 *
 * To each particular clusters chain
 */
typedef struct fat_file_fd_s
{
    rtems_chain_node link;          /*
                                     * fat-file descriptors organized into hash;
                                     * collision lists are handled via link
                                     * field
                                     */
    uint32_t         links_num;     /*
                                     * the number of fat_file_open call on
                                     * this fat-file
                                     */
    uint32_t         ino;           /* inode, file serial number :)))) */
    fat_file_type_t  fat_file_type;
    uint32_t         size_limit;
    uint32_t         fat_file_size; /* length  */
    uint32_t         cln;
    fat_dir_pos_t    dir_pos;
    uint8_t          flags;
    fat_file_map_t   map;
    time_t           mtime;

} fat_file_fd_t;

#define FAT_FILE_REMOVED  0x01

#define FAT_FILE_IS_REMOVED(p)\
    (((p)->flags & FAT_FILE_REMOVED) ? 1 : 0)

/* ioctl macros */
#define F_CLU_NUM  0x01

/*
 * Each file and directory on a MSDOS volume is unique identified by it
 * location, i.e. location of it 32 Bytes Directory Entry Structure. We can
 * distinguish them by cluster number it locates on and offset inside this
 * cluster. But root directory on any volumes (FAT12/16/32) has no 32 Bytes
 * Directory Entry Structure corresponded to it. So we assume 32 Bytes
 * Directory Entry Structure of root directory locates at cluster 1 (invalid
 * cluaster number) and offset 0
 */
#define FAT_ROOTDIR_CLUSTER_NUM 0x01

#define FAT_FD_OF_ROOT_DIR(fat_fd)  \
  ((fat_fd->dir_pos.sname.cln == FAT_ROOTDIR_CLUSTER_NUM) && \
  (fat_fd->dir_pos.sname.ofs == 0))

#define FAT_EOF           0x00

/* fat_construct_key --
 *     Construct key for hash access: convert (cluster num, offset) to
 *     (sector512 num, new offset) and than construct key as
 *     key = (sector512 num) << 4 | (new offset)
 *
 * PARAMETERS:
 *     cl       - cluster number
 *     ofs      - offset inside cluster 'cl'
 *     fs_info  - FS info
 *
 * RETURNS:
 *     constructed key
 */
static inline uint32_t
fat_construct_key(
    const fat_fs_info_t                  *fs_info,
    fat_pos_t                            *pos)
{
    return ( ((fat_cluster_num_to_sector512_num(fs_info, pos->cln) +
              (pos->ofs >> FAT_SECTOR512_BITS)) << 4)              +
              ((pos->ofs >> 5) & (FAT_DIRENTRIES_PER_SEC512 - 1)) );
}

/* Prototypes for "fat-file" operations */
int
fat_file_open(fat_fs_info_t                         *fs_info,
              fat_dir_pos_t                         *dir_pos,
              fat_file_fd_t                        **fat_fd);

int
fat_file_reopen(fat_file_fd_t *fat_fd);

int
fat_file_close(fat_fs_info_t                        *fs_info,
               fat_file_fd_t                        *fat_fd);

ssize_t
fat_file_read(fat_fs_info_t                        *fs_info,
              fat_file_fd_t                        *fat_fd,
              uint32_t                              start,
              uint32_t                              count,
              uint8_t                              *buf);

ssize_t
fat_file_write(fat_fs_info_t                        *fs_info,
               fat_file_fd_t                        *fat_fd,
               uint32_t                              start,
               uint32_t                              count,
               const uint8_t                        *buf);

int
fat_file_extend(fat_fs_info_t                        *fs_info,
                fat_file_fd_t                        *fat_fd,
                bool                                  zero_fill,
                uint32_t                              new_length,
                uint32_t                             *a_length);

int
fat_file_truncate(fat_fs_info_t                        *fs_info,
                  fat_file_fd_t                        *fat_fd,
                  uint32_t                              new_length);

int
fat_file_ioctl(fat_fs_info_t                        *fs_info,
               fat_file_fd_t                        *fat_fd,
               int                                   cmd,
               ...);

int
fat_file_size(fat_fs_info_t                        *fs_info,
              fat_file_fd_t                        *fat_fd);

void
fat_file_mark_removed(fat_fs_info_t                        *fs_info,
                      fat_file_fd_t                        *fat_fd);

#ifdef __cplusplus
}
#endif

#endif /* __DOSFS_FAT_FILE_H__ */
