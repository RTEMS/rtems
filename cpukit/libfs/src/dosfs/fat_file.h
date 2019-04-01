/**
 * @file
 *
 * @brief Constants/Data Structures/Prototypes for Operations on "fat-file"
 *
 * @ingroup libfs_ff
 */

/*
 *
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __DOSFS_FAT_FILE_H__
#define __DOSFS_FAT_FILE_H__

#include <rtems.h>
#include <rtems/libio_.h>

#include <time.h>

#include "fat.h"

/**
 *  @defgroup libfs_ff Fat File
 *
 *  @ingroup libfs_dosfs
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  FAT_DIRECTORY = 0,
  FAT_HARD_LINK = 2, /* pseudo type */
  FAT_FILE = 4
} fat_file_type_t;

/**
 * @brief The "fat-file" representation.
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
typedef struct fat_file_map_s
{
    uint32_t   file_cln;
    uint32_t   disk_cln;
    uint32_t   last_cln;
} fat_file_map_t;

/**
 * @brief Descriptor of a fat-file.
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
    time_t           ctime;
    time_t           mtime;

} fat_file_fd_t;

#define FAT_FILE_REMOVED 0x01

#define FAT_FILE_META_DATA_CHANGED 0x02

static inline bool FAT_FILE_IS_REMOVED(const fat_file_fd_t *fat_fd)
{
     return (fat_fd->flags & FAT_FILE_REMOVED) != 0;
}

static inline bool FAT_FILE_HAS_META_DATA_CHANGED(const fat_file_fd_t *fat_fd)
{
     return (fat_fd->flags & FAT_FILE_META_DATA_CHANGED) != 0;
}

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

/* @brief Construct key for hash access.
 *
 * Construct key for hash access: convert (cluster num, offset) to
 * (sector512 num, new offset) and than construct key as
 * key = (sector512 num) << 4 | (new offset)
 *
 * @param[in] cl - cluster number
 * @param[in] ofs - offset inside cluster 'cl'
 * @param[in] fs_info - FS info
 *
 * @retval constructed key
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

static inline void
fat_file_set_first_cluster_num(fat_file_fd_t *fat_fd, uint32_t cln)
{
    fat_fd->cln = cln;
    fat_fd->flags |= FAT_FILE_META_DATA_CHANGED;
}

static inline void fat_file_set_file_size(fat_file_fd_t *fat_fd, uint32_t s)
{
    fat_fd->fat_file_size = s;
    fat_fd->flags |= FAT_FILE_META_DATA_CHANGED;
}

static inline void fat_file_set_ctime(fat_file_fd_t *fat_fd, time_t t)
{
    fat_fd->ctime = t;
    fat_fd->flags |= FAT_FILE_META_DATA_CHANGED;
}

static inline void fat_file_set_mtime(fat_file_fd_t *fat_fd, time_t t)
{
    fat_fd->mtime = t;
    fat_fd->flags |= FAT_FILE_META_DATA_CHANGED;
}

static inline void fat_file_set_ctime_mtime(fat_file_fd_t *fat_fd, time_t t)
{
    fat_fd->ctime = t;
    fat_fd->mtime = t;
    fat_fd->flags |= FAT_FILE_META_DATA_CHANGED;
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

int
fat_file_size(fat_fs_info_t                        *fs_info,
              fat_file_fd_t                        *fat_fd);

int
fat_file_write_first_cluster_num(fat_fs_info_t *fs_info,
                                 fat_file_fd_t *fat_fd);

int
fat_file_write_file_size(fat_fs_info_t *fs_info,
                         fat_file_fd_t *fat_fd);

int
fat_file_write_time_and_date(fat_fs_info_t *fs_info,
                             fat_file_fd_t *fat_fd);

int
fat_file_update(fat_fs_info_t *fs_info,
                fat_file_fd_t *fat_fd);

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* __DOSFS_FAT_FILE_H__ */
